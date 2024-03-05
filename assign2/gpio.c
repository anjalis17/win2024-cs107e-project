/* File: gpio.c
 * ------------
 * Author: Anjali Sreenivas 
 * 
 * The gpio.c module consists of a set of functions that can be used to 
 * set and get the 4-bit configs of any GPIO pin, and write and read 
 * the state of any GPIO pin (1 representing high; 0 representing low).
 *
 * Citations: After struggling to find my error across several initial drafts of my gpio.c set and get
 * functions, I discussed my high-level approach with Aditi. I was originally trying to store the exact 
 * address of the config register I wanted to access in a pointer variable, but since it wasn't 
 * working as I intended, she suggested I try the approach of simply offseting from the base
 * address as I was dereferencing. 
 * Additionally, I used Daniel's gpio_fel.py script to test and debug my gpio.c module! :)
 */

#include "gpio.h"
#include <stddef.h>
 
enum { GROUP_B = 0, GROUP_C, GROUP_D, GROUP_E, GROUP_F, GROUP_G };

typedef struct  {
    unsigned int group;
    unsigned int pin_index;
} gpio_pin_t;

// The gpio_id_t enumeration assigns a symbolic constant for each
// in such a way to use a single hex constant. The more significant
// hex digit identifies the group and lower 2 hex digits are pin index:
//       constant 0xNnn  N = which group,  nn = pin index within group
//
// This helper function extracts the group and pin index from a gpio_id_t
// e.g. GPIO_PB4 belongs to GROUP_B and has pin_index 4
static gpio_pin_t get_group_and_index(gpio_id_t gpio) {
    gpio_pin_t gp;
    gp.group = gpio >> 8;
    gp.pin_index = gpio & 0xff; // lower 2 hex digits
    return gp;
}

// The gpio groups are differently sized, e.g. B has 13 pins, C only 8.
// This helper function confirms that a gpio_id_t is valid (group
// and pin index are valid)
bool gpio_id_is_valid(gpio_id_t pin) {
    gpio_pin_t gp = get_group_and_index(pin);
    switch (gp.group) {
        case GROUP_B: return (gp.pin_index <= GPIO_PB_LAST_INDEX);
        case GROUP_C: return (gp.pin_index <= GPIO_PC_LAST_INDEX);
        case GROUP_D: return (gp.pin_index <= GPIO_PD_LAST_INDEX);
        case GROUP_E: return (gp.pin_index <= GPIO_PE_LAST_INDEX);
        case GROUP_F: return (gp.pin_index <= GPIO_PF_LAST_INDEX);
        case GROUP_G: return (gp.pin_index <= GPIO_PG_LAST_INDEX);
        default:      return false;
    }
}

// This helper function is suggested to return the address of
// the config0 register for a gpio group, i.e. get_cfg0_reg(GROUP_B)
// Refer to the D1 user manual to learn the address the config0 register
// for each group. Be sure to note how the address of the config1 and
// config2 register can be computed as relative offset from config0.
static volatile unsigned int *get_cfg0_reg(unsigned int group) {
    switch (group) {
	case GROUP_B: return (unsigned int*) 0x02000030;
	case GROUP_C: return (unsigned int*) 0x02000060;
	case GROUP_D: return (unsigned int*) 0x02000090;
	case GROUP_E: return (unsigned int*) 0x020000C0;
	case GROUP_F: return (unsigned int*) 0x020000F0;
	case GROUP_G: return (unsigned int*) 0x02000120;
	default:      return NULL;
    }
}

// This helper function is suggested to return the address of
// the data register for a gpio group. Refer to the D1 user manual
// to learn the address of the data register for each group.
static volatile unsigned int *get_data_reg(unsigned int group) {
    switch (group) {
	case GROUP_B: return (unsigned int*) 0x02000040;
	case GROUP_C: return (unsigned int*) 0x02000070;
	case GROUP_D: return (unsigned int*) 0x020000A0;
	case GROUP_E: return (unsigned int*) 0x020000D0;
	case GROUP_F: return (unsigned int*) 0x02000100;
	case GROUP_G: return (unsigned int*) 0x02000130;
	default:      return NULL;
    }
}

