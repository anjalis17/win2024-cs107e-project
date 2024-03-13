#ifndef _GAME_INTERLUDE_H
#define _GAME_INTERLUDE_H

// TODO WRITE

#define LEADERBOARD_SIZE 5


typedef struct {
    char _initials[3] ; // 2 initials + \0 = 3 characters long
    unsigned int _score;
} leaderboard_character_t;

typedef struct {
    int _nrows;
    int _ncols;
    leaderboard_character_t* _leaderboard; // todo make this leaderboard??
} interlude_contents_t;


void game_interlude_init(int nrows, int ncols) ;

int game_interlude_get_rows(void) ;
int game_interlude_get_cols(void) ;

#endif