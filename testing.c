#include "testing.h"

#include "game_update.h"
#include "uart.h"
#include "assert.h"
#include "timer.h"
#include "printf.h"

#include "gpio.h"
#include "gpio_extra.h"
#include "servo.h"
#include "remote.h"
#include "LSD6DS33.h"
#include "i2c.h"
#include "passive_buzz_intr.h"
#include "passive_buzz.h"
#include "game_interlude.h"
#include "console.h"
#include "music.h"

// void pause(const char *message) {
//     if (message) printf("\n%s\n", message);
//     printf("[PAUSED] type any key in minicom/terminal to continue: ");
//     int ch = uart_getchar();
//     uart_putchar(ch);
//     uart_putchar('\n');
// }

int get_keystroke(const char *message) {
    if (message) printf("\n%s\n", message);
    // printf("[PAUSED] type any key in minicom/terminal to continue: ");
    int ch = uart_getchar();
    uart_putchar(ch);
    uart_putchar('\n');
    return ch;
}

void test_random_init(void) {
    for (int i = 0; i < 10; i++) {
        timer_init();
        game_update_init(20, 10);
        falling_piece_t piece = init_falling_piece();
        pause("start");
    }
}

void test_basic_block_motion(void) {
    timer_init();
    game_update_init(20, 10);
    falling_piece_t piece = init_falling_piece();
    while (1) {
        pause("key press to move down");
        move_down(&piece);
        if (piece.fallen) {
            printf("fallen; new piece spawning");
            piece = init_falling_piece();
        }
    }
}

void test_motions(void) {
    timer_init();
    game_update_init(20, 10);
    falling_piece_t piece = init_falling_piece();
    while (1) {
        int ch = get_keystroke("key press to move down (S) / left (A) / right (D) or rotate (R)");
        if (ch == 's') move_down(&piece);
        else if (ch == 'a') move_left(&piece);
        else if (ch == 'd') move_right(&piece);
        else if (ch == 'r') rotate(&piece);
        if (piece.fallen) {
            printf("fallen; new piece spawning");
            piece = init_falling_piece();
        }
    }
}

void test_remote(void) {
    gpio_init() ;
    timer_init() ;
    uart_init() ;
    interrupts_init() ;
    remote_init(GPIO_PB1, GPIO_PB0, GPIO_PB6, TEMPO_DEFAULT) ; 
    interrupts_global_enable() ; 
    printf("\nremote tests enabled\n") ;
    while(1) {
        // obsolete function calls :( Mar 11 2024

        // // tilt blocks
        // int tilt_status = get_tilt();
        // printf("\n device.state %s\n", tilt_status==HOME?"home":(tilt_status==LEFT?"left":"right")) ;
        
        // // drop blocks
        // if (is_drop()) {printf("\n*** x dropped ***\n");}
    }
}

void test_motions_integrated(void) { // OBSOLETE TEST FUNCTION!!
    gpio_init() ;
    timer_init() ;
    uart_init() ;
    interrupts_init() ;
    remote_init(GPIO_PB1, GPIO_PB0, GPIO_PB6,TEMPO_DEFAULT) ; 
    interrupts_global_enable() ;
    
    game_update_init(20, 10);

    falling_piece_t piece = init_falling_piece();

    printf("\nin test_motions_integrated; initialized\n") ;

    // we write accelerometer x/y position meanings to these vars
    int pitch = 0; int roll = 0;

    int n = 250 ; // total ms wait for each loop
    n = (n*1000*TICKS_PER_USEC);

    while(1) {
        // tilt blocks
        remote_get_x_y_status(&pitch, &roll); // the x and y tilt statuses
        
        // horizontal movement
        if (roll == LEFT) move_left(&piece);
        else if (roll == RIGHT) move_right(&piece);
        
        // drop a block faster
        if (pitch == X_FAST) { 
            // printf("\n*** x dropped ***\n"); // for debugging
            if(!piece.fallen){
                move_down(&piece);
            }
            if(!piece.fallen){
                move_down(&piece);
            }
        }

        while (remote_is_button_press()) {
            rotate(&piece);
        }
        
        move_down(&piece);

        if (piece.fallen) {
            // printf("fallen; new piece spawning"); // for debugging
            piece = init_falling_piece();
        }

        while (timer_get_ticks() % n != 0) {
        }  

    }
}

