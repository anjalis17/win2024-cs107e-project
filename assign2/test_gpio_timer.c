/* File: test_gpio_timer.c
 * -----------------------
 * ***** TODO: add your file header comment here *****
 */
#include "gpio.h"
#include "timer.h"

// You call assert on an expression that you expect to be true. If expr
// instead evaluates to false, then assert calls abort, which stops
// your program and flashes onboard led.
#define assert(expr) if(!(expr)) abort()

// infinite loop that flashes onboard blue LED (GPIO PD18)
void abort(void) {
    volatile unsigned int *GPIO_CFG2 = (unsigned int *)0x02000098;
    volatile unsigned int *GPIO_DATA = (unsigned int *)0x020000a0;

    // Configure GPIO PD18 function to be output.
    *GPIO_CFG2 = (*GPIO_CFG2 & ~(0xf00)) | 0x100;
    while (1) { // infinite loop
        *GPIO_DATA ^= (1 << 18); // invert value
        for (volatile int delay = 0x100000; delay > 0; delay--) ; // wait
   }
}

// Test function to ensure invalid pin IDs passed through get function are properly handled
void test_invalid_pin_ids(gpio_id_t pin) {
    assert(gpio_get_function(pin) == GPIO_INVALID_REQUEST);
}

void test_gpio_set_get_function(void) {
    // Test get pin function (pin defaults to disabled)
    assert( gpio_get_function(GPIO_PC0) == GPIO_FN_DISABLED);

    // Set pin to output, confirm get returns what was set
    gpio_set_output(GPIO_PC0);
    assert( gpio_get_function(GPIO_PC0) == GPIO_FN_OUTPUT );

    // Set pin to input, confirm get returns what was set
    gpio_set_input(GPIO_PC0);
    assert( gpio_get_function(GPIO_PC0) == GPIO_FN_INPUT );
    
    // Set pin at offset config register to input; verify 
    gpio_set_input(GPIO_PB8);
    assert( gpio_get_function(GPIO_PB8) == GPIO_FN_INPUT );
    // Change state of same pin and verify re-configuration
    gpio_set_output(GPIO_PB8);
    assert( gpio_get_function(GPIO_PB8) == GPIO_FN_OUTPUT );
    
    // Verify non input/output configurations
    gpio_set_function(GPIO_PB3, GPIO_FN_ALT3);
    assert( gpio_get_function(GPIO_PB3) == GPIO_FN_ALT3);

    // Verify pin configuration independence
    gpio_set_function(GPIO_PB4, GPIO_FN_ALT5);
    // configs of nearby pins in same config register should not be lost
    assert(gpio_get_function(GPIO_PB4) == GPIO_FN_ALT5);
    assert(gpio_get_function(GPIO_PB3) == GPIO_FN_ALT3);
    assert(gpio_get_function(GPIO_PB2) == GPIO_FN_DISABLED);
    
    // testing input of invalid pins into get function
    test_invalid_pin_ids(0x013);
    test_invalid_pin_ids(0x225);
    // verify that configs of existing pins are not modified after call with invalid pin
    assert(gpio_get_function(GPIO_PB4) == GPIO_FN_ALT5);
    assert(gpio_get_function(GPIO_PB3) == GPIO_FN_ALT3);
    assert(gpio_get_function(GPIO_PB2) == GPIO_FN_DISABLED);
}

// Prior to calling this function, I wrote to the pin's config using xfel, allowing me to verify that my gpio_get_function is working.
void test_get_with_xfel(gpio_id_t pin, unsigned int function) {
    assert(gpio_get_function(pin) == function);    
}

// Function to visually verify functionality of gpio_write function - verified that connected LED lit up! :)
void visual_test(gpio_id_t pin) {
    gpio_set_output(pin);
    gpio_write(pin, 1);
    // Verified gpio_read accuracy by writing to register via xfel, 
    // and here, further verifying its consistency with gpio_write function
    assert(gpio_read(pin) == GPIO_FN_OUTPUT);
}

void test_gpio_read_write(void) {
    // set pin to output before gpio_write
    gpio_set_output(GPIO_PB4);

    // gpio_write low, confirm gpio_read reads what was written
    gpio_write(GPIO_PB4, 0);
    assert( gpio_read(GPIO_PB4) ==  0 );

   // gpio_write high, confirm gpio_read reads what was written
    gpio_write(GPIO_PB4, 1);
    assert( gpio_read(GPIO_PB4) ==  1 );

    // gpio_write low, confirm gpio_read reads what was written
    gpio_write(GPIO_PB4, 0);
    assert( gpio_read(GPIO_PB4) ==  0 );

    visual_test(GPIO_PB12);
    // Verify data config independence; i.e. other pin data not changed
    assert(gpio_read(GPIO_PB4) == 0); 
    visual_test(GPIO_PD17);
    visual_test(GPIO_PB6);

    // Verify proper handling of reading an invalid pin
    assert(gpio_read(0x013) == GPIO_INVALID_REQUEST);
    gpio_write(0x013, 1); // nothing should happen (verified no change in visual output)
}

void test_timer(void) {
    // Test timer tick count incrementing
    unsigned long start = timer_get_ticks();
    for( int i=0; i<10; i++ ) { /* Spin */ }
    unsigned long finish = timer_get_ticks();
    assert( finish > start );

    // Test timer delay
    int usecs = 100;
    start = timer_get_ticks();
    timer_delay_us(usecs);
    finish = timer_get_ticks();
    assert( finish >= start + usecs*TICKS_PER_USEC );
}

void test_breadboard(void) {
    unsigned int segment[7] = {GPIO_PD17, GPIO_PB6, GPIO_PB12, GPIO_PB11, GPIO_PB10, GPIO_PE17, GPIO_PB0};
    unsigned int digit[4] = {GPIO_PB4, GPIO_PB3, GPIO_PB2, GPIO_PC0};

    unsigned int button = GPIO_PG13;

    for (int i = 0; i < 7; i++) {  // configure segments
        gpio_set_output(segment[i]);
    }
    for (int i = 0; i < 4; i++) {  // configure digits
        gpio_set_output(digit[i]);
    }
    gpio_set_input(button); // configure button

    while (1) { // loop forever (finish via button press, see below)
        for (int i = 0; i < 4; i++) {   // iterate over digits
            gpio_write(digit[i], 1);    // turn on digit
            for (int j = 0; j < 7; j++) {   // iterate over segments
                gpio_write(segment[j], 1);  // turn on segment
                timer_delay_ms(200);
                gpio_write(segment[j], 0);  // turn off segment
                if (gpio_read(button) == 0) return;  // stop when button pressed
            }
            gpio_write(digit[i], 0);    // turn off digit
        }
    }
}

void main(void) {
    gpio_init();
    timer_init();

   // test_gpio_set_get_function();
    // See function header comment - requires xfel config for assert to pass 
    // test_get_with_xfel(GPIO_PB7, GPIO_FN_ALT2);
    // test_get_with_xfel(GPIO_PG16, GPIO_FN_ALT8);

   // test_gpio_read_write();
   // test_timer();
    test_breadboard();
}
