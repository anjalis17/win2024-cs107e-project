/* File: ps2_assign5.c
 * -------------------
 * Author: Anjali Sreenivas
 * 
 * The ps2.c file defines low-level functions that can be used to read input
 * from a PS/2 device such as a keyboard or mouse.
 */
#include "gpio.h"
#include "gpio_extra.h"
#include "malloc.h"
#include "ps2.h"
#include "uart.h"

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
};

// Define constants
enum { HIGH = 1, LOW = 0};
const int SCANCODE_LEN = 11; const int DATA_LEN = 8;

// Creates a new PS2 device connected to given clock and data pins,
// The gpios are configured as input and set to use internal pull-up
// (PS/2 protocol requires clock/data to be high default)
ps2_device_t *ps2_new(gpio_id_t clock_gpio, gpio_id_t data_gpio) {
    // consider why must malloc be used to allocate device
    ps2_device_t *dev = malloc(sizeof(*dev));

    dev->clock = clock_gpio;
    gpio_set_input(dev->clock);
    gpio_set_pullup(dev->clock);

    dev->data = data_gpio;
    gpio_set_input(dev->data);
    gpio_set_pullup(dev->data);
    return dev;
}

// Helper function to read bit in data register after clock edge falls
unsigned char read_bit(ps2_device_t *dev) {\
    while (gpio_read(dev->clock) == LOW) {}

    // Wait until clock edge falls (bit val becomes LOW (0)) before reading
    while (gpio_read(dev->clock) == HIGH) { };
    return gpio_read(dev->data);
}

// Read a single PS2 scan code. Always returns a correctly received scan code:
// if an error occurs (e.g., start bit not detected, parity is wrong), the
// function should read another scan code.
unsigned char ps2_read(ps2_device_t *dev) {
    start:  
    unsigned char scancode = 0;
    unsigned int countOnes = 0;
    
    // Ensure valid start bit -- should be 0; if not, keep reading for new start bit 
    while (read_bit(dev)) {}

    // Read remaining bits (1 through 10); LSB comes first
    for (int i = 1; i <= DATA_LEN; i++) {
        unsigned char bit = read_bit(dev);
        // Processing 8 data bits (bits 1 through 8)
        if (bit == HIGH) {
            scancode |= (bit << (i - 1));
            countOnes++;
        }
    }

    // Handle parity bit (bit 9)
    if (read_bit(dev) == HIGH) countOnes++;
    // If we don't have an odd number of ones among data and parity bits, 
    // invalid scancode --> restart reading
    if (countOnes % 2 != 1) goto start;

    // Handle final stop bit (bit 10)
    // If stop bit invalid, restart reading
    if (read_bit(dev) != HIGH) goto start;
    return scancode;
}
           
    
    