void integration_test_v2(void) {
    gpio_init() ;
    timer_init() ;
    uart_init() ;
    interrupts_init() ;
    remote_init(GPIO_PB1, GPIO_PB0, GPIO_PB6,TEMPO_DEFAULT) ; 
    interrupts_global_enable() ;
    
    game_update_init(20, 10);
    falling_piece_t piece = init_falling_piece();

    printf("\nin test_motions_integrated; initialized\n") ;

    // we write accelerometer x/y position meanings to these vars
    int pitch = 0; int roll = 0;
    long n = 500 ; // total ms wait for each loop
    n = (n * 1000 * TICKS_PER_USEC);

    int toggle_turns = 0 ;

    while(1) {
        printf ("timer get ticks START(): %ld\n", timer_get_ticks() % n) ;
        while (timer_get_ticks() % n <= (0.8 * n)) {
            toggle_turns += 1 ;
            // tilt blocks
            remote_get_x_y_status(&pitch, &roll); // the x and y tilt statuses
    
            // horizontal movement
            if (toggle_turns % 3 == 0) {
                if (roll == LEFT) move_left(&piece);
                else if (roll == RIGHT) move_right(&piece);                
            }
        
            // drop a block faster
            if (pitch == X_FAST) { 
                printf("\n*** x dropped fast ***\n"); // for debugging
                if (!piece.fallen){
                    move_down(&piece);
                }
                if (!piece.fallen){
                    move_down(&piece);
                }
            }

            while (remote_is_button_press()) {
                rotate(&piece);
            }  
            if (piece.fallen) {
                piece = init_falling_piece();
            }
            // timer_delay_ms(20);
        } 
        printf("timer get ticks END(): %ld\n", timer_get_ticks() % n) ;     
        
        printf("\n*** x dropped normal ***\n"); // for debugging
        move_down(&piece);

        while (timer_get_ticks() % n > (0.8 * n)) {
            printf("waiting...");
        };
    }
}

// void tetris_theme_song(void) {
//     gpio_init() ;
//     timer_init() ;
//     uart_init() ;
//     buzzer_init(GPIO_PB6) ; // is also pwm 1
//     buzzer_set_tempo(TEMPO_ANDANTE) ; // RESOLVED IN FUTURE! FIX allegro+ tempos not working :(
//     // each block of code is a measure
//     // music: https://musescore.com/neoguizmo/scores/2601951; shifted down 1 octave
//     while(1) {
//         printf("in buzzer tetris theme");

//         // fast part
//         for (int i = 0; i < 2; i++) {
//             buzzer_play_note(NOTE_FREQ_E, NOTE_QUARTER) ;
//             buzzer_play_note(NOTE_FREQ_B_3, NOTE_EIGHTH) ;
//             buzzer_play_note(NOTE_FREQ_C, NOTE_EIGHTH) ;
//             buzzer_play_note(NOTE_FREQ_D, NOTE_QUARTER) ;
//             buzzer_play_note(NOTE_FREQ_C, NOTE_EIGHTH) ;
//             buzzer_play_note(NOTE_FREQ_B_3, NOTE_EIGHTH) ;

//             buzzer_play_note(NOTE_FREQ_A_3, NOTE_QUARTER) ;
//             buzzer_play_note(NOTE_FREQ_A_3, NOTE_EIGHTH) ;
//             buzzer_play_note(NOTE_FREQ_C, NOTE_EIGHTH) ;
//             buzzer_play_note(NOTE_FREQ_E, NOTE_QUARTER) ;
//             buzzer_play_note(NOTE_FREQ_D, NOTE_EIGHTH) ;
//             buzzer_play_note(NOTE_FREQ_C, NOTE_EIGHTH) ;

//             buzzer_play_note(NOTE_FREQ_B_3, NOTE_QUARTER) ;
//             buzzer_play_note(NOTE_FREQ_B_3, NOTE_EIGHTH) ;
//             buzzer_play_note(NOTE_FREQ_C, NOTE_EIGHTH) ;
//             buzzer_play_note(NOTE_FREQ_D, NOTE_QUARTER) ;
//             buzzer_play_note(NOTE_FREQ_E, NOTE_QUARTER) ;

//             buzzer_play_note(NOTE_FREQ_C, NOTE_QUARTER) ;
//             buzzer_play_note(NOTE_FREQ_A_3, NOTE_QUARTER) ;
//             buzzer_play_note(NOTE_FREQ_A_3, NOTE_HALF) ;

//             buzzer_play_note(NOTE_FREQ_D, NOTE_QUARTER) ;
//             buzzer_play_note(NOTE_FREQ_D, NOTE_EIGHTH) ;
//             buzzer_play_note(NOTE_FREQ_F, NOTE_EIGHTH) ;
//             buzzer_play_note(NOTE_FREQ_A, NOTE_QUARTER) ;
//             buzzer_play_note(NOTE_FREQ_G, NOTE_EIGHTH) ;
//             buzzer_play_note(NOTE_FREQ_F, NOTE_EIGHTH) ;

