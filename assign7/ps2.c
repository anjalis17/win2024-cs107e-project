/* File: ps2_assign7.c
 * -------------------
 * Author: Anjali Sreenivas
 * 
 * The ps2.c file defines low-level functions that can be used to read input
 * from a PS/2 device such as a keyboard or mouse. Inputs are read and processed
 * using interrupts!
 */
#include "gpio.h"
#include "gpio_extra.h"
#include "gpio_interrupt.h"
#include "malloc.h"
#include "ps2.h"
#include "uart.h"
#include "ringbuffer.h"
#include "printf.h"

// A ps2_device is a structure that stores all of the state and information
// needed for a PS2 device. The clock field stores the gpio id for the
// clock pin, and the data field stores the gpio id for the data pin.
// Read ps2_new for example code that sets and uses these fields.
//
// You may extend the ps2_device structure with additional fields as needed.
// A pointer to the current ps2_device is passed into all ps2_ calls.
// Storing state in this structure is preferable to using global variables:
// it allows your driver to support multiple PS2 devices accessed concurrently
// (e.g., a keyboard and a mouse).
//
// This definition fills out the structure declared in ps2.h.
struct ps2_device {
    gpio_id_t clock;
    gpio_id_t data;
    unsigned char scancode;  // stores scancode currently being built up
    unsigned char bitCount;  // number of scancode bits received
    unsigned char countOnes; // stores number of 1s in scancode bits so far
    rb_t* ringbuffer;  // ringbuffer to store series of processed scancodes
};

// Define constants
enum { HIGH = 1, LOW = 0};
const int SCANCODE_LEN = 11; const int DATA_LEN = 8;

// Interrupt handler to handle input received from PS/2 device
void handle_read(uintptr_t pc, void* aux_data) {
    gpio_interrupt_clear(((ps2_device_t*) aux_data)->clock);
    ps2_device_t* dev = (ps2_device_t*) aux_data;
    unsigned char bit = gpio_read(dev->data);

    // handle start bit
    if (dev->bitCount == 0) {
        // if non-0 bit, don't process anything
        if (bit != LOW) return;
    }
    // if bit is a scancode data bit (bits 1 through 8)
    else if (bit == HIGH && (dev->bitCount <= DATA_LEN && dev->bitCount >= 1)) {
        dev->scancode |= (bit << (dev->bitCount - 1));
        dev->countOnes++;
    }
    // parity bit
    else if (dev->bitCount == 9) {
        if (bit == HIGH) dev->countOnes++;
        if (dev->countOnes % 2 != 1) {
            // reset state if we don't have an odd # of 1s among data and parity bits (invalid scancode)
            dev->scancode = 0; dev->countOnes = 0; dev->bitCount = 0;
            return; 
        }
    }
    // handle stop bit
    else if (dev->bitCount == 10) {
        // if stop bit is valid, enqueue scancode to ringbuffer
        if (bit == HIGH) {
            rb_enqueue(dev->ringbuffer, dev->scancode);
        }
        // reset state 
        dev->scancode = 0; dev->countOnes = 0; dev->bitCount = 0;
        return;
    }
    dev->bitCount++;
}

// Creates a new PS2 device connected to given clock and data pins,
// The gpios are configured as input and set to use internal pull-up
// (PS/2 protocol requires clock/data to be high default)
ps2_device_t *ps2_new(gpio_id_t clock_gpio, gpio_id_t data_gpio) {
    ps2_device_t *dev = malloc(sizeof(*dev));

    dev->clock = clock_gpio;
    gpio_set_input(dev->clock);
    gpio_set_pullup(dev->clock);

    dev->data = data_gpio;
    gpio_set_input(dev->data);
    gpio_set_pullup(dev->data);
    
    dev->ringbuffer = rb_new();
    dev->bitCount = 0;
    dev->scancode = 0;
    dev->countOnes = 0;

    gpio_interrupt_init();
    // configure interrupt on clock edge fall 
    gpio_interrupt_config(dev->clock, GPIO_INTERRUPT_NEGATIVE_EDGE, false);  // no debouncing for keyboard readings
    gpio_interrupt_register_handler(dev->clock, handle_read, dev); // register handle_read as clock interrupt handler function
    gpio_interrupt_enable(dev->clock);

    return dev;
}

// Helper function to read bit in data register after clock edge falls
unsigned char read_bit(ps2_device_t *dev) {
    // Wait until clock edge falls (bit val becomes LOW (0)) before reading
    while (gpio_read(dev->clock) == HIGH) { };
    return gpio_read(dev->data);
}

// Read a single PS2 scan code. Always returns a correctly received scan code:
// if an error occurs (e.g., start bit not detected, parity is wrong), the
// function should read another scan code.
unsigned char ps2_read(ps2_device_t *dev) {
    int scancode;
    while (1) {
        // loop keeps going until ringbuffer queue is not empty (until there's a valid scancode to return)
        if (!rb_empty(dev->ringbuffer)) {
            rb_dequeue(dev->ringbuffer, &scancode);
            break;
        } 
    }
    return (char)scancode;
}