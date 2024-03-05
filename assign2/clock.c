/* File: clock.c
 * -------------
 * Author: Anjali Sreenivas
 * 
 * The clock.c file contains a set of functions that can be 
 * used to (1) configure and display a countdown clock (in terms
 * of minutes and seconds) and (2) configure and display a normal
 * clock on a 7-segment LED display! Configurations for duration 
 * (countdown) and set time (clock) are handled via button presses.
 */
#include "gpio.h"
#include "timer.h"
bool debounced_reading(unsigned int button_pin, unsigned int prev_state);
static unsigned int reset_loop(unsigned int orig_val);

////// COUNTDOWN FUNCTIONS //////

// Get 7-segment light-up patterns for digits 0-F (represented as 8-bit binary numbers)
static unsigned char get_digit_pattern(int digit) {
    if (digit > 0xf) { // not a valid digit in range 0-F
        return 0;
    }

    unsigned char digit_patterns[16] = {0b00111111, 0b00000110, 0b01011011, 0b01001111, 0b01100110, 0b01101101, 
    0b01111101, 0b00000111, 0b01111111, 0b01100111, 0b01110111, 0b01111100, 0b00111001, 0b01011110, 0b01111001, 0b01110001};
    return digit_patterns[digit];
}

// Display a digit defined by an unsigned char 'digit_pattern' onto the 7-seg LED display 
// 'digit' param specifies which digit (0-3) on LED display to write to
// digit is displayed for 2500 microseconds
static void display_digit(unsigned char digit_pattern, int digit) {
    unsigned int digits[4] = {GPIO_PB4, GPIO_PB3, GPIO_PB2, GPIO_PC0}; 
    gpio_write(digits[digit], 1); // turn digit on

    unsigned int segment[7] = {GPIO_PD17, GPIO_PB6, GPIO_PB12, GPIO_PB11, GPIO_PB10, GPIO_PE17, GPIO_PB0};
    
    // Iterating to process bits 0 through 7 of 'digit_pattern'
    // 1 bit is used to represent config (on or off) of each segment A-G
    for (int i = 0; i < 7; i++) {
        unsigned char mask = 0b1 << i;
        unsigned char seg_value = (digit_pattern & mask) >> i;
        gpio_set_output(segment[i]);
        // turns on segment if corresponding bit value in 'digit_pattern' (stored in 'seg_value') is a 1
        gpio_write(segment[i], seg_value);
    }
    timer_delay_us(2500);
    gpio_write(digits[digit], 0); // turn digit off
}

// Return decimal place value (1, 10, 100, etc.) of specified 'place' in 'val'; places are counted right 
// to left (place 0 is the least significant digit and has place value 1). 
static unsigned int place_value_divisor(unsigned int val, unsigned int place) {
    unsigned int divisor = 1;
    for (int i = 0; i < place; i++) {
        divisor *= 10;
    }
    return divisor;
}

// Blinks 'val' on 7-seg LED display for 'us' microseconds
static void display_refresh_loop (unsigned int val, unsigned int us) {   
    unsigned long wait_until = timer_get_ticks() + us * TICKS_PER_USEC; 

    while (timer_get_ticks() < wait_until) {
        // Iterating over 4 digits of 7-seg LED display in quick succession and
        //  displaying the corresponding digit from 'val' (decimal representation)
        for (int i = 0; i < 4; i++) {
            unsigned int place_val = place_value_divisor(val, 3 - i);
            display_digit(get_digit_pattern((val / place_val) % 10), i);
        }
    }
}

// Function that blinks 'done' 3x on 7-seg LED display
static void conclude(void) {
    for (int i = 0; i < 3; i++) { 
    unsigned long wait_until = timer_get_ticks() + 500000 * TICKS_PER_USEC; 
        while (timer_get_ticks() < wait_until) {
            display_digit(get_digit_pattern(0xd), 0); // 'd'
            display_digit(get_digit_pattern(0), 1); // 'o'
            display_digit(0b01010100, 2); // 'n'
            display_digit(get_digit_pattern(0xe), 3); // 'e'
        }
        timer_delay_us(500000); // all LEDs off to get blinking effect
    }    
}

