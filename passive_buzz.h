#ifndef PB_H
#define PB_H
/*
 * Module to send buzzer tones to buzzer
 * Author: Aditi (aditijb@stanford.edu)
 */

#pragma once
#include "gpio.h"
#include <stdbool.h>
#include "music.h"

/* buzzer_init
 * @param gpio_id_t id - buzzer GPIO id
 * @functionality - initializes the buzzer as an output
 * ask yourself: Want to play a single note and *not* multitask? 
 *      Yes: You are in the right place.
 *      No: Do you want multitask and play a song continuously in the background? See passive_buzz_intr.c 
*/
void buzzer_init(gpio_id_t id) ; 

/* buzzer_set_tempo
 * @param int new_tempo - new tempo (see music.h for acceptable tempos)
 * @functionality - update the tempo (bpm) we want the buzzer to buzz at
*/
// PENDING ISSUE! tempo ranges > andante don't work. RESOLVED in passive_buzz_intr.c
bool buzzer_set_tempo(int new_tempo) ;

/* buzzer_get_tempo
 * @returns int tempo - 
 * @functionality - returns the currrent tempo (bpm) 
*/
int buzzer_get_tempo(void) ;

/* buzzer_play_note
 * @param int frequency - note frequency (see music.h for notes)
 * @param int duration_msec - note duration in milliseconds (see music.h for durations like whole/half/etc.)
 * @functionality - plays a note at frequency `frequency` for duration `duration_msec` in milliseconds
 * Note: durations < 200ms do not play
*/
void buzzer_play_note(int frequency, int duration_msec) ; 

#endif