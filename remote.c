/*
 * Module for remote control for Tetris game
 * Author: Aditi Bhaskar (aditijb@stanford.edu)
 */

#include "gpio.h"
#include "timer.h"
#include "servo.h"
#include "gpio_interrupt.h"
#include "interrupts.h"
#include "uart.h"
#include <stddef.h>
#include "LSD6DS33.h"
#include "i2c.h"
#include "game_update.h"

static struct {
    gpio_id_t servo ;
    gpio_id_t button ;
    
} remote ;

// handles a button press 
static void handle_button(uintptr_t pc, void *aux_data) {
    gpio_interrupt_clear(remote.button) ;

    // todo implm the ring buffer so I can dequeue in main game
    // rotate(&piece);
    uart_putchar('b') ; // todo do something with this related to the game
    servo_vibrate_milli_sec(100) ;
    // todo turn into an RBQ and dequeue when we want to potentially vibrate/do something onscreen
}

void remote_init(gpio_id_t servo_id, gpio_id_t button_id) { // todo add gpio_id_t accelerometer_id
    gpio_set_input(button_id) ;
    remote.button = button_id ;

    remote.servo = servo_id ;    
    servo_init(servo_id) ;

    // accelerometer init
    i2c_init();
	lsm6ds33_init();

    gpio_interrupt_init() ;
    gpio_interrupt_config(remote.button, GPIO_INTERRUPT_DOUBLE_EDGE, true) ; // while pressed
    gpio_interrupt_register_handler(remote.button, handle_button, NULL) ;
    gpio_interrupt_enable(remote.button) ;

}

// only vibrates the most recently configured servo
void remote_vibrate(int duration_sec) {
    servo_vibrate(duration_sec) ;
}

// returns int enum "left/right/home" ... enum defined in lsd6ds33.h
int get_tilt() {
    short x, y, z;
    int state = lsm6ds33_read_durable_pos_y(&x, &y, &z) ; // read and print avged positions
    return state ;
}

// returns true if remote is in "drop block" (tilt tip towards the ground) orientation
bool is_drop() {
    short x, y, z;
    int state = lsm6ds33_read_durable_pos_y(&x, &y, &z) ;
    return lsm6ds33_durable_pos_x(x);
}
