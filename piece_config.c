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

    int nbytes = game_config.width * game_config.height;
    game_config.background_tracker = malloc(nbytes);
}

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

