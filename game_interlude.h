#ifndef _GAME_INTERLUDE_H
#define _GAME_INTERLUDE_H

#define LEADERBOARD_SIZE 5

typedef struct {
    char _initials[3] ; // 2 initials + \0 = 3 characters long
    unsigned int _score;
} leaderboard_character_t;

typedef struct {
    int _nrows;
    int _ncols;
    leaderboard_character_t* _leaderboard;
} interlude_contents_t;

/* 'game_interlude_init'
 * initializes game screen to 
 *  @param nrows and 
 *  @param ncols wide, 
 * where every row/col size (in pixels) is determined by the character size
 * and the text and background color are determined by 
 *  @param text
 *  @param bg
 */
void game_interlude_init(int nrows, int ncols, color_t text, color_t bg) ;

/* game_interlude_print_leaderboard
 * @param score from most recent game
 * @param num rows cleared from most recent game
 * @functionality updates leaderboard with most recent score (prompts player for initials if necessary) and prints leaderboard to screen
 * @exit tilt remote down
*/
void game_interlude_print_leaderboard(unsigned int score, unsigned int lines_cleared) ;

// getter methods for game_interlude variables
int game_interlude_get_rows(void) ;
int game_interlude_get_cols(void) ;

#endif