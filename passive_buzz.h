/*
 * Module to send buzzer tones to buzzer
 * Author: Aditi Bhaskar (aditijb@stanford.edu)
 */

#pragma once
#include "gpio.h"
#include <stdbool.h>

enum { // NOTE FREQUENCY
    
    // in hertz 
    // https://miro.medium.com/v2/resize:fit:618/format:webp/1*K7y56Rd1kolwV_GNOpmwEg.png

    // most of the notes are in the 4th octave; notes outside 4th octave are explicitly named with the octave number after the note like NOTE_FREQ_B_3
    // notes lower on the piano are towards the top of this list 
    
    NOTE_FREQ_G_SHARP_3 = 208, 
    NOTE_FREQ_A_3 = 220, 
    NOTE_FREQ_A_SHARP_3 = 233,
    NOTE_FREQ_B_3 = 247, 
    NOTE_FREQ_C = 262, // 261.53,
    NOTE_FREQ_C_SHARP = 277, // 277.18,
    NOTE_FREQ_D = 294, // 293.66
    NOTE_FREQ_D_SHARP = 311,
    NOTE_FREQ_E = 330, 
    NOTE_FREQ_F = 349,
    NOTE_FREQ_F_SHARP = 370,
    NOTE_FREQ_G = 392,
    NOTE_FREQ_G_SHARP = 415,
    NOTE_FREQ_A = 440,
    NOTE_FREQ_A_SHARP = 466,
    NOTE_FREQ_B = 494
} ;

enum { // NOTE LENGTH
    NOTE_WHOLE = 2000,
    NOTE_HALF = 1000,
    NOTE_QUARTER = 500,
    NOTE_EIGHTH = 250,
    NOTE_SIXTEENTH = 125
} ;

enum { // TEMPO
    TEMPO_LENTO = 55,
    TEMPO_DEFAULT = 60,
    TEMPO_ADAGIO = 65,
    TEMPO_ANDANTE = 85,
    TEMPO_MODERATO = 115,
    TEMPO_ALLEGRETTO = 120,
    TEMPO_ALLEGRO = 140,
    TEMPO_VIVACE = 160,
} ;

void buzzer_init(gpio_id_t id) ; 

// update the tempo (bpm) we want the buzzer to buzz at
bool buzzer_set_tempo(int new_tempo) ;

// returns the currrent tempo (bpm) 
int buzzer_get_tempo(void) ;

// plays a note at frequency `frequency` for duration `duration_msec` in milliseconds
// durations < 200ms do not play
void buzzer_play_note(int frequency, int duration_msec) ; 
