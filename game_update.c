#include "game_update.h"
#include "malloc.h"
#include "strings.h"
#include "uart.h"

static struct {
    int nrows;
    int ncols;
    color_t background = GL_INDIGO;
    unsigned int (*background_tracker)[module.ncols];  
} game_config;

const unsigned int SQUARE_DIM = 50; // game pixel dimensions

// Required init 
void piece_config_init(int nrows, int ncols) {
    if (game_config.background_tracker != NULL) free(game_config.background_tracker);
    game_config.nrows = nrows;
    game_config.ncols = ncols;

    int nbytes = game_config.nrows * game_config.ncols;
    game_config.background_tracker = malloc(nbytes);
    memset(game_config.background_tracker, 0, nbytes);

    // need gl init here
}

//////////////// STATICS 
typedef bool (*functionPtr)(int x, int y, color_t color); 

bool iterateThroughPieceSquares(falling_piece_t piece, functionPtr action) {
    int piece_config = (piece.pieceT).block_rotations[piece.rotation];

    // pieceRow and pieceCol will change from 0 through 3 during loop
    // Indicates where on 4x4 piece grid we are checking
    int pieceRow = 0; int pieceCol = 0;  
    for (int bitSequence = 0x8000; bit > 0; bit = bit >> 1) {
        // if piece has a square in the 4x4 grid spot we're checking, perform action fn on that square
        if (piece_config & bitSequence) {
            if (!action(piece.x + pieceCol, piece.y + pieceRow, piece.pieceT.color)) return false;
        }
        pieceCol++;
        // move to next row in grid, reset pieceCol to 0
        if (pieceCol == 4) {
            pieceCol = 0; pieceRow++;
        }
    }
    return true;
}

bool checkIfValidMove(int x, int y, color_t color) {
    if (x < 0 || y < 0) return false;
    if (x >= game_config.ncols || y >= game_config.nrows) return false;
    return true;
}

// Draws square (of tetris piece) specified by top left coordinate (x, y) into 
// framebuffer (handled by gl / fb modules)
// Returns true always -- function only called after valid move is verified
bool drawSquare(int x, int y, color_t color) {
    gl_draw_rect(x, y, SQUARE_DIM, SQUARE_DIM, color);
    return true;
}

// Embeds square (of tetris piece) into background tracker
// Returns true always -- function only called after valid move is verified
bool update_background(int x, int y, color_t color) {
    game_config.background_tracker[y][x] = color;
    return true;
}

// Called as prologue to every move/rotate function
// Sets up screen according to background tracker 
void wipe_screen() {
    gl_clear(module.background);
    for (int y = 0; y < module.nrows; y++) {
        for (int x = 0; x < module.ncols; x++) {
            // if colored square in background (from fallen piece), draw
            if (game_config.background_tracker[y][x] != 0) {
                drawSquare(x, y, game_config.background_tracker[y][x]);
            }
        }
    }
}

////////////////  

// return true if piece is in fallen state -- prompt generation of new falling piece in client file
bool move_down(falling_piece_t* piece) {
    wipe_screen();
    piece->y += 1;
    iterateThroughPieceSquares(*piece, drawSquare);
    if (piece->fallen) {
        iterateThroughPieceSquares(*piece, update_background);
        return true;
    }
    return false;
}

void move_left(falling_piece_t* piece) {

}

void move_right(falling_piece_t* piece) {

}

void rotate(falling_piece_t* piece) {

}