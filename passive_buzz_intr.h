#ifndef PBINTERRUPT_H
#define PBINTERRUPT_H
/*
 * Module to send buzzer tones to buzzer with interrupt-based setup. Yay multitasking!
 * Author: Aditi Bhaskar (aditijb@stanford.edu)
 */

#include "gpio.h"
#include <stdbool.h>
#include "music.h"

// init the interrupt system for the buzzer and start playing tetris song! add a reasonable tempo (choose from music.h's tempo enum options)
void buzzer_intr_init(gpio_id_t id, int tempo_) ; // for timer-interrupt based music 

// change tempo. add a reasonable tempo (choose from music.h's tempo enum options)
void buzzer_intr_change_tempo(int tempo_) ;

#endif