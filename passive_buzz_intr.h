/* passive_buzz_intr.h
 * Module to send buzzer tones to buzzer with interrupt-based setup. Yay for multitasking!
 * Author: Aditi (aditijb@stanford.edu)
 */

#ifndef PBINTERRUPT_H
#define PBINTERRUPT_H

#include "gpio.h"
#include <stdbool.h>
#include "music.h"

/* 'buzzer_intr_init'
 * @param gpio_id_t id - buzzer GPIO id
 * @param int tempo_ - music tempo. use a reasonable tempo (choose from music.h's tempo enum options)
 * @functionality - init the interrupt system for the buzzer and start playing tetris song!
 * ask yourself: Do I want to multitask and play a song continuously in the background? 
 *      Yes: You are in the right place. 
 *      No: Want to play a single note and *not* multitask? See passive_buzz.c
*/
void buzzer_intr_init(gpio_id_t id, int tempo_) ; // for timer-interrupt based music 

/* 'buzzer_intr_set_tempo'
 * @param int tempo_ - new tempo you want to set to the buzzer. 
 * @functionality - change tempo. add a reasonable tempo (choose from music.h's tempo enum options). The song continues and does not restart
*/
void buzzer_intr_set_tempo(int tempo_) ;

/* 'buzzer_intr_get_tempo'
 * @functionality - get tempo. note that there may be rounding issues (so it may not match the tempo enum in music.h)
*/
int buzzer_intr_get_tempo(void) ;

/* 'buzzer_intr_restart_song'
 * @functionality - restarts the song on the next note
*/
void buzzer_intr_restart_song(void) ;


// todo comment

void buzzer_intr_pause(void) ;
void buzzer_intr_play(void) ;
bool buzzer_intr_is_playing(void) ;


#endif