#ifndef SERVO_H
#define SERVO_H
/*
 * Module to send signals to a 180 degree servo
 * Author: Aditi (aditijb@stanford.edu)
 */

/* servo_init
 * @param gpio_id_t id - GPIO ID of servo
 * @functionality - initializes the servo as an output GPIO
*/
void servo_init(gpio_id_t id) ;

/* servo_turn
 * @param int scale - legal range: -1 to 1
 *                  - 1 goes ccw and -1 goes cw from 0 (looking down on the servo's shaft).
 *                  - each value in the range is a position
 * @functionality - sends a single pulse to the servo to turn it to a position
*/
void servo_turn(int scale) ;

/* servo_vibrate
 * @param int duration_sec - duration for vibration in seconds
 * @functionality - vibrates by turning servo back and forth
*/
void servo_vibrate(int duration_sec) ;

/* servo_vibrate_milli_sec
 * @param int duration_milli_sec - duration for vibration in ms
 * @functionality - vibrates by turning servo back and forth
*/
void servo_vibrate_milli_sec(int duration_milli_sec) ;

#endif