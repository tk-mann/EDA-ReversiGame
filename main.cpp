#include "model.h"
#include "view.h"
#include "controller.h"

int main()
{
    GameModel model;

    initModel(model);
    initView();

    while (updateView(model))
        ;

    freeView();
}





/*
#include <iostream>
#include <bitset>
#include <cstdint>
using namespace std;

// Mostrar un bitboard en formato tablero 8x8
void printBoard(uint64_t bb) {
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            int pos = row * 8 + col;
            cout << ((bb >> pos) & 1ULL ? "1 " : ". ");
        }
        cout << endl;
    }
    cout << endl;
}

// Poner ficha en posición (row, col)
void setBit(uint64_t& bb, int row, int col) {
    int pos = row * 8 + col;
    bb |= (1ULL << pos);
}

// Quitar ficha
void clearBit(uint64_t& bb, int row, int col) {
    int pos = row * 8 + col;
    bb &= ~(1ULL << pos);
}

// Comprobar si hay ficha
bool isSet(uint64_t bb, int row, int col) {
    int pos = row * 8 + col;
    return (bb >> pos) & 1ULL;
}



int main() {
    uint64_t black = 0; // bitboard para negras
    uint64_t white = 0; // bitboard para blancas

    // Colocamos la posición inicial de Othello
    setBit(white, 3, 3);
    setBit(white, 4, 4);
    setBit(black, 3, 4);
    setBit(black, 4, 3);

    cout << "Negras:" << endl;
    printBoard(black);

    cout << "Blancas:" << endl;
    printBoard(white);

    // Ejemplo: colocar una ficha negra en (2,3)
    setBit(black, 2, 3);
    cout << "Negras despues de mover en (2,3):" << endl;
    printBoard(black);
}
*/