//             buzzer_play_note(NOTE_FREQ_E, NOTE_QUARTER) ;
//             buzzer_play_note(NOTE_FREQ_E, NOTE_EIGHTH) ;
//             buzzer_play_note(NOTE_FREQ_C, NOTE_EIGHTH) ;
//             buzzer_play_note(NOTE_FREQ_E, NOTE_QUARTER) ;
//             buzzer_play_note(NOTE_FREQ_D, NOTE_EIGHTH) ;
//             buzzer_play_note(NOTE_FREQ_C, NOTE_EIGHTH) ;

//             buzzer_play_note(NOTE_FREQ_B_3, NOTE_QUARTER) ;
//             buzzer_play_note(NOTE_FREQ_B_3, NOTE_EIGHTH) ;
//             buzzer_play_note(NOTE_FREQ_C, NOTE_EIGHTH) ;
//             buzzer_play_note(NOTE_FREQ_D, NOTE_QUARTER) ;
//             buzzer_play_note(NOTE_FREQ_E, NOTE_QUARTER) ;

//             buzzer_play_note(NOTE_FREQ_C, NOTE_QUARTER) ;
//             buzzer_play_note(NOTE_FREQ_A_3, NOTE_QUARTER) ;
//             buzzer_play_note(NOTE_FREQ_A_3, NOTE_HALF) ;
//         }

//         // slow falling part
//         buzzer_play_note(NOTE_FREQ_E, NOTE_HALF) ;
//         buzzer_play_note(NOTE_FREQ_C, NOTE_HALF) ;

//         buzzer_play_note(NOTE_FREQ_D, NOTE_HALF) ;
//         buzzer_play_note(NOTE_FREQ_B_3, NOTE_HALF) ;

//         buzzer_play_note(NOTE_FREQ_C, NOTE_HALF) ;
//         buzzer_play_note(NOTE_FREQ_A_3, NOTE_HALF) ;

//         buzzer_play_note(NOTE_FREQ_G_SHARP_3, NOTE_WHOLE) ;

//         buzzer_play_note(NOTE_FREQ_E, NOTE_HALF) ;
//         buzzer_play_note(NOTE_FREQ_C, NOTE_HALF) ;

//         buzzer_play_note(NOTE_FREQ_D, NOTE_HALF) ;
//         buzzer_play_note(NOTE_FREQ_B_3, NOTE_HALF) ;

//         buzzer_play_note(NOTE_FREQ_C, NOTE_QUARTER) ;
//         buzzer_play_note(NOTE_FREQ_E, NOTE_QUARTER) ;
//         buzzer_play_note(NOTE_FREQ_A, NOTE_HALF) ; 

//         buzzer_play_note(NOTE_FREQ_G_SHARP, NOTE_WHOLE) ;
//     }
// }

void test_leaderboard(void) {

    gpio_init() ;
    timer_init() ;
    uart_init() ;
    interrupts_init() ;
    remote_init(GPIO_PB1, GPIO_PB0, GPIO_PB6,TEMPO_DEFAULT) ; 
    interrupts_global_enable() ;
    timer_delay(2) ;

    remote_is_button_press() ; // get rid of the extra button press...

    game_interlude_init(30, 50, GL_AMBER, GL_BLACK) ;
    game_interlude_print_leaderboard(400, 2) ;
    game_interlude_print_leaderboard(500, 3) ;
    game_interlude_print_leaderboard(100, 4) ;
    game_interlude_print_leaderboard(400, 5) ;
    game_interlude_print_leaderboard(300, 6) ;
    game_interlude_print_leaderboard(40, 1) ;
    game_interlude_print_leaderboard(200, 5) ;

}

