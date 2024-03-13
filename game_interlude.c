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

void game_interlude_init(int nrows, int ncols) {

    // console info
    console_init(nrows, ncols, GL_AMBER, GL_BLACK);
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


// GET USER INITIALS!! if they are worthy of the leaderboard :)
    // how to use:
    // click button to get to character (start at A and go to Z; loop around)
    // tilt down to confirm character
static char* game_interlude_get_user_initials() {

    char *initials = malloc(3) ; // todo take care of freeing this!!
    initials[0] = '\0' ;
    initials[1] = '\0' ;
    initials[2] = '\0' ;

    // display instructions
    // todo crop text length if necessary
    console_clear() ;
    console_printf("Add yourself to the leaderboard!\n  button: next letter\n  tilt down: select\n\ntilt down to continue.") ; 
    int pitch = 0; int roll = 0 ;
    remote_get_x_y_status(&pitch, &roll) ;
    timer_delay(5) ;
    while (pitch != X_FAST) { remote_get_x_y_status(&pitch, &roll) ; } // wait for tilt

    // ask for input (user initials) from user
    int blink_delay = 100 ;

    // (flickering effect)
    for (int i = 0; i < 5; i++) {
        console_clear() ; 
        console_printf("your initials:\n **\n\n(click button)") ;
        timer_delay_ms(blink_delay) ;
        console_clear() ; 
        console_printf("your initials:\n  *\n\n(click button)") ;
        timer_delay_ms(blink_delay) ;
    }
    console_clear() ; 
    console_printf("your initials:\n **\n\n(click button)") ;

    // gather 1st initial
    int first_letter = 25 ; // Z
    remote_get_x_y_status(&pitch, &roll) ;
    while (pitch != X_FAST) {
        if(remote_is_button_press()) {
            first_letter ++ ;
            console_clear() ;
            console_printf("your initials:\n %c*", ('A'+first_letter%26)) ;
        }
        remote_get_x_y_status(&pitch, &roll) ;
    }

    // (flickering effect)
    for (int i = 0; i < 5; i++) {
        console_clear() ; 
        console_printf("your initials:\n %c*", ('A'+first_letter%26)) ;
        timer_delay_ms(blink_delay) ;
        console_clear() ; 
        console_printf("your initials:\n %c ", ('A'+first_letter%26)) ;
        timer_delay_ms(blink_delay) ;
    }
    console_clear() ; 
    console_printf("your initials:\n %c*", ('A'+first_letter%26)) ;

    // gather 2nd initial
    int second_letter = 25 ; // Z
    remote_get_x_y_status(&pitch, &roll) ;
    while (pitch != X_FAST) {
        if(remote_is_button_press()) {
            second_letter ++ ;
            console_clear() ;
            console_printf("your initials:\n %c%c", ('A'+first_letter%26), ('A'+second_letter%26)) ;
        }
        remote_get_x_y_status(&pitch, &roll) ;
    }

    timer_delay(1) ; // arbitrary; todo train for user

    // set it into user's initials to return
    initials[0] = ('A'+first_letter%26) ;
    initials[1] = ('A'+second_letter%26) ;

    return initials ;
}

static void game_interlude_update_leaderboard(unsigned int score) {
    if (score >= contents._leaderboard[LEADERBOARD_SIZE-1]._score) {  // check if the score is high enough for leaderboard and add if possible
        char* initials = game_interlude_get_user_initials() ;

        for(int i = LEADERBOARD_SIZE-1; i >= 0; i--) {
            if (score <= contents._leaderboard[i]._score || i == 0) {

                if(i==0 && (score > contents._leaderboard[0]._score)) {i=-1;}  // in case we have a new high-score

                // shift the scores down
                for (int j = LEADERBOARD_SIZE-1; j > i+1; j--) {  // todo package into a function
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

// param: score from most recent game
void game_interlude_print_leaderboard(unsigned int score) {
    game_interlude_update_leaderboard(score) ; // need to update leaderboard first! (if worthy player)

    console_clear() ;
    console_printf("**LEADERBOARD**\n") ;
    console_printf("rank:\tname\tscore\n") ;

    for (int i = 0; i < LEADERBOARD_SIZE; i++) {
        console_printf("%d: \t\t%s \t%d\n", i, contents._leaderboard[i]._initials, contents._leaderboard[i]._score) ;
    }

    console_printf("\ntilt down to play again") ; // todo come up with better message...
    int pitch = 0; int roll = 0 ;
    remote_get_x_y_status(&pitch, &roll) ;
    while (pitch != X_FAST) {}
}

int game_interlude_get_rows(void) {
    return contents._nrows ;
}

int game_interlude_get_cols(void) {
    return contents._ncols ;
}

