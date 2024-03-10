#include "testing.h"

#include "game_update.h"
#include "uart.h"
#include "assert.h"
#include "timer.h"
#include "printf.h"

void test_basic_block_motion(void) {
    game_update_init(20, 10);
    // falling_piece_t piece = init_falling_piece();
    drawSquare(5, 5, GL_BLACK);
    pause("test");
}

void pause(const char *message) {
    if (message) printf("\n%s\n", message);
    printf("[PAUSED] type any key in minicom/terminal to continue: ");
    int ch = uart_getchar();
    uart_putchar(ch);
    uart_putchar('\n');
}



