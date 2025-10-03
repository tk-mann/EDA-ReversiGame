/**
 * @brief Implements the Reversi game model
 * @author Marc S. Ressl
 *
 * @copyright Copyright (c) 2023-2024
 */

#include "raylib.h"

#include "model.h"
#include <iostream>
#include <cstdint>

#include <iostream>
#include <bitset>
#include <cstdint>
using namespace std;

const uint64_t NOT_A_FILE = 0xFEFEFEFEFEFEFEFEULL; // columna izquierda
const uint64_t NOT_H_FILE = 0x7F7F7F7F7F7F7F7FULL; // columna derecha+



void initModel(GameModel& model)
{
    model.gameOver = true;

    model.playerTime[0] = 0;
    model.playerTime[1] = 0;

    for (int x = 0; x < BOARD_SIZE; x++){
        for (int y = 0; y < BOARD_SIZE; y++){
			int pos = x * 8 + y;
			model.removePiece(pos);
        }
    }
}

void startModel(GameModel& model)
{
    model.gameOver = false;

    model.currentPlayer = PLAYER_BLACK;

    model.playerTime[0] = 0;
    model.playerTime[1] = 0;
    model.turnTimer = GetTime();

    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            int pos = x * 8 + y;
            model.removePiece(pos);
        }
    }

	model.placePiece(PLAYER_WHITE, BOARD_SIZE / 2 * BOARD_SIZE + BOARD_SIZE / 2);
	model.placePiece(PLAYER_WHITE, (BOARD_SIZE / 2 - 1) * BOARD_SIZE + (BOARD_SIZE / 2 - 1));
	model.placePiece(PLAYER_BLACK, (BOARD_SIZE / 2 - 1) * BOARD_SIZE + (BOARD_SIZE / 2));
	model.placePiece(PLAYER_BLACK, (BOARD_SIZE / 2) * BOARD_SIZE + (BOARD_SIZE / 2 - 1));
}

Player getCurrentPlayer(GameModel &model)
{
    return model.currentPlayer;
}

Piece getBoardPiece(GameModel& model, int pos)
{
    return model.getPiece(pos);
}

void setBoardPiece(GameModel& model, Player player, int pos)
{
    model.placePiece(player, pos);
}

int getScore(GameModel &model, Player player)
{
    int score = 0;

    for (int x = 0; x < BOARD_SIZE; x++) 
        for (int y = 0; y < BOARD_SIZE; y++) {
            int pos = x * 8 + y;
            if (((model.hasPiece(PLAYER_WHITE, pos)) && (player == PLAYER_WHITE)) ||
                ((model.hasPiece(PLAYER_BLACK, pos)) && (player == PLAYER_BLACK))) {
                score++;
            }

        }

    return score;
}

double getTimer(GameModel &model, Player player)
{
    double turnTime = 0;

    if (!model.gameOver && (player == model.currentPlayer))
        turnTime = GetTime() - model.turnTimer;

    return model.playerTime[player] + turnTime;
}

bool isSquareValid(Square square)
{
    return (square.x >= 0) &&
           (square.x < BOARD_SIZE) &&
           (square.y >= 0) &&
           (square.y < BOARD_SIZE);
}

int ctz64_simple(uint64_t x) {
    int count = 0;
    while (x && (x & 1) == 0) {
        x >>= 1;
        count++;
    }
    return count;
}

void check_if_legal(uint64_t a, uint64_t &b) {

    if (a == 1 || a == 9 || a == -7) {
        b &= NOT_H_FILE;
    }
    if (a == -1 || a == 7 || a == -9) {
        b &= NOT_A_FILE;
    }

}

void printBoard(uint64_t board) {
    std::cout << "\nBitboard visualization:\n";
    std::cout << "  0 1 2 3 4 5 6 7\n";
    std::cout << "  ---------------\n";

    for (int row = 0; row < 8; row++) {
        std::cout << row << "|";
        for (int col = 0; col < 8; col++) {
            int index = row * 8 + col;
            uint64_t bit = 1ULL << index;
            std::cout << ((board & bit) ? "X " : ". ");
        }
        std::cout << "| (índices " << row * 8 << "-" << (row * 8 + 7) << ")\n";
    }
    std::cout << "  ---------------\n";
}


