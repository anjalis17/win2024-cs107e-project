/* passive_buzz.c
 * Module to play buzzer tones to buzzer
 * Author: Aditi Bhaskar (aditijb@stanford.edu)
 */

#include "gpio.h"
#include "timer.h"
#include "printf.h"
#include "passive_buzz.h"
#include "interrupts.h"
#include "hstimer.h"

#define TICKS_PER_USEC 24 // 24 ticks counted per one microsecond
#define uSEC_IN_SEC 1000000
#define SONG tetris_song // can declare and just start using a new song :)

static gpio_id_t buzzer_id ;
static int tempo ; // in beats per minute
static int song_index ;

// 'buzzer_init'
// initializes buzzer
void buzzer_init(gpio_id_t id) {
    gpio_set_output(id) ;
    buzzer_id = id ;
    tempo = TEMPO_DEFAULT;
}


// 'buzzer_set_tempo'
// set the tempo for the buzzer
// returns false if invalid tempo
// TODO test tempo ranges!! > andante doesn't work
bool buzzer_set_tempo(int new_tempo) {
    if (new_tempo > 50 && new_tempo < 200) {
        tempo = new_tempo ;
        return true ;
    }
    return false ;
}

// 'buzzer_get_tempo'
// returns current tempo
int buzzer_get_tempo(void) {
    return tempo ;
}

