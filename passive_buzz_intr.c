/* passive_buzz.c
 * Module to send buzzer tones to buzzer with interrupt-based setup. Yay multitasking!
 * Author: Aditi Bhaskar (aditijb@stanford.edu)
 * credit: thanks to Antonio for mentioning https://cs107e.github.io/header#hstimer had the timer_interrupt_clear function
 */

#include "gpio.h"
#include "passive_buzz_intr.h"
#include "interrupts.h"
#include "hstimer.h"
#include "music.h"
#include <stddef.h>

#define uSEC_IN_SEC 1000000
#define SONG tetris_song // can declare and start using a new song

static gpio_id_t buzzer_id ;
static int tempo ; // in beats per minute

const int song_length = 8*4*6 ;
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

static int song_index ;

// freq_to_period_us
// @param frequency in Hertz (1/s)
// @returns period in useconds
static int freq_to_period_us(int frequency) {
    return (uSEC_IN_SEC / frequency) ;
}

// `handle_note_buzz`
// handler for INTERRUPT_SOURCE_HSTIMER0
// uses HSTIMER0's countdown to manually PWM the buzzer at the correct frequency
static void handle_note_buzz(uintptr_t pc, void *aux_data) {
    hstimer_interrupt_clear(HSTIMER0);

    // toggle buzzer
    if (gpio_read(buzzer_id) == 1) gpio_write(buzzer_id, 0);
    else gpio_write(buzzer_id, 1);

    hstimer_enable(HSTIMER0);
}

// `handle_note_change`
// handler for INTERRUPT_SOURCE_HSTIMER1
// uses HSTIMER1's countdown to indicate when the note should change to the next 
//      note, and updates HSTIMER0's frequency to the appropriate note
static void handle_note_change(uintptr_t pc, void *aux_data) {
    hstimer_interrupt_clear(HSTIMER1);

    // iterates to next note in the song 
    song_index = (song_index+1) % song_length; // 8*4*6 is the number of notes in the tetris song

    // changes the frequency that the buzzer will buzz at
    // remember I need to use note freq / 2 for the toggle in handle_note_buzz to work
    hstimer_init(HSTIMER0, (freq_to_period_us(SONG[song_index]) / 2)); 
    hstimer_enable(HSTIMER0) ;

    hstimer_enable(HSTIMER1);
}

// `buzzer_init_interrupt`
// initializes the interrupts which play the tetris theme
void buzzer_init_interrupt(gpio_id_t id) {
    gpio_set_output(id) ;
    buzzer_id = id ;
    tempo = TEMPO_VIVACE * 2; // todo make this a param, see I multiply by 2 because I do my stuff in 8th notes, not quarter

    // initializing interrupt system to listen for timer

    // INTERRUPT_SOURCE_HSTIMER0 to pwm the note
    interrupts_enable_source(INTERRUPT_SOURCE_HSTIMER0); //= 71, # INTERRUPT_SOURCE_HSTIMER1 = 72,
    interrupts_register_handler(INTERRUPT_SOURCE_HSTIMER0, handle_note_buzz, NULL) ;
    hstimer_init(HSTIMER0, freq_to_period_us(SONG[song_index]) / 2) ; 
    hstimer_enable(HSTIMER0) ;

    // INTERRUPT_SOURCE_HSTIMER1 to change which note is playing
    interrupts_enable_source(INTERRUPT_SOURCE_HSTIMER1); 
    interrupts_register_handler(INTERRUPT_SOURCE_HSTIMER1, handle_note_change, NULL) ;
    // hstimer_init(HSTIMER1, 300000) ; 
    hstimer_init(HSTIMER1, 54000000/tempo) ; // todo div by tempo better
    hstimer_enable(HSTIMER1) ;

    song_index = 0 ;
}
