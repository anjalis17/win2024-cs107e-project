#include "uart.h"
#include "testing.h"

void main(void) {
    uart_init();
    // say_hello("CS107e"); 
    // test_random_init();
    // test_basic_block_motion();
    test_motions();
}