// includes the leaderboard loop and constant games!
void integration_test_v3(void) {
    gpio_init() ;
    timer_init() ;
    uart_init() ;
    interrupts_init() ;
    remote_init(GPIO_PB1, GPIO_PB0, GPIO_PB6,TEMPO_DEFAULT) ; 
    interrupts_global_enable() ;
    timer_delay(2) ;

    remote_is_button_press() ; // get rid of the extra button press...

    game_interlude_init(30, 50, GL_AMBER, GL_BLACK) ; // can do this outside

    while(1) {
        game_update_init(20, 10);
        falling_piece_t piece = init_falling_piece();

        // write accelerometer x/y position to pitch(x) and roll(y)
        int pitch = 0; int roll = 0;
        long n = 500 ; // total ms wait for each loop
        n = (n * 1000 * TICKS_PER_USEC);

        int toggle_turns = 0 ;

        while(1) {
            while (timer_get_ticks() % n <= (0.8 * n)) {
                toggle_turns += 1 ; toggle_turns %= 3 ; // so we don't overflow
                // tilt blocks
                remote_get_x_y_status(&pitch, &roll); // the x and y tilt statuses
        
                // horizontal movement
                if (toggle_turns % 3 == 0) {
                    if (roll == LEFT) move_left(&piece);
                    else if (roll == RIGHT) move_right(&piece);                
                }
            
                // drop a block faster
                if (pitch == X_FAST) { 
                    if (!piece.fallen) move_down(&piece);
                    if (!piece.fallen) move_down(&piece);
                }

                while (remote_is_button_press()) rotate(&piece);
                if (piece.fallen) piece = init_falling_piece();
            } 

            move_down(&piece);
            if (game_update_is_game_over()) {timer_delay(2) ; break ;} // exits game-playing mode if game is over

            while (timer_get_ticks() % n > (0.8 * n)) {
                // RESOLVED WITH passive_buzz_intr.c aditi play music notes in here???
            };
        } 

        game_interlude_print_leaderboard(game_update_get_score(), game_update_get_rows_cleared()) ; 
    }
}



#define uSEC_IN_SEC 1000000

// DANGER ZONE
// includes the leaderboard loop and constant games!  AND MUSIC?? chromatic scale...
void integration_test_v4(void) {
    gpio_init() ;
    timer_init() ;
    uart_init() ;
    interrupts_init() ;
    remote_init(GPIO_PB1, GPIO_PB0, GPIO_PB6,TEMPO_DEFAULT) ; 
    buzzer_init(GPIO_PB6) ;
    interrupts_global_enable() ;
    timer_delay(2) ;

    remote_is_button_press() ; // get rid of the extra button press...

    game_interlude_init(30, 50, GL_AMBER, GL_BLACK) ; // can do this outside

    while(1) {
        game_update_init(20, 10);
        falling_piece_t piece = init_falling_piece();

        // write accelerometer x/y position to pitch(x) and roll(y)
        int pitch = 0; int roll = 0;
        long n = 500 ; // total ms wait for each loop
        n = (n * 1000 * TICKS_PER_USEC);

        int toggle_turns = 0 ;

        const int num_notes = 10 ;
        int music_notes[10] = {NOTE_FREQ_C, NOTE_FREQ_D, NOTE_FREQ_E, NOTE_FREQ_F, NOTE_FREQ_F_SHARP, NOTE_FREQ_G, NOTE_FREQ_G_SHARP, NOTE_FREQ_A, NOTE_FREQ_A_SHARP, NOTE_FREQ_B} ;
        int music_index = 0 ;

        while(1) {
            while (timer_get_ticks() % n <= (0.8 * n)) {
                toggle_turns += 1 ; toggle_turns %= 3 ; // so we don't overflow
                // tilt blocks
                remote_get_x_y_status(&pitch, &roll); // the x and y tilt statuses
        
                // horizontal movement
                if (toggle_turns % 3 == 0) {
                    if (roll == LEFT) move_left(&piece);
                    else if (roll == RIGHT) move_right(&piece);                
                }
            
                // drop a block faster
                if (pitch == X_FAST) { 
                    if (!piece.fallen) move_down(&piece);
                    if (!piece.fallen) move_down(&piece);
                }

                while (remote_is_button_press()) rotate(&piece);
                if (piece.fallen) piece = init_falling_piece();
            } 

            move_down(&piece);
            if (game_update_is_game_over()) {timer_delay(2) ; break ;} // exits game-playing mode if game is over

            music_index += 1; music_index %= num_notes; 
            int note_period = (uSEC_IN_SEC / music_notes[music_index]) ; // keep it approximate

            while (timer_get_ticks() % n > (0.8 * n)) {
                // play music notes in here???
                gpio_write(GPIO_PB6, 1);
                timer_delay_us(note_period/2);
                gpio_write(GPIO_PB6, 0);
                timer_delay_us(note_period/2);
            };
        } 

        game_interlude_print_leaderboard(game_update_get_score(), game_update_get_rows_cleared()) ; 
    }
}


