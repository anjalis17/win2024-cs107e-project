#ifndef PBINTERRUPT_H
#define PBINTERRUPT_H
/*
 * Module to send buzzer tones to buzzer with interrupt-based setup. Yay multitasking!
 * Author: Aditi Bhaskar (aditijb@stanford.edu)
 */

#include "gpio.h"
#include <stdbool.h>
#include "music.h"

// todo add tempo param
void buzzer_init_interrupt(gpio_id_t id) ; // for timer-interrupt based music 

// todo add function to change tempo
// todo add function to init with song ?? maybe ...

#endif