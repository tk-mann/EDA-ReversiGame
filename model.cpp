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



void getValidMoves(GameModel& model, Moves& validMoves, uint64_t black_board, uint64_t white_board)
{
    validMoves.clear();
    // direcciones: desplazamientos en el tablero
    int directions[8] = { 1, -1, 8, -8, 7, -7, 9, -9 }; /*1: derecha, -1: izquierda, 8: abajo, -8: arriba7: diagonal arriba izquierda, -7 : diagonal abajo derecha9: diagonal arriba derecha, -9: diagonal abajo izquierda */
    uint64_t empty = ~(black_board | white_board); // casillas vacías
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

    for (int i = 0; i < 8; i++) {
        int direction = directions[i];
        uint64_t mask = 0ULL;
        uint64_t possible = 0;
        if (direction > 0) {
            mask = myBoard << direction;
            for (int i = 0; i < 5; i++) {
                mask &= (myBoard << direction * (i + 1));
            }
            possible = (opponentBoard << direction);
        }
        else {
            mask = myBoard >> -direction;
            for (int i = 0; i < 5; i++) {
                mask &= (myBoard >> -direction * (i + 1));
            }
            possible = (opponentBoard >> -direction);
        }
        validBits |= empty & mask & possible;

        while (validBits) {
            int index = ctz64_simple(validBits); // índice del bit menos significativo que está a 1
            validBits &= validBits - 1;            // borro ese bit

            int x = index / 8; // fila
            int y = index % 8; // columna
            Square move = { x, y };
            validMoves.push_back(move);
        }

    }
}
bool playMove(GameModel &model, Square move)
{
    // Set game piece
    Piece piece =
        (getCurrentPlayer(model) == PLAYER_WHITE)
            ? PIECE_WHITE
            : PIECE_BLACK;

	int pos = move.x * 8 + move.y;
	setBoardPiece(model, getCurrentPlayer(model), pos);

    // To-do: your code goes here...

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
