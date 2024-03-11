#ifndef SERVO_H
#define SERVO_H
/*
 * Module to send signals to servo
 * Author: Aditi Bhaskar (aditijb@stanford.edu)
 */

void servo_init(gpio_id_t id) ;

void servo_turn(int scale) ;

void servo_vibrate(int duration_sec) ;

void servo_vibrate_milli_sec(int duration_milli_sec) ;


#endif