/*
 * Module to print game interlude (leaderboard and adding people)
 * Author: Aditi Bhaskar (aditijb@stanford.edu)
 * abilities:
 *   store and show leaderboard 
 *   to add yourself to leaderboard, select 2-letter initials (using button presses)
 *   play game again
 */

#include "console.h"
#include "game_interlude.h"
#include "malloc.h"
#include "remote.h"
#include "LSD6DS33.h"
#include "timer.h"
#include "printf.h"

static interlude_contents_t contents ;
#define BLINK_DELAY 100 

/* 'game_interlude_init'
 * initializes game screen to nrows and ncols wide, where every row/col size (in pixels) is determined by the character size
 * takes colors text and bg for the text and background colors respectively
 */
void game_interlude_init(int nrows, int ncols, color_t text, color_t bg) {

    // console info
    console_init(nrows, ncols, text, bg);
    contents._ncols = ncols ;
    contents._nrows = nrows ;
    contents._leaderboard = malloc(LEADERBOARD_SIZE*sizeof(leaderboard_character_t)) ; // 3 chars per each leaderboard thing

    for(int i = 0; i < LEADERBOARD_SIZE; i++) {
        // p6 todo fun francis (FR) gets top score :)
        contents._leaderboard[i]._initials[0] = '*' ; 
        contents._leaderboard[i]._initials[1] = '*' ; 
        contents._leaderboard[i]._initials[2] = '\0' ; 
        contents._leaderboard[i]._score = 0 ;
    }
}

// display instructions
static void game_interlude_operations(void) {
    console_clear() ;
    // o = button -- todo use symbols instead? how do i make instruction page neat?
    // ^ = tilt
    // console_printf("LEADERBOARD\n\n Tilt Down:\n   select\n   next\n button:\n   iterate\n\n\n") ; 
    console_printf("\nLEADERBOARD!\n\n Down:\n  Set / Next\n  \n Button:\n  Change\n\n\n") ; 
    int pitch = 0; int roll = 0 ;
    remote_get_x_y_status(&pitch, &roll) ;
    timer_delay(2) ;
    while (pitch != X_FAST) { remote_get_x_y_status(&pitch, &roll) ; } // wait for tilt
}

/* game_interlude_print_leaderboard
 * @param score from most recent game
 * @param num rows cleared from most recent game
 * @functionality prints useful info about the most recent game to screen
 * @exit tilt remote down
*/
static void game_interlude_display_game_stats(unsigned int score, unsigned int lines_cleared) {
    console_clear() ;
    console_printf("score:\n %08d\nlines cleared:\n %03d", score, lines_cleared) ; 
    int pitch = 0; int roll = 0 ;
    remote_get_x_y_status(&pitch, &roll) ;
    timer_delay(2) ;
    while (pitch != X_FAST) { remote_get_x_y_status(&pitch, &roll) ; } // wait for tilt
}

/* game_interlude_get_user_initials
 * @functionality uses a simple state machine to prompt and read 2 letters in a user's initials
 * @returns a char* of the user's initials
 * how to use: Click button to iterate thru characters. Tilt remote down to confirm character. Characters loop from A-Z
 */
