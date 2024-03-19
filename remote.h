/*
 * Module for remote control for Tetris game
 * Author: Aditi (aditijb@stanford.edu)
 */
#ifndef REMOTE_H
#define REMOTE_H

#include "ringbuffer.h"
#include "gpio.h"

// remote struct 
typedef struct {
    gpio_id_t servo ;
    gpio_id_t button ;
    rb_t *rb ;
} remote_t;

// initializes button and servo
void remote_init(gpio_id_t servo_id, gpio_id_t button_id, gpio_id_t buzzer_id, int music_tempo) ;

// button
// handled via interrupt
bool remote_is_button_press(void) ; // returns whether there is a button press in rb queue

// servo
void remote_vibrate(int duration_sec) ;

// accelerometer
void remote_get_x_y_status(int *x, int *y) ;

#endif