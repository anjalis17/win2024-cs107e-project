#ifndef PB_H
#define PB_H
/*
 * Module to send buzzer tones to buzzer
 * Author: Aditi Bhaskar (aditijb@stanford.edu)
 */

#pragma once
#include "gpio.h"
#include <stdbool.h>
#include "music.h"

void buzzer_init(gpio_id_t id) ; 

// update the tempo (bpm) we want the buzzer to buzz at
bool buzzer_set_tempo(int new_tempo) ;

// returns the currrent tempo (bpm) 
int buzzer_get_tempo(void) ;

// plays a note at frequency `frequency` for duration `duration_msec` in milliseconds
// durations < 200ms do not play
void buzzer_play_note(int frequency, int duration_msec) ; 

#endif