// includes the leaderboard loop and constant games!  AND MUSIC!! actual tetris theme
void integration_test_v5(void) {
    // gpio_init() ;
    // timer_init() ;
    // uart_init() ;
    // interrupts_init() ;
    // remote_init(GPIO_PB1, GPIO_PB0, GPIO_PB6,TEMPO_DEFAULT) ; 
    // buzzer_init(GPIO_PB6) ; // RESOLVED in TEST V6 add to the remote module
    // interrupts_global_enable() ;
    // timer_delay(2) ;

    // remote_is_button_press() ; // get rid of the extra button press... 

    // game_interlude_init(30, 50, GL_AMBER, GL_BLACK) ; // can do this outside

    // while(1) {
    //     game_update_init(20, 10);
    //     falling_piece_t piece = init_falling_piece();

    //     // write accelerometer x/y position to pitch(x) and roll(y)
    //     int pitch = 0; int roll = 0;
    //     long n = 500 ; // total ms wait for each loop
    //     n = (n * 1000 * TICKS_PER_USEC);

    //     int toggle_turns = 0 ;

    //     const int num_notes = 8*4*6 ; 
    //     int music_notes[8*4 * 6] = // all notes are eigth notes. each line is a measure. each 4 lines is a grouped musical phrase
    //                             {
    //                                 // theme 
    //                                 NOTE_FREQ_E, NOTE_FREQ_E, NOTE_FREQ_B_3, NOTE_FREQ_C, NOTE_FREQ_D, NOTE_FREQ_D, NOTE_FREQ_C, NOTE_FREQ_B_3, 
    //                                 NOTE_FREQ_A_3, NOTE_FREQ_A_3, NOTE_FREQ_A_3, NOTE_FREQ_C, NOTE_FREQ_E, NOTE_FREQ_E, NOTE_FREQ_D, NOTE_FREQ_C,
    //                                 NOTE_FREQ_B_3, NOTE_FREQ_B_3, NOTE_FREQ_B_3, NOTE_FREQ_C, NOTE_FREQ_D, NOTE_FREQ_D, NOTE_FREQ_E, NOTE_FREQ_E,
    //                                 NOTE_FREQ_C, NOTE_FREQ_C, NOTE_FREQ_A_3, NOTE_FREQ_A_3, NOTE_FREQ_A_3, NOTE_FREQ_A_3, NOTE_FREQ_A_3, NOTE_FREQ_A_3,
                                    
    //                                 NOTE_FREQ_D, NOTE_FREQ_D, NOTE_FREQ_D, NOTE_FREQ_F, NOTE_FREQ_A, NOTE_FREQ_A, NOTE_FREQ_G, NOTE_FREQ_F, 
    //                                 NOTE_FREQ_E, NOTE_FREQ_E, NOTE_FREQ_E, NOTE_FREQ_C, NOTE_FREQ_E, NOTE_FREQ_E, NOTE_FREQ_D, NOTE_FREQ_C,
    //                                 NOTE_FREQ_B_3, NOTE_FREQ_B_3, NOTE_FREQ_B_3, NOTE_FREQ_C, NOTE_FREQ_D, NOTE_FREQ_D, NOTE_FREQ_E, NOTE_FREQ_E,
    //                                 NOTE_FREQ_C, NOTE_FREQ_C, NOTE_FREQ_A_3, NOTE_FREQ_A_3, NOTE_FREQ_A_3, NOTE_FREQ_A_3, NOTE_FREQ_A_3, NOTE_FREQ_A_3,
                                    
    //                                 // theme again
    //                                 NOTE_FREQ_E, NOTE_FREQ_E, NOTE_FREQ_B_3, NOTE_FREQ_C, NOTE_FREQ_D, NOTE_FREQ_D, NOTE_FREQ_C, NOTE_FREQ_B_3, 
    //                                 NOTE_FREQ_A_3, NOTE_FREQ_A_3, NOTE_FREQ_A_3, NOTE_FREQ_C, NOTE_FREQ_E, NOTE_FREQ_E, NOTE_FREQ_D, NOTE_FREQ_C,
    //                                 NOTE_FREQ_B_3, NOTE_FREQ_B_3, NOTE_FREQ_B_3, NOTE_FREQ_C, NOTE_FREQ_D, NOTE_FREQ_D, NOTE_FREQ_E, NOTE_FREQ_E,
    //                                 NOTE_FREQ_C, NOTE_FREQ_C, NOTE_FREQ_A_3, NOTE_FREQ_A_3, NOTE_FREQ_A_3, NOTE_FREQ_A_3, NOTE_FREQ_A_3, NOTE_FREQ_A_3,
                                    
    //                                 NOTE_FREQ_D, NOTE_FREQ_D, NOTE_FREQ_D, NOTE_FREQ_F, NOTE_FREQ_A, NOTE_FREQ_A, NOTE_FREQ_G, NOTE_FREQ_F, 
    //                                 NOTE_FREQ_E, NOTE_FREQ_E, NOTE_FREQ_E, NOTE_FREQ_C, NOTE_FREQ_E, NOTE_FREQ_E, NOTE_FREQ_D, NOTE_FREQ_C,
    //                                 NOTE_FREQ_B_3, NOTE_FREQ_B_3, NOTE_FREQ_B_3, NOTE_FREQ_C, NOTE_FREQ_D, NOTE_FREQ_D, NOTE_FREQ_E, NOTE_FREQ_E,
    //                                 NOTE_FREQ_C, NOTE_FREQ_C, NOTE_FREQ_A_3, NOTE_FREQ_A_3, NOTE_FREQ_A_3, NOTE_FREQ_A_3, NOTE_FREQ_A_3, NOTE_FREQ_A_3,

    //                                 // slow falling part
    //                                 NOTE_FREQ_E, NOTE_FREQ_E, NOTE_FREQ_E, NOTE_FREQ_E, NOTE_FREQ_C, NOTE_FREQ_C, NOTE_FREQ_C, NOTE_FREQ_C, 
    //                                 NOTE_FREQ_D, NOTE_FREQ_D, NOTE_FREQ_D, NOTE_FREQ_D, NOTE_FREQ_B_3, NOTE_FREQ_B_3, NOTE_FREQ_B_3, NOTE_FREQ_B_3,
    //                                 NOTE_FREQ_C, NOTE_FREQ_C, NOTE_FREQ_C, NOTE_FREQ_C, NOTE_FREQ_A_3, NOTE_FREQ_A_3, NOTE_FREQ_A_3, NOTE_FREQ_A_3,
    //                                 NOTE_FREQ_G_SHARP_3, NOTE_FREQ_G_SHARP_3, NOTE_FREQ_G_SHARP_3, NOTE_FREQ_G_SHARP_3, NOTE_FREQ_G_SHARP_3, NOTE_FREQ_G_SHARP_3, NOTE_FREQ_G_SHARP_3, NOTE_FREQ_G_SHARP_3,
                                    
    //                                 NOTE_FREQ_E, NOTE_FREQ_E, NOTE_FREQ_E, NOTE_FREQ_E, NOTE_FREQ_C, NOTE_FREQ_C, NOTE_FREQ_C, NOTE_FREQ_C, 
    //                                 NOTE_FREQ_D, NOTE_FREQ_D, NOTE_FREQ_D, NOTE_FREQ_D, NOTE_FREQ_B_3, NOTE_FREQ_B_3, NOTE_FREQ_B_3, NOTE_FREQ_B_3,
    //                                 NOTE_FREQ_C, NOTE_FREQ_C, NOTE_FREQ_E, NOTE_FREQ_E, NOTE_FREQ_A, NOTE_FREQ_A, NOTE_FREQ_A, NOTE_FREQ_A,
    //                                 NOTE_FREQ_G_SHARP_3, NOTE_FREQ_G_SHARP_3, NOTE_FREQ_G_SHARP_3, NOTE_FREQ_G_SHARP_3, NOTE_FREQ_G_SHARP_3, NOTE_FREQ_G_SHARP_3, NOTE_FREQ_G_SHARP_3, NOTE_FREQ_G_SHARP_3
    //                             } ;

    //     int music_index = 0 ;

    //     while(1) {
    //         while (timer_get_ticks() % n <= (0.8 * n)) {
    //             toggle_turns += 1 ; toggle_turns %= 3 ; // so we don't overflow
    //             // tilt blocks
    //             remote_get_x_y_status(&pitch, &roll); // the x and y tilt statuses
        
    //             // horizontal movement
    //             if (toggle_turns % 3 == 0) {
    //                 if (roll == LEFT) move_left(&piece);
    //                 else if (roll == RIGHT) move_right(&piece);                
    //             }
            
    //             // drop a block faster
    //             if (pitch == X_FAST) { 
    //                 if (!piece.fallen) move_down(&piece);
    //                 if (!piece.fallen) move_down(&piece);
    //             }

    //             while (remote_is_button_press()) rotate(&piece);
    //             if (piece.fallen) piece = init_falling_piece();
    //         } 

    //         move_down(&piece);
    //         if (game_update_is_game_over()) {timer_delay(2) ; break ;} // exits game-playing mode if game is over

    //         music_index += 1; music_index %= num_notes; 
    //         int note_period = (uSEC_IN_SEC / music_notes[music_index]) ; // keep it approximate

    //         while (timer_get_ticks() % n > (0.8 * n)) {};
    //     } 

    //     game_interlude_print_leaderboard(game_update_get_score(), game_update_get_rows_cleared()) ; 
    // }
}

