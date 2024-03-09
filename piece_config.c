#include "piece_config.h"
#include "malloc.h"
#include "strings.h"
#include "uart.h"

static struct {
    int nrows;
    int ncols;
    color_t background = GL_INDIGO;
    void* background_tracker;
} game_config;

// Required init 
void piece_config_init(int nrows, int ncols) {
    if (game_config.background_tracker != NULL) free(game_config.background_tracker);
    game_config.nrows = nrows;
    game_config.ncols = ncols;

    int nbytes = game_config.nrows * game_config.ncols;
    game_config.background_tracker = malloc(nbytes);
    memset(game_config.background_tracker, '\0', nbytes);
}

typedef bool (*functionPtr)(int x, int y); 

bool iterateThroughPieceSquares(falling_piece_t piece, functionPtr action) {
    int piece_config = (piece.pieceT).block_rotations[piece.rotation];

    // pieceRow and pieceCol will change from 0 through 3 during loop
    // Indicates where on 4x4 piece grid we are checking
    int pieceRow = 0; int pieceCol = 0;  
    for (int bitSequence = 0x8000; bit > 0; bit = bit >> 1) {
        // if piece has a square in the 4x4 grid spot we're checking, perform action fn on that square
        if (piece_config & bitSequence) {
            if (!action(piece.x + pieceCol, piece.y + pieceRow)) return false;
        }
        pieceCol++;
        // move to next row in grid, reset pieceCol to 0
        if (pieceCol == 4) {
            pieceCol = 0; pieceRow++;
        }
    }
    return true;
}

bool checkIfValidMove(int x, int y) {
    
}

// returns true if piece is in fallen state (i.e. square below one of the squares is occupied)
bool drawSquare(int x, int y) {
    // piece should be drawn into background
}

// iterate through piece squares
// check if fallen in place -- bool
// draw piece -- bool
// 
void move_down(falling_piece_t piece) {

}

void move_left(falling_piece_t piece) {

}

void move_right(falling_piece_t piece) {

}

void rotate(falling_piece_t piece) {

}

