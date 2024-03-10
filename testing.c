#include "testing.h"

void test_basic_block_motion(void) {
    game_update_init(20, 10);
    falling_piece_t piece = init_falling_piece();
    pause("test");
}

static void pause(const char *message) {
    if (message) printf("\n%s\n", message);
    printf("[PAUSED] type any key in minicom/terminal to continue: ");
    int ch = uart_getchar();
    uart_putchar(ch);
    uart_putchar('\n');
}



