#include "testing.h"

#include "game_update.h"
#include "uart.h"
#include "assert.h"
#include "timer.h"
#include "printf.h"

void pause(const char *message) {
    if (message) printf("\n%s\n", message);
    printf("[PAUSED] type any key in minicom/terminal to continue: ");
    int ch = uart_getchar();
    uart_putchar(ch);
    uart_putchar('\n');
}


void test_random_init(void) {
    for (int i = 0; i < 10; i++) {
        timer_init();
        game_update_init(20, 10);
        falling_piece_t piece = init_falling_piece();
        pause("start");
    }
}

void test_basic_block_motion(void) {
    timer_init();
    game_update_init(20, 10);
    falling_piece_t piece = init_falling_piece();
    while (!piece.fallen) {
        pause("key press to move down");
        move_down(&piece);
    }
    pause("fallen");
}


