/*
 * Module for remote control for Tetris game
 * Author: Aditi (aditijb@stanford.edu)
 * 
 * a physical remote object requires the following hardware: 
 *   accelerometer (lsd6ds33)
 *   servo (small 5v 180 degree servo works)
 *   button (click! click!)
 *   buzzer (passive buzzer)
 *   neon-green casing as seen in /documents is ergonomic and preffered :)
 */
#ifndef REMOTE_H
#define REMOTE_H

#include "ringbuffer.h"
#include "gpio.h"

/* remote_t struct
 * stores gpio id's of each component 
 * stores rb to store interrupts registered from button presses 
 */
typedef struct {
    gpio_id_t servo ; 
    gpio_id_t button ;
    gpio_id_t buzzer ;
    rb_t *rb ;
} remote_t;

/* remote_init
 * @params gpio_id_t servo_id, gpio_id_t button_id, gpio_id_t buzzer_id - GPIO ID of all physical components in remote 
 *                                                                      - this does not include the accelerometer
 * @param int music_tempo - tempo for the buzzer to play music at. the buzzer is initialized using passive_buzz_intr
 *                        -     to utilize multitasking to play music for the entire duration of the game/interludes
 * @functionality - initalizes remote components (and remote)
*/
void remote_init(gpio_id_t servo_id, gpio_id_t button_id, gpio_id_t buzzer_id, int music_tempo) ;

/* remote_is_button_press
 * @return - whether there is a queued button press (as registered by interrupt handler)
 * @functionality - checks for pending button press and dequeues if there is one
*/
bool remote_is_button_press(void) ; 

/* remote_vibrate
 * @param int duration_sec - duration of remote vibration in seconds
 * @functionality - calls on servo to vibrate for duration_sec seconds
*/
void remote_vibrate(int duration_sec) ;

/* remote_get_x_y_status
 * @param int *x, int *y - user-passed ints to receive data about x and y positions from accelerometer
 * @return - technically, through the params
 * @functionality - calls on accelerometer (LSD6DS33) to get x and y status in user-friendly format (as defined in LSD6DS33.h)
 *                -         x = {X_HOME, X_FAST, X_SWAP}
 *                -         y = {LEFT, HOME, RIGHT }
*/
void remote_get_x_y_status(int *x, int *y) ;

#endif