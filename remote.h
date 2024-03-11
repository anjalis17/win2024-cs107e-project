#ifndef REMOTE_H
#define REMOTE_H
/*
 * Module for remote control for Tetris game
 * Author: Aditi Bhaskar (aditijb@stanford.edu)
 */

// initializes button and servo
void remote_init(gpio_id_t servo_id, gpio_id_t button_id) ; // gpio_id_t accelerometer_id

// button
// handled via interrupt

// servo
void remote_vibrate(int duration_sec) ;

// accelerometer
int get_tilt() ;
bool is_drop() ;



#endif