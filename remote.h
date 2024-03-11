#ifndef REMOTE_H
#define REMOTE_H

#include "ringbuffer.h"
/*
 * Module for remote control for Tetris game
 * Author: Aditi Bhaskar (aditijb@stanford.edu)
 */

// remote struct 
typedef struct {
    // typedef union { // todo del
    gpio_id_t servo ;
    gpio_id_t button ;
    rb_t *rb ;
    // }
} remote_t;

// initializes button and servo
void remote_init(gpio_id_t servo_id, gpio_id_t button_id) ;

// button
// handled via interrupt
bool is_button_press(void) ; // returns whether there is a button press in rb queue


// servo
void remote_vibrate(int duration_sec) ;

// accelerometer
int get_tilt() ;
bool is_drop() ;

#endif