void getValidMoves(GameModel& model, Moves& validMoves, uint64_t black_board, uint64_t white_board)
{
    validMoves.clear();
    // direcciones: desplazamientos en el tablero
    int directions[8] = {
    1,   // Este ()      - derecha
    -1,  // Oeste ()     - izquierda
    8,   // Sur ()       - abajo
    -8,  // Norte      - arriba
    7,   // Suroeste  - diagonal abajo-izquierda
    -7,  // Noreste    - diagonal arriba-derecha
    9,   // Sureste   - diagonal abajo-derecha
    -9   // Noroeste   - diagonal arriba-izquierda
    };
    uint64_t empty = ~(black_board | white_board); // casillas vacías
    uint64_t prev_mask = 0ULL;
    uint64_t validBits = 0ULL;
    uint64_t myBoard = 0ULL;
    uint64_t opponentBoard = 0ULL;
    if (getCurrentPlayer(model) == PLAYER_BLACK) {
        myBoard = black_board;
        opponentBoard = white_board;
    }
    else
    {
        myBoard = white_board;
        opponentBoard = black_board;
    }

    for(int i = 0; i < 8; i++) {
        int direction = directions[i];
        uint64_t mask = direction > 0 ? (myBoard << direction) : (myBoard >> (-direction));
		//check_if_legal(direction, mask);
        int count = 1;
        uint64_t candidates = 0ULL;
        mask &= opponentBoard;
        //printBoard(mask);
        while (mask){
            prev_mask = mask;
            mask = direction > 0 ? (mask << (direction)) : (mask >> (-(direction)));
			check_if_legal(direction, mask);
          //  printBoard(mask);
            candidates = mask;
            validBits |= candidates & empty;
            mask &= opponentBoard;
            count++;
        }

		//printBoard(candidates);
        validBits |= candidates & empty;
	}
    //ME FALTARÍA SOLO ARREGLAR LOS CASOS LÍMITES
    while (validBits) {
       // printBoard(validBits);
        uint64_t index = ctz64_simple(validBits); // índice del bit menos significativo que está a 1
        validBits &= validBits - 1ULL;
        int fila = index / 8; // fila
        int columna = index % 8; // coumna
        cout << "Valid move: " << fila << "," << columna<< endl;
        Square move = { fila, columna, index };
        validMoves.push_back(move);
    }
}


bool playMove(GameModel& model, Square move)
{
    vector<int> piezas_comidas;
    vector<int> piezas_potencial;
    // Set game piece
    Piece piece =
        (getCurrentPlayer(model) == PLAYER_WHITE)
        ? PIECE_WHITE
        : PIECE_BLACK;
    cout << "Move played: " << move.x << "," << move.y << endl;
    uint64_t pos = move.x * 8 + move.y;
    cout << pos << endl;
    uint64_t myBoard = 0ULL;
    uint64_t opponentBoard = 0ULL;
    uint64_t piezas_posibles = 0ULL;

    setBoardPiece(model, getCurrentPlayer(model), pos);

    int directions[8] = {
    1,   // Este ()      - derecha
    -1,  // Oeste ()     - izquierda
    8,   // Sur ()       - abajo
    -8,  // Norte      - arriba
    7,   // Suroeste  - diagonal abajo-izquierda
    -7,  // Noreste    - diagonal arriba-derecha
    9,   // Sureste   - diagonal abajo-derecha
    -9   // Noroeste   - diagonal arriba-izquierda
    }; if (getCurrentPlayer(model) == PLAYER_BLACK) {
        myBoard = model.black;
        opponentBoard = model.white;
    }
    else
    {
        myBoard = model.white;
        opponentBoard = model.black;
    }

    int longitud = 0;

    for (int i = 0; i < 8; i++) {
        int direction = directions[i];
        uint64_t mask = 0ULL;
        uint64_t piezas_potenciales = 0ULL;
        mask = 1ULL << pos;
        mask = direction > 0 ? (mask << direction) : (mask >> (-direction));
        uint64_t mask_enemy = mask & opponentBoard;
        uint64_t mask_own = mask & myBoard;
        bool potencial_pieza = false;
        while (mask_enemy || mask_own) {
            if (mask_enemy != 0) {
                piezas_potenciales |= mask_enemy;
                uint64_t index = ctz64_simple(piezas_potenciales); // índice del bit menos significativo que está a 1
                int fila = index / 8; // fila
                int columna = index % 8; // columna
                potencial_pieza = true;
            }
            else if (mask_own != 0 && potencial_pieza) {
                while (piezas_potenciales) {
                    uint64_t index = ctz64_simple(piezas_potenciales); // índice del bit menos significativo que está a 1
                    piezas_potenciales &= piezas_potenciales - 1ULL;
                    int fila = index / 8; // fila
                    int columna = index % 8; // coumna
                    piezas_comidas.push_back(fila * 8 + columna);
                    longitud++;

                }
                potencial_pieza = false;
                break;
            }
            mask = direction > 0 ? (mask << direction) : (mask >> (-direction));
            mask_enemy = mask & opponentBoard;
            mask_own = mask & myBoard;
        }
    }

    for (int i = 0; i < longitud; i++) {
        setBoardPiece(model, getCurrentPlayer(model), piezas_comidas[i]);
    }

    if (getCurrentPlayer(model) == PLAYER_BLACK) {
        myBoard = model.black;
        opponentBoard = model.white;
    }
    else
    {
        myBoard = model.white;
        opponentBoard = model.black;
    }
	printBoard(myBoard);
    printBoard(opponentBoard);
    // To-do: your code goes here...
    myBoard = model.black;
    opponentBoard = model.white;


    // Update timer
    double currentTime = GetTime();
    model.playerTime[model.currentPlayer] += currentTime - model.turnTimer;
    model.turnTimer = currentTime;

    // Swap player
    model.currentPlayer =
        (model.currentPlayer == PLAYER_WHITE)
            ? PLAYER_BLACK
            : PLAYER_WHITE;

    // Game over?
    Moves validMoves;
    getValidMoves(model, validMoves, model.black, model.white);

    if (validMoves.size() == 0)
    {
        // Swap player
        model.currentPlayer =
            (model.currentPlayer == PLAYER_WHITE)
                ? PLAYER_BLACK
                : PLAYER_WHITE;

        Moves validMoves;
        getValidMoves(model, validMoves, model.black, model.white);

        if (validMoves.size() == 0)
            model.gameOver = true;
    }

    return true;
}
