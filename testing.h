#include "game_update.h"
#include "uart.h"
#include "assert.h"
#include "timer.h"
#include "printf.h"

void test_basic_block_motion(void);
static void pause(const char *message);