// Configures and displays a countdown clock where initial time is set via unsigned int time_left param. 
// Bits 0 and 1 (least significant) specify the seconds, and 3 and 4 specify the minutes. 
// e.g. To set a countdown clock for 1 min 17 seconds, call countdown(117). To set 10 min 56 seconds, call countdown(1056).
void countdown(unsigned int time_left) {
    unsigned int digits[4] = {GPIO_PB4, GPIO_PB3, GPIO_PB2, GPIO_PC0};
    for (int i = 0; i < 4; i++) {  // configure digits
        gpio_set_output(digits[i]);
    }    
    
    // configure button   
    unsigned int button = GPIO_PG13;
    gpio_set_input(button);
    
    // init display of set time; countdown starts in subsequent while loop after button press
    while (gpio_read(button) == 1) {
        display_refresh_loop(time_left, 100);
    } 

    // seconds to microseconds (us) conversion
    const unsigned int sec = 1000000;

    while (time_left != 0) {
        display_refresh_loop(time_left, sec);
        // after display of every :00 minute mark (represented as a value of __00), change to :59 of subsequent minute in countdown 
        // e.g. changing from 1000 (10:00) to 0959 (9:59) 
        if (time_left % 100 == 0) {
            time_left -= 41;
        }
        // in all other cases, simply decrement time_left by 1 (second digits change, minute digits kept constant) 
        else {
            time_left--;
        }
    }
    conclude(); // blink 'done'
}

////// CLOCK FUNCTIONS (EXTENSION) ///////

// This helper function blinks 'val' on 7-seg LED display for 'us' microseconds
// If the param 'reset_state' is false, then button presses will be monitored to enter the reset 
// loop as appropriate; if true, then button presses are ignored
unsigned int display_refresh_loop_clock(unsigned int val, unsigned int us, bool reset_state) {   
    unsigned long wait_until = timer_get_ticks() + us * TICKS_PER_USEC; 

    unsigned int hour_button = GPIO_PG12;
    unsigned int min_button = GPIO_PG13;

    while (timer_get_ticks() < wait_until) {
        // if both buttons are pressed, go into reset loop if not already in reset state
        // (after verification of stable readings via debounce helper)
        if (gpio_read(hour_button) == 0 && gpio_read(min_button) == 0 && !reset_state) {
            if (debounced_reading(hour_button, 0) && debounced_reading(min_button, 0)) {
                while(debounced_reading(hour_button, 0) || debounced_reading(min_button, 0)) {}
                val = reset_loop(val);
                // reset 1 min time for val display after val has been reset
                wait_until = timer_get_ticks() + us * TICKS_PER_USEC; 
            }
        }

        // Iterating over 4 digits of 7-seg LED display in quick succession and
        //  displaying the corresponding digit from 'val' (decimal representation)
        for (int i = 0; i < 4; i++) {
            unsigned int place_val = place_value_divisor(val, 3 - i);
            display_digit(get_digit_pattern((val / place_val) % 10), i);
        }
    }
    return val;
}

// Returns bool True or False depending on if button state after delay (debounce) reflects
// the 'changed state' (1 or 0) passed as a param
// In other words, returns whether or not 'changed_state' is a stable reading.
bool debounced_reading(unsigned int button_pin, unsigned int changed_state) {
    const int debounce_delay = 10; // ms
    
    while (1) {
        bool curr_state = gpio_read(button_pin);
        // verify that curr_state has changed 
        if (curr_state == changed_state) {
            // delay to confirm stable state
            timer_delay_ms(debounce_delay);
            curr_state = gpio_read(button_pin);
            // after delay, curr_state should still be changed
            if (curr_state == changed_state) { 
                // detected stable state (button pushed)
                return true;
            }
        }
        return false; 
    }
}

