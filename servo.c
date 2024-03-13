/*
 * Module to send tasks to servo
 * Author: Aditi Bhaskar (aditijb@stanford.edu)
 */

#include "gpio.h"
#include "timer.h"

#define TICKS_PER_USEC 24 // 24 ticks counted per one microsecond

static gpio_id_t servo_id ;

// 'servo_init'
// initializes servo
void servo_init(gpio_id_t id) {
    gpio_set_output(id) ;
    servo_id = id ;
}

// 'servo_turn'
// sends a single pulse to the servo to turn it to a position
// https://learn.sparkfun.com/tutorials/hobby-servo-tutorial#servo-motor-background
// The pulses occur at a 20 mSec (50 Hz) interval, and vary between 1 and 2 mSec in width.
// @param scale     can be anywhere from -1 (1000 milliseconds) to 1 (2000 milliseconds)
//                  goes to any position in the range
//                  1 goes ccw and -1 goes cw (looking down on servo's shaft)
void servo_turn(int scale) { 
    gpio_write(servo_id, 1) ;
    timer_delay_us(1500 + scale*(500)) ; // turn it into milliseconds
    gpio_write(servo_id, 0) ;
    timer_delay_us(20000 - (1500 + scale*(500))) ; // 19 millisecond
}

// 'servo_vibrate'
// vibrates by turning servo back and forth
void servo_vibrate(int duration_sec) {
    unsigned long busy_wait_until = timer_get_ticks() + duration_sec * TICKS_PER_USEC * 1000000; // convert to seconds
    while (timer_get_ticks() < busy_wait_until) { // spin
        servo_turn(1) ;
        servo_turn(-1) ;
    }
}

// 'servo_vibrate_milli_sec'
// vibrates by turning servo back and forth for duration (in milliseconds)
void servo_vibrate_milli_sec(int duration_milli_sec) {
    unsigned long busy_wait_until = timer_get_ticks() + duration_milli_sec * TICKS_PER_USEC * 1000; // convert to milli_seconds
    while (timer_get_ticks() < busy_wait_until) { // spin
        servo_turn(1) ;
        servo_turn(-1) ;
    }
}
