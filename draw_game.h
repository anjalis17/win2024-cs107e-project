#include "gl.h"

// Define constant tetris pieces and specify their 4 rotations via a 4x4 grid
// ADD IN DIAGRAM TO SHOW HOW TO COME UP WITH THIS ***
typedef struct _piece {
    char name;
    color_t color;
    int block_rotations[4];
} piece_t;

const piece_t i = {'i', GL_CYAN, {0x0F00, 0x2222, 0x00F0, 0x4444}};
const piece_t j = {'j', GL_BLUE, {0x44C0, 0x8E00, 0x6440, 0x0E20}};
const piece_t l = {'l', GL_ORANGE, {0x4460, 0x0E80, 0xC440, 0x2E00}};
const piece_t o = {'o', GL_YELLOW, {0xCC00, 0xCC00, 0xCC00, 0xCC00}};
const piece_t s = {'s', GL_GREEN, {0x06C0, 0x8C40, 0x6C00, 0x4620}};
const piece_t t = {'t', GL_PURPLE, {0x0E40, 0x4C40, 0x4E00, 0x4640}};
const piece_t z = {'z', GL_RED, {0x0C60, 0x4C80, 0xC600, 0x2640}};

const piece_t pieces[7] = {i, j, l, o, s, t, z};

typedef struct {
    piece_t pieceT;
    char rotation;  // direction of rotation (0 through 3)
    char x;
    char y;
    bool fallen;
} falling_piece_t;