// This helper function is used to put the clock in "reset mode", where the time can be set.
// The time at which the user enters reset mode is passed in through the 'orig_val' parameter, 
// and the hours and minutes are modified via button presses from there.
static unsigned int reset_loop(unsigned int orig_val) {
    unsigned int hour_button = GPIO_PG12;
    unsigned int min_button = GPIO_PG13;

    unsigned int new_val = orig_val; 
    while (1) {
        // Blinking current time (blinking used to indicate reset mode)
        display_refresh_loop_clock(new_val, 250000, true);
        timer_delay_us(100000); // all LEDs off to get blinking effect

        bool hour_press = debounced_reading(hour_button, 0);
        bool min_press = debounced_reading(min_button, 0);

        // if stable readings that both buttons are pushed...
        if (hour_press && min_press) {
            // Wait until both button states revert back to high state
            while(debounced_reading(hour_button, 0) || debounced_reading(min_button, 0)) {}
            return new_val;
        }
        else if (hour_press) {    // only hour button pressed
            // when time is 23:XX, change to 00:XX
            if (new_val / 100 == 23) {
                new_val -= 2300; 
            }
            // otherwise increment hour by 1
            // e.g. 7:34 (734) --> 8:34 (834)
            else { 
                new_val += 100;
            }
        }
        else if (min_press) {     // only min button pressed
            // if XX:59 time, switch to next hour and reset minutes to 00
            if (new_val % 100 == 59) {
                // After 2359 (23:59), set time to 0 (representing 00:00)
                if (new_val == 2359) {
                    new_val = 0;
                }
                else { 
                    new_val += 41;
                }
            }
            // otherwise, increment minute by 1
            else {
                new_val += 1;
            }
        }
    }
    return orig_val;
}

// The 'clock' function can be used to set and display a clock (hours and minutes) on a 7-seg LED display, where 
// config is done using two buttons (the hour_button and min_button). 
// The left-most two digits on the LED display represent the hours, and the right-most two digits represent 
// the minutes. The clock is configured to represent time in military time (00:00 - 23:59).
void clock(void) {
    // CONSTANTS: conversions to microseconds (us)
    const unsigned int sec = 1000000;   // sec to us
    const unsigned int min = 60 * sec;  // min to us
    
    // configure digits as outputs
    unsigned int digits[4] = {GPIO_PB4, GPIO_PB3, GPIO_PB2, GPIO_PC0};
    for (int i = 0; i < 4; i++) {  
        gpio_set_output(digits[i]);
    }   
    
    // config buttons
    unsigned int hour_button = GPIO_PG12;
    unsigned int min_button = GPIO_PG13;
    gpio_set_input(hour_button);
    gpio_set_input(min_button);

    // Initial time config -- both buttons must be simultaneously pressed (with stable readings)
    // to prompt entering into reset loop (time setting state)
    unsigned int time;
    while (1) {
        if (gpio_read(hour_button) == 0 && gpio_read(min_button) == 0) {
            // If stable readings of button presses (0) on both buttons...
            if (debounced_reading(hour_button, 0) && debounced_reading(min_button, 0)) {
                // Wait until both button states revert back to high state (until they are both no longer stable 0s)...
                while(debounced_reading(hour_button, 0) || debounced_reading(min_button, 0)) {}
                time = reset_loop(0); // enter reset loop to set time
                break;
            }
        }
    }

    while (1) {
        // display time value via helper function for a duration of 1 min
        // time is re-configured in helper function if both buttons are simultaneously pressed
        // (triggering entry into reset loop)
        time = display_refresh_loop_clock(time, min, false);

        // if XX:59 time, switch to next hour and reset minutes to 00
        if (time % 100 == 59) {
            // After 2359 (23:59), set time to 0 (representing 00:00)
            if (time == 2359) {
                time = 0;
            }
            else {
                time += 41;
            }
        }
        // increment time by 1 in all other cases (minute digits change, hour digits kept constant) 
        else {
            time++;
        }
    }
}

void main(void) {
    // Core functionality - set countdown time here (see countdown function header)
    // countdown(314);

    clock();
}
