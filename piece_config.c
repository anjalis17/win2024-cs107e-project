#include "piece_config.h"
#include "malloc.h"
#include "strings.h"
#include "uart.h"

static struct {
    int nrows;
    int ncols;
    color_t background = GL_INDIGO;
    void* background_tracker;
} config;

void piece_config_init(int nrows, int ncols, color_t background) {

}

// numRows, numCols = dimensions of game board
bool checkIfValidMove(falling_piece_t piece) {

}

bool checkIfFallenInPlace(falling_piece_t piece) {
    // piece should be drawn into background
}

void move_down(falling_piece_t piece) {

}

void move_left(falling_piece_t piece) {

}

void move_right(falling_piece_t piece) {

}

void rotate(falling_piece_t piece) {

}