void gpio_init(void) {
    // no initialization required for this peripheral
}

void gpio_set_input(gpio_id_t pin) {
    gpio_set_function(pin, GPIO_FN_INPUT);
}

void gpio_set_output(gpio_id_t pin) {
    gpio_set_function(pin, GPIO_FN_OUTPUT);
}

// This function sets the config of a GPIO pin 'pin' to the 4 bit config inputted with the 'function' param. 
// If 'pin' or 'function' is not valid, no changes are made.
void gpio_set_function(gpio_id_t pin, unsigned int function) {
    if (!gpio_id_is_valid(pin) || function > 0xf) { 
        return;
    }
    gpio_pin_t pin_t =  get_group_and_index(pin);   
    // Step 1 - clear out relevant 4 bits at pin config address with bit mask 
    // pin_index % 8 accounts for index displacement of pin config location relative to
    // index 0 of its config register, which can store up to 8 pins' configs 
    unsigned int mask = ~(0xf << (4 * (pin_t.pin_index % 8)));
    // offset by (pin_index / 8) scales by factor of 4 bytes (size of int and 1 config register)
    *(get_cfg0_reg(pin_t.group) + (pin_t.pin_index / 8)) &= mask;
   
    // Step 2 - update cleared out 4 bits with function 
    *(get_cfg0_reg(pin_t.group) + (pin_t.pin_index / 8)) |= (function << (4 * (pin_t.pin_index % 8))); 
}

// Function gets 4 bit config value of GPIO pin passed via parameter 'pin'
unsigned int gpio_get_function(gpio_id_t pin) {
    if (!gpio_id_is_valid(pin)) { 
        return GPIO_INVALID_REQUEST;
    }
    gpio_pin_t pin_t =  get_group_and_index(pin);   
 
    // dereference config address for pin to get current 32-bit value at register;
    // config address is (base config address for pin group + (index / 8))
    // since each config register can hold config data for up to 8 pins before 
    // we need to offset (move up by factor of 4 bytes)
    unsigned int value = *(get_cfg0_reg(pin_t.group) + (pin_t.pin_index / 8));
    // bit mask - left-shifting 0xf = 0b1111 to the location of the relevant 4 bits
    unsigned int mask = 0xf << (4 * (pin_t.pin_index % 8));
    // return value & mask right-shifted all the way down--gives us the
    // relevant 4 bits determining the config of pin
    return (value & mask) >> (4 * (pin_t.pin_index % 8));
}

// This function sets the value of a GPIO pin 'pin' to 1 (high) or 0 (low) 
// specified by the int 'value' param. 
// If 'pin' isn't valid, no changes are made.
void gpio_write(gpio_id_t pin, int value) {
    if (!gpio_id_is_valid(pin)) { 
        return;
    }
    gpio_pin_t pin_t =  get_group_and_index(pin);   
    // step 1 - clear out relevant bit at pin config address with bit mask
    unsigned int mask = ~(0x1 << (pin_t.pin_index));
    *(get_data_reg(pin_t.group)) &= mask;
   
    // step 2 - update cleared out bit with value 
    *(get_data_reg(pin_t.group)) |= (value << (pin_t.pin_index)); 
}

// This function reads the value for GPIO pin 'pin' and returns 1 (high) or 0 (low). 
// If 'pin' isn't valid, GPIO_INVALID_REQUEST is returned.
int gpio_read(gpio_id_t pin) {
    if (!gpio_id_is_valid(pin)) { 
        return GPIO_INVALID_REQUEST;
    }
    gpio_pin_t pin_t =  get_group_and_index(pin);    
    // dereference data address for pin to get current 32-bit value at register
    unsigned int value = *(get_data_reg(pin_t.group));
    // bit mask - shifting 0x1 to the location of the relevant bit
    unsigned int mask = 0x1 << (pin_t.pin_index);
    // return value & mask right-shifted all the way down
    return ((value & mask) >> (pin_t.pin_index));
}