// 'buzzer_play_note'
// plays a note on the buzzer
// code citation: https://blog.tarkalabs.com/digital-audio-101-playing-audio-from-a-microcontroller-5df1463616c
// "The formula for calculating period is period (in sec) = 1/frequency (in Hz)."
void buzzer_play_note(int frequency, int duration_msec) {
    printf("\nfreq: %d\n", frequency) ;

    duration_msec = duration_msec * 60 / tempo ; // updates speed of song based on tempo

    unsigned long busy_wait_until = (timer_get_ticks() + (duration_msec*1000*TICKS_PER_USEC))  ; // turn into milliseconds
    int note_frequency = frequency ; 
    int note_period = (uSEC_IN_SEC / note_frequency) ; // keep it approximate

    while (timer_get_ticks() < busy_wait_until) {
        double total_duration = 0 ;
        while(total_duration <= (uSEC_IN_SEC/4)) {
            total_duration += note_period; 
            gpio_write(buzzer_id, 1);
            timer_delay_us(note_period/2);
            gpio_write(buzzer_id, 0);
            timer_delay_us(note_period/2);
        }
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// thanks to Antonio for mentioning https://cs107e.github.io/header#hstimer had the timer_interrupt_clear function

const int tetris_song[8*4*6] = // all notes are eigth notes. each line is a measure. each 4 lines is a grouped musical phrase
                    {
                        // theme 
                        NOTE_FREQ_E, NOTE_FREQ_E, NOTE_FREQ_B_3, NOTE_FREQ_C, NOTE_FREQ_D, NOTE_FREQ_D, NOTE_FREQ_C, NOTE_FREQ_B_3, 
                        NOTE_FREQ_A_3, NOTE_FREQ_A_3, NOTE_FREQ_A_3, NOTE_FREQ_C, NOTE_FREQ_E, NOTE_FREQ_E, NOTE_FREQ_D, NOTE_FREQ_C,
                        NOTE_FREQ_B_3, NOTE_FREQ_B_3, NOTE_FREQ_B_3, NOTE_FREQ_C, NOTE_FREQ_D, NOTE_FREQ_D, NOTE_FREQ_E, NOTE_FREQ_E,
                        NOTE_FREQ_C, NOTE_FREQ_C, NOTE_FREQ_A_3, NOTE_FREQ_A_3, NOTE_FREQ_A_3, NOTE_FREQ_A_3, NOTE_FREQ_A_3, NOTE_FREQ_A_3,
                        
                        NOTE_FREQ_D, NOTE_FREQ_D, NOTE_FREQ_D, NOTE_FREQ_F, NOTE_FREQ_A, NOTE_FREQ_A, NOTE_FREQ_G, NOTE_FREQ_F, 
                        NOTE_FREQ_E, NOTE_FREQ_E, NOTE_FREQ_E, NOTE_FREQ_C, NOTE_FREQ_E, NOTE_FREQ_E, NOTE_FREQ_D, NOTE_FREQ_C,
                        NOTE_FREQ_B_3, NOTE_FREQ_B_3, NOTE_FREQ_B_3, NOTE_FREQ_C, NOTE_FREQ_D, NOTE_FREQ_D, NOTE_FREQ_E, NOTE_FREQ_E,
                        NOTE_FREQ_C, NOTE_FREQ_C, NOTE_FREQ_A_3, NOTE_FREQ_A_3, NOTE_FREQ_A_3, NOTE_FREQ_A_3, NOTE_FREQ_A_3, NOTE_FREQ_A_3,
                        
                        // theme again
                        NOTE_FREQ_E, NOTE_FREQ_E, NOTE_FREQ_B_3, NOTE_FREQ_C, NOTE_FREQ_D, NOTE_FREQ_D, NOTE_FREQ_C, NOTE_FREQ_B_3, 
                        NOTE_FREQ_A_3, NOTE_FREQ_A_3, NOTE_FREQ_A_3, NOTE_FREQ_C, NOTE_FREQ_E, NOTE_FREQ_E, NOTE_FREQ_D, NOTE_FREQ_C,
                        NOTE_FREQ_B_3, NOTE_FREQ_B_3, NOTE_FREQ_B_3, NOTE_FREQ_C, NOTE_FREQ_D, NOTE_FREQ_D, NOTE_FREQ_E, NOTE_FREQ_E,
                        NOTE_FREQ_C, NOTE_FREQ_C, NOTE_FREQ_A_3, NOTE_FREQ_A_3, NOTE_FREQ_A_3, NOTE_FREQ_A_3, NOTE_FREQ_A_3, NOTE_FREQ_A_3,
                        
                        NOTE_FREQ_D, NOTE_FREQ_D, NOTE_FREQ_D, NOTE_FREQ_F, NOTE_FREQ_A, NOTE_FREQ_A, NOTE_FREQ_G, NOTE_FREQ_F, 
                        NOTE_FREQ_E, NOTE_FREQ_E, NOTE_FREQ_E, NOTE_FREQ_C, NOTE_FREQ_E, NOTE_FREQ_E, NOTE_FREQ_D, NOTE_FREQ_C,
                        NOTE_FREQ_B_3, NOTE_FREQ_B_3, NOTE_FREQ_B_3, NOTE_FREQ_C, NOTE_FREQ_D, NOTE_FREQ_D, NOTE_FREQ_E, NOTE_FREQ_E,
                        NOTE_FREQ_C, NOTE_FREQ_C, NOTE_FREQ_A_3, NOTE_FREQ_A_3, NOTE_FREQ_A_3, NOTE_FREQ_A_3, NOTE_FREQ_A_3, NOTE_FREQ_A_3,

                        // slow falling part
                        NOTE_FREQ_E, NOTE_FREQ_E, NOTE_FREQ_E, NOTE_FREQ_E, NOTE_FREQ_C, NOTE_FREQ_C, NOTE_FREQ_C, NOTE_FREQ_C, 
                        NOTE_FREQ_D, NOTE_FREQ_D, NOTE_FREQ_D, NOTE_FREQ_D, NOTE_FREQ_B_3, NOTE_FREQ_B_3, NOTE_FREQ_B_3, NOTE_FREQ_B_3,
                        NOTE_FREQ_C, NOTE_FREQ_C, NOTE_FREQ_C, NOTE_FREQ_C, NOTE_FREQ_A_3, NOTE_FREQ_A_3, NOTE_FREQ_A_3, NOTE_FREQ_A_3,
                        NOTE_FREQ_G_SHARP_3, NOTE_FREQ_G_SHARP_3, NOTE_FREQ_G_SHARP_3, NOTE_FREQ_G_SHARP_3, NOTE_FREQ_G_SHARP_3, NOTE_FREQ_G_SHARP_3, NOTE_FREQ_G_SHARP_3, NOTE_FREQ_G_SHARP_3,
                        
                        NOTE_FREQ_E, NOTE_FREQ_E, NOTE_FREQ_E, NOTE_FREQ_E, NOTE_FREQ_C, NOTE_FREQ_C, NOTE_FREQ_C, NOTE_FREQ_C, 
                        NOTE_FREQ_D, NOTE_FREQ_D, NOTE_FREQ_D, NOTE_FREQ_D, NOTE_FREQ_B_3, NOTE_FREQ_B_3, NOTE_FREQ_B_3, NOTE_FREQ_B_3,
                        NOTE_FREQ_C, NOTE_FREQ_C, NOTE_FREQ_E, NOTE_FREQ_E, NOTE_FREQ_A, NOTE_FREQ_A, NOTE_FREQ_A, NOTE_FREQ_A,
                        NOTE_FREQ_G_SHARP_3, NOTE_FREQ_G_SHARP_3, NOTE_FREQ_G_SHARP_3, NOTE_FREQ_G_SHARP_3, NOTE_FREQ_G_SHARP_3, NOTE_FREQ_G_SHARP_3, NOTE_FREQ_G_SHARP_3, NOTE_FREQ_G_SHARP_3
                    } ;

// returns period in useconds
int freq_to_period_s(int frequency) {
    return (uSEC_IN_SEC / frequency) ;
}

static void handle_note_buzz(uintptr_t pc, void *aux_data) {
    hstimer_interrupt_clear(HSTIMER0);

    // toggle buzzer
    if (gpio_read(buzzer_id) == 1) gpio_write(buzzer_id, 0);
    else gpio_write(buzzer_id, 1);

    hstimer_enable(HSTIMER0);
}

static void handle_note_change(uintptr_t pc, void *aux_data) {
    hstimer_interrupt_clear(HSTIMER1);

    // iterates to next note in the song 
    song_index = (song_index+1) % (8*4*6); // 8*4*6 is the number of notes in the tetris song

    // changes the frequency that the buzzer will buzz at
    // remember i need to use note freq / 2 for the toggle in handle_note_buzz to work
    hstimer_init(HSTIMER0, (freq_to_period_s(SONG[song_index]) / 2)); 

    hstimer_enable(HSTIMER1);
}


// todo aditi! keep it static for now while I figure it out
void buzzer_init_interrupt(gpio_id_t id) {

    // pg 1086: Table 9-21 PB Multiplex Function    
    //      PB6, function 5 : PWM1
    // gpio_set_function(GPIO_PB6, GPIO_FN_ALT5) ;

    gpio_set_output(id) ;
    buzzer_id = id ;
    tempo = TEMPO_DEFAULT;

    // initializing interrupt system to listen for timer

    // to pwm the note
    interrupts_enable_source(INTERRUPT_SOURCE_HSTIMER0); //= 71, # INTERRUPT_SOURCE_HSTIMER1 = 72,
    interrupts_register_handler(INTERRUPT_SOURCE_HSTIMER0, handle_note_buzz, NULL) ;
    hstimer_init(HSTIMER0, 1) ; // todo update this number
    hstimer_enable(HSTIMER0) ;

    // to change which note is playing
    interrupts_enable_source(INTERRUPT_SOURCE_HSTIMER1); 
    interrupts_register_handler(INTERRUPT_SOURCE_HSTIMER1, handle_note_change, NULL) ;
    hstimer_init(HSTIMER1, 1000000) ; // todo update this number to be the tempo
    hstimer_enable(HSTIMER1) ;

    song_index = 0 ;

}



// // new, hands-off way to do pwm :) thanks Javier for the suggestion ... will it work? idk
// // todo test

// // todo create a struct for this stuff??
// static int duration ;
// static int period ;

// // init for every single note change!!
// void buzzer_freq_init(int note_duration, int note_frequency) {
//     duration = duration; // the duration of time for an eighth note to play, in ms
//     period = freq_to_period(note_frequency) ;
// }

// // for client:
// // input: int note_period = (uSEC_IN_SEC / music_notes[music_index]) ; 
// //  aka   int note_period = (uSEC_IN_SEC / NOTE_FREQ) ;  
// void buzzer_timing_play_note(void) {
//     // check timer
//     // if % frequency >/< 1/2 frequency, turn on/off for that note
//     if ((timer_get_ticks() - start_ticks) % (note_period) < (note_period / 2)) {
//         gpio_write(buzzer_id, 1);
//     } else {
//         gpio_write(buzzer_id, 0);
//     }
// }