// includes the leaderboard loop and constant games! AND MUSIC!! actual tetris theme
void integration_test_v6(void) {
    gpio_init() ;
    timer_init() ;
    uart_init() ;
    interrupts_init() ; // interrupt sandwich start
    remote_init(GPIO_PB1, GPIO_PB0, GPIO_PB6,TEMPO_DEFAULT) ; 
    buzzer_init(GPIO_PB6) ; 
    interrupts_global_enable() ; // interrupt sandwich end
    timer_delay(2) ;

    remote_is_button_press() ; // get rid of the extra button press... 

    game_interlude_init(30, 50, GL_WHITE, GL_INDIGO) ; // can do this outside

    while(1) {
        game_update_init(20, 10);
        falling_piece_t piece = init_falling_piece();

        // write accelerometer x/y position to pitch(x) and roll(y)
        int pitch = 0; int roll = 0;
        long n = 480 ; // total ms wait for each loop
        n = (n * 1000 * TICKS_PER_USEC);

        int toggle_turns = 0 ;

        while(1) {
            while (timer_get_ticks() % n <= (0.8 * n)) {
                toggle_turns += 1 ; toggle_turns %= (3*9) ; // so we don't overflow

                // get accelerometer readings
                remote_get_x_y_status(&pitch, &roll); // the x and y tilt statuses
            
                if (toggle_turns % 9 == 0) {
                    if (pitch == X_SWAP) swap(&piece);
                }

                if (toggle_turns % 3 == 0) {
                    if (roll == LEFT) move_left(&piece);
                    else if (roll == RIGHT) move_right(&piece); 
                }

                // drop a block faster
                if (pitch == X_FAST) { 
                    if (!piece.fallen) move_down(&piece);
                    if (!piece.fallen) move_down(&piece);
                }

                while (remote_is_button_press()) rotate(&piece);
                if (piece.fallen) {
                    remote_get_x_y_status(&pitch, &roll); // the x and y tilt statuses
            
                    // horizontal movement
                    if (roll == LEFT) {
                        move_left(&piece);
                    }
                    else if (roll == RIGHT) {
                        move_right(&piece); 
                    }

                    if (iterateVariant(&piece, checkIfFallen)) {
                        iterateThroughPieceSquares(&piece, update_background);
                        clearRows();
                        piece = init_falling_piece();
                    }
                }
            } 
            
            move_down(&piece);
            if (game_update_is_game_over()) {timer_delay(2); break;} // exits game-playing mode if game is over

            while (timer_get_ticks() % n > (0.8 * n)) {};
        } 

        game_interlude_print_leaderboard(game_update_get_score(), game_update_get_rows_cleared()); 
    }
}