static char* game_interlude_get_user_initials(void) {

    char *initials = malloc(3) ;
    initials[0] = '\0' ;
    initials[1] = '\0' ;
    initials[2] = '\0' ;

    // ask for input (user initials) from user

    // (flickering effect)
    for (int i = 0; i < 5; i++) {
        console_clear() ; 
        console_printf("Your Initials:\n **\n\n(Click Button)") ;
        timer_delay_ms(BLINK_DELAY) ;
        console_clear() ; 
        console_printf("Your Initials:\n  *\n\n(Click Button)") ;
        timer_delay_ms(BLINK_DELAY) ;
    }
    console_clear() ; 
    console_printf("Your Initials:\n **\n\n(Click Button)") ;

    // gather 1st initial
    int first_letter = 25 ; // Z
    int pitch = 0; int roll = 0 ;
    remote_get_x_y_status(&pitch, &roll) ;
    while (pitch != X_FAST) {
        if(remote_is_button_press()) {
            first_letter ++ ;
            console_clear() ;
            console_printf("Your Initials:\n %c*", ('A'+first_letter%26)) ;
        }
        remote_get_x_y_status(&pitch, &roll) ;
    }

    // (flickering effect)
    for (int i = 0; i < 5; i++) {
        console_clear() ; 
        console_printf("Your Initials:\n %c*", ('A'+first_letter%26)) ;
        timer_delay_ms(BLINK_DELAY) ;
        console_clear() ; 
        console_printf("Your Initials:\n %c ", ('A'+first_letter%26)) ;
        timer_delay_ms(BLINK_DELAY) ;
    }
    console_clear() ; 
    console_printf("Your Initials:\n %c*", ('A'+first_letter%26)) ;

    // gather 2nd initial
    int second_letter = 25 ; // Z
    remote_get_x_y_status(&pitch, &roll) ;
    while (pitch != X_FAST) {
        if(remote_is_button_press()) {
            second_letter ++ ;
            console_clear() ;
            console_printf("Your Initials:\n %c%c", ('A'+first_letter%26), ('A'+second_letter%26)) ;
        }
        remote_get_x_y_status(&pitch, &roll) ;
    }

    timer_delay(1) ; // arbitrary; todo train for user

    // set it into user's initials to return
    initials[0] = ('A'+first_letter%26) ;
    initials[1] = ('A'+second_letter%26) ;

    return initials ;
}

/* game_interlude_update_leaderboard
 * @param takes a score and adds it to the leaderboard if the score is high enough
 */
static void game_interlude_update_leaderboard(unsigned int score) {

    if (score >= contents._leaderboard[LEADERBOARD_SIZE-1]._score) {  // if the score is high enough for leaderboard

        char* initials = game_interlude_get_user_initials() ; 

        for(int i = LEADERBOARD_SIZE-1; i >= 0; i--) {
            if (score < contents._leaderboard[i]._score || i == 0) { // most recent tie will be at the top
                if(i==0 && (score >= contents._leaderboard[0]._score)) {i=-1;}  // in case we have a new high-score

                // shift the scores down
                for (int j = LEADERBOARD_SIZE-1; j > i+1; j--) { 
                    contents._leaderboard[j]._initials[0] = contents._leaderboard[j-1]._initials[0] ;
                    contents._leaderboard[j]._initials[1] = contents._leaderboard[j-1]._initials[1] ;
                    contents._leaderboard[j]._initials[2] = '\0' ; // just bc; why not! :)
                    contents._leaderboard[j]._score = contents._leaderboard[j-1]._score;
                }

                // replace cur score with the person
                contents._leaderboard[i+1]._initials[0] = initials[0] ;
                contents._leaderboard[i+1]._initials[1] = initials[1] ;
                contents._leaderboard[i+1]._initials[2] = '\0' ; // just bc; why not! :)
                contents._leaderboard[i+1]._score = score ;

                free(initials) ; 
                break ;
            }
        }
    }
}

/* game_interlude_print_leaderboard
 * @param score from most recent game
 * @param num rows cleared from most recent game
 * @functionality updates leaderboard with most recent score (prompts player for initials if necessary) and prints leaderboard to screen
 * @exit tilt remote down
*/
void game_interlude_print_leaderboard(unsigned int score, unsigned int lines_cleared) {

    // pre-leaderboard stuff
    game_interlude_operations() ;
    game_interlude_display_game_stats(score, lines_cleared) ; // tell the player how they did!

    // now, we get to the leaderboard
    game_interlude_update_leaderboard(score) ; // need to update leaderboard first! (if worthy player)

    console_clear() ;
    console_printf("*LEADERBOARD*\n") ;
    console_printf("<#>\t <n>\tSCORE\n") ;

    for (int i = 0; i < LEADERBOARD_SIZE; i++) {
        console_printf("%d: \t\t%s \t%d\n", i, contents._leaderboard[i]._initials, contents._leaderboard[i]._score) ;
    }

    timer_delay(1) ; //arbitrary, todo update to ux
    console_printf("\nTilt to Play") ; // todo come up with better message...
    int pitch = 0; int roll = 0 ;
    remote_get_x_y_status(&pitch, &roll) ;
    while (pitch != X_FAST) {remote_get_x_y_status(&pitch, &roll) ;}
}

// returns num rows in console
int game_interlude_get_rows(void) {
    return contents._nrows ;
}

// returns num cols in console
int game_interlude_get_cols(void) {
    return contents._ncols ;
}

