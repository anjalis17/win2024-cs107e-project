#include "uart.h"
#include "testing.h"

void main(void) {
    uart_init();
    // say_hello("CS107e");
    test_basic_block_motion();
}