// using timer interrupt for music
void integration_test_v8(void) {
    gpio_init() ;
    timer_init() ;
    uart_init() ;
    interrupts_init() ; // interrupt sandwich start
    // buzzer_init(GPIO_PB6) ;
    buzzer_intr_init(GPIO_PB6, TEMPO_VIVACE) ; // this uses both timer0 and timer1 for the pwm and note-change
    interrupts_global_enable() ; // interrupt sandwich end
    timer_delay(2) ;

    gl_init(400, 400, GL_DOUBLEBUFFER) ;
    gl_clear(GL_BLUE) ;
    gl_swap_buffer() ;
    gl_clear(GL_BLUE) ;
    timer_delay(2) ;

    while(1) {
        // // confirm the buzzer works
        // buzzer_play_note(NOTE_FREQ_A_3, NOTE_EIGHTH) ; 
        // buzzer_play_note(NOTE_FREQ_D, NOTE_EIGHTH) ;
        
        gl_draw_line((timer_get_ticks()*12)%400, timer_get_ticks()%400, timer_get_ticks()%400, (timer_get_ticks()*12)%400, GL_AMBER) ;
        gl_swap_buffer() ;
        timer_delay(2) ;
    }
}

// TETRIS THEME interrupt version!
void integration_test_v9(void) {
    gpio_init() ;
    timer_init() ;
    uart_init() ;
    interrupts_init() ; // interrupt sandwich start
    remote_init(GPIO_PB1, GPIO_PB0, GPIO_PB6, TEMPO_ALLEGRO) ;  // buzzer interrupt moved into remote_init
    interrupts_global_enable() ; // interrupt sandwich end
    timer_delay(2) ;

    remote_is_button_press() ; // get rid of the extra button press... 

    game_interlude_init(30, 50, GL_WHITE, GL_INDIGO) ; // can do this outside

    while(1) {
        game_update_init(20, 10);
        falling_piece_t piece = init_falling_piece();

        // write accelerometer x/y position to pitch(x) and roll(y)
        int pitch = 0; int roll = 0;
        long n = 480 ; // total ms wait for each loop
        n = (n * 1000 * TICKS_PER_USEC);

        int toggle_turns = 0 ;

        while(1) {
            while (timer_get_ticks() % n <= (0.8 * n)) {
                toggle_turns += 1 ; toggle_turns %= (3*9) ; // so we don't overflow

                // get accelerometer readings
                remote_get_x_y_status(&pitch, &roll); // the x and y tilt statuses
            
                if (toggle_turns % 9 == 0) {
                    if (pitch == X_SWAP) swap(&piece);
                }

                // horizontal movement or swap
                if (toggle_turns % 3 == 0) {
                    if (roll == LEFT) move_left(&piece);
                    else if (roll == RIGHT) move_right(&piece); 
                }

                // drop a block faster
                if (pitch == X_FAST) { 
                    if (!piece.fallen) move_down(&piece);
                    if (!piece.fallen) move_down(&piece);
                }

                while (remote_is_button_press()) rotate(&piece);
                if (piece.fallen) {
                    remote_get_x_y_status(&pitch, &roll); // the x and y tilt statuses
            
                    // horizontal movement
                    if (roll == LEFT) {
                        move_left(&piece);
                    }
                    else if (roll == RIGHT) {
                        move_right(&piece); 
                    }

                    if (iterateVariant(&piece, checkIfFallen)) {
                        iterateThroughPieceSquares(&piece, update_background);
                        clearRows(); // inside clear rows: now, we get and update the tempo +=2 for every line cleared
                        piece = init_falling_piece();
                    }
                }
            } 
            
            move_down(&piece);
            if (game_update_is_game_over()) {timer_delay(2); break;} // exits game-playing mode if game is over

            while (timer_get_ticks() % n > (0.8 * n)) {};
        } 

        game_interlude_print_leaderboard(game_update_get_score(), game_update_get_rows_cleared()); 
    }
}


