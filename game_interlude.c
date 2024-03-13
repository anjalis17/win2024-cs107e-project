// TODO WRITE
// TODO add header comment

// requirements ~
// leaderboard -- select 2-letter name (using tilts??)
// store and show leaderboard
// motion to play game again

#include "console.h"
#include "game_interlude.h"
#include "malloc.h"

static interlude_contents_t contents ;

/*
typedef struct {
    int _nrows;
    int _ncols;
    char* _contents;
} interlude_contents_t;
*/

void game_interlude_init(int nrows, int ncols) {

    // console info
    console_init(nrows, ncols, GL_AMBER, GL_BLACK);
    contents._ncols = ncols ;
    contents._nrows = nrows ;
    contents._leaderboard = malloc(LEADERBOARD_SIZE) ;

    for(int i = 0; i < LEADERBOARD_SIZE; i++) {
        // p6 todo fun francis (FR) gets top score :)
        contents._leaderboard[i]._initials[0] = '*' ; 
        contents._leaderboard[i]._initials[1] = '*' ; 
        contents._leaderboard[i]._initials[2] = '\0' ; 
        contents._leaderboard[i]._score = 0 ;
    }
}


static char* game_interlude_get_user_initials() {

    char ret[3] = "\0\0\0" ;

// ONLY 2 CHARACTERS!! write to ret's 0th and 1st index

// click button to get to character (start at A and go to Z; loop around)
// tilt down to confirm character

    // GET USER INITIALS!!
    // todo ask for input from user -> initials
    // print alphabet and use tilt to hover over the letter we wanna change?
    // return a struct with name and the score??
    // param = score of prev game?


    return NULL ; // TODO FIX!!
}

void update_leaderboard(int score) {
    if (score < contents._leaderboard[LEADERBOARD_SIZE-1]._score) {
        return ; // no need to add the person to leaderboard. not good enough!!
    } else {
        char* initials = game_interlude_get_user_initials() ;
        for(int i = LEADERBOARD_SIZE-1; i >= 0; i--) {
            if (score <= contents._leaderboard[i]._score) {

                // shift the scores down
                for (int j = LEADERBOARD_SIZE-1; j >= i; j--) { 
                    contents._leaderboard[j]._initials[0] = contents._leaderboard[j-1]._initials[0] ;
                    contents._leaderboard[j]._initials[1] = contents._leaderboard[j-1]._initials[1] ;
                    contents._leaderboard[j]._initials[2] = '\0' ; // just bc; why not! :)
                }

                // replace cur score with the person
                contents._leaderboard[i]._initials[0] = initials[0] ;
                contents._leaderboard[i]._initials[1] = initials[1] ;
                contents._leaderboard[i]._initials[2] = '\0' ;
            }
        }
    }
    // todo check if the score is high enough for leaderboard and add if possible
}





int game_interlude_get_rows(void) {
    return contents._nrows ;
}

int game_interlude_get_cols(void) {
    return contents._ncols ;
}

