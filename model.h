/**
 * @brief Implements the Reversi game model
 * @author Marc S. Ressl
 *
 * @copyright Copyright (c) 2023-2024
 */

#ifndef MODEL_H
#define MODEL_H

#include <cstdint>
#include <vector>

#define BOARD_SIZE 8

enum Player
{
    PLAYER_BLACK,
    PLAYER_WHITE,
};

enum Piece
{
    PIECE_EMPTY,
    PIECE_BLACK,
    PIECE_WHITE,
};

struct Square
{
    int x;
    int y;
    uint64_t index;
};

#define GAME_INVALID_SQUARE \
    {                       \
        -1, -1              \
    }

struct GameModel
{
    bool gameOver;

    Player currentPlayer;

    double playerTime[2];
    double turnTimer;

    // Piece board[BOARD_SIZE][BOARD_SIZE]; //no la usamos, usamos bitboards ya que son más eficientes y rápdios que una matriz
    // 0 = empty, 1 = black, 2 = white

    uint64_t black = 0ULL; // bitboard para negras
    uint64_t white = 0ULL; // bitboard para blancas

    /*
    0  1  2  3  4  5  6  7
    8  9 10 11 12 13 14 15
    16 17 18 19 20 21 22 23
    24 25 26 27 28 29 30 31
    32 33 34 35 36 37 38 39
    40 41 42 43 44 45 46 47
    48 49 50 51 52 53 54 55
    56 57 58 59 60 61 62 63

    Cada número representa una posición en el tablero.
    Si el bit en esa posición es 1, hay una ficha del color correspondiente.
    Si es 0, no hay ficha.
    */

    Player humanPlayer;

public:
    // Colocar una pieza de un jugador en la posición `pos`
    void placePiece(Player p, int pos) {
        if (p == PLAYER_BLACK) {
            black = setBit(black, pos);
            white = clearBit(white, pos);
        }
        else {
            white = setBit(white, pos);
            black = clearBit(black, pos);
        }
    }
    // Verifica si un jugador tiene pieza en `pos`
    bool hasPiece(Player p, int pos) const {
        if (p == PLAYER_BLACK) return getBit(black, pos);
        else return getBit(white, pos);
    }
    // Borra cualquier pieza (blanca o negra) en `pos`
    void removePiece(int pos) {
        black = clearBit(black, pos);
        white = clearBit(white, pos);
    }
    Piece getPiece(int pos) {
        if (getBit(black, pos)) return PIECE_BLACK;
        else if (getBit(white, pos)) return PIECE_WHITE;
        else return PIECE_EMPTY;
	}

private:
     // ---- Utilidades de bitboard ----
    uint64_t setBit(uint64_t board, int pos) const {
        return board | (1ULL << pos);
    }

    uint64_t clearBit(uint64_t board, int pos) const {
        return board & ~(1ULL << pos);
    }

    bool getBit(uint64_t board, int pos) const {
        return (board >> pos) & 1ULL;
    }

};

typedef std::vector<Square> Moves;

/**
 * @brief Initializes a game model.
 *
 * @param model The game model.
 */
void initModel(GameModel &model);

/**
 * @brief Starts a game.
 *
 * @param model The game model.
 */
void startModel(GameModel &model);

/**
 * @brief Returns the model's current player.
 *
 * @param model The game model.
 * @return PLAYER_WHITE or PLAYER_BLACK.
 */
Player getCurrentPlayer(GameModel &model);

/**
 * @brief Returns the model's current score.
 *
 * @param model The game model.
 * @param player The player (PLAYER_WHITE or PLAYER_BLACK).
 * @return The score.
 */
int getScore(GameModel &model, Player player);

/**
 * @brief Returns the game timer for a player.
 *
 * @param model The game model.
 * @param player The player (PLAYER_WHITE or PLAYER_BLACK).
 * @return The time in seconds.
 */
double getTimer(GameModel &model, Player player);

/**
 * @brief Return a model's piece.
 *
 * @param model The game model.
 * @param square The square.
 * @return The piece at the square.
 */
Piece getBoardPiece(GameModel& model, Square square);

/**
 * @brief Sets a model's piece.
 *
 * @param model The game model.
 * @param square The square.
 * @param piece The piece to be set
 */
void setBoardPiece(GameModel& model, Square square, Piece piece);


/**
 * @brief Checks whether a square is within the board.
 *
 * @param square The square.
 * @return True or false.
 */
bool isSquareValid(Square square);

/**
 * @brief Returns a list of valid moves for the current player.
 *
 * @param model The game model.
 * @param validMoves A list that receives the valid moves.
 */
void getValidMoves(GameModel &model, Moves &validMoves, uint64_t black_board, uint64_t white_board);

/**
 * @brief Plays a move.
 *
 * @param model The game model.
 * @param square The move.
 * @return Move accepted.
 */
bool playMove(GameModel &model, Square move);

#endif