// TETRIS THEME interrupt version!
void integration_test_v10(void) {
    gpio_init() ;
    timer_init() ;
    uart_init() ;
    interrupts_init() ; // interrupt sandwich start
    remote_init(GPIO_PB1, GPIO_PB0, GPIO_PB6, TEMPO_ALLEGRO) ;  // buzzer interrupt moved into remote_init
    interrupts_global_enable() ; // interrupt sandwich end
    timer_delay(2) ;

    remote_is_button_press() ; // get rid of the extra button press... 

    game_interlude_init(30, 50, GL_WHITE, GL_INDIGO) ; // can do this outside

    while(1) {
        game_update_init(20, 10);
        falling_piece_t piece = init_falling_piece();
        buzzer_intr_set_tempo(TEMPO_ALLEGRO) ;

        // write accelerometer x/y position to pitch(x) and roll(y)
        int pitch = 0; int roll = 0;
        // int n_init = 480 ;
        long n = 480 ; // total ms wait for each loop
        n = (n * 1000 * TICKS_PER_USEC);

        int toggle_turns = 0 ;
        
        startGame();

        while(1) {
            while (timer_get_ticks() % n <= (0.8 * n)) {
                toggle_turns += 1 ; toggle_turns %= (3*9) ; // so we don't overflow

                // get accelerometer readings
                remote_get_x_y_status(&pitch, &roll); // the x and y tilt statuses
            
                if (toggle_turns % 7 == 0) {
                    if (pitch == X_SWAP) swap(&piece);
                }

                // horizontal movement or swap
                if (toggle_turns % 3 == 0) {
                    if (roll == LEFT) move_left(&piece);
                    else if (roll == RIGHT) move_right(&piece); 
                }

                while (remote_is_button_press()) rotate(&piece);
                if (piece.fallen) {
                    remote_get_x_y_status(&pitch, &roll); // the x and y tilt statuses
            
                    // horizontal movement
                    if (roll == LEFT) {
                        move_left(&piece);
                    }
                    else if (roll == RIGHT) {
                        move_right(&piece); 
                    }

                    if (iterateVariant(&piece, checkIfFallen)) {
                        iterateThroughPieceSquares(&piece, update_background);
                        clearRows(); // inside clear rows: now, we get and update the tempo +=2 for every line cleared
                        piece = init_falling_piece();
                    }
                }

                // drop a block faster
                if (pitch == X_FAST) { 
                    if (!piece.fallen) move_down(&piece);
                    if (!piece.fallen) move_down(&piece);
                }
            } 
            
            move_down(&piece);
            if (game_update_is_game_over()) {timer_delay(2); break;} // exits game-playing mode if game is over

            while (timer_get_ticks() % n > (0.8 * n)) {};
        } 

        game_interlude_print_leaderboard(game_update_get_score(), game_update_get_rows_cleared()); 
    }
}






