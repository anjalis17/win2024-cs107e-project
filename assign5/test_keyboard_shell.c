/* File: test_keyboard_shell.c
 * ---------------------------
 * Authors: Anjali Sreenivas & CS107E Course Staff
 * 
 * The test_keyboard_shell.c file defines a set of test functions used to evaluate the 
 * functionality of my PS/2 keyboard driver (keyboard.c) and a simple shell
 * modules (shell.c) for the Mango Pi. The file lays out tests 
 * for which the modules are tested independents and in conjunction with each other.
 */
#include "assert.h"
#include "keyboard.h"
#include "printf.h"
#include "shell.h"
#include "strings.h"
#include "malloc.h"
#include "uart.h"
#include "timer.h"

// Prototype pasted to test tokenize functionality (verified, then changed tokenize to be 
// a static function in shell.c)
int tokenize(const char *line, char *array[],  int max);

#define ESC_SCANCODE 0x76

static void test_keyboard_scancodes(void) {
    printf("\nNow reading single scancodes. Type ESC to finish this test.\n");
    while (1) {
        unsigned char scancode = keyboard_read_scancode();
        printf("[%02x]\n", scancode);
        if (scancode == ESC_SCANCODE) break;
    }
    printf("\nDone with scancode test.\n");
}

static void test_keyboard_sequences(void) {
    printf("\nNow reading scancode sequences (key actions). Type ESC to finish this test.\n");
    while (1) {
        key_action_t action = keyboard_read_sequence();
        printf("%s [%02x]\n", action.what == KEY_PRESS ? "  Press" :"Release", action.keycode);
        if (action.keycode == ESC_SCANCODE) break;
    }
    printf("Done with scancode sequences test.\n");
}

static void test_keyboard_events(void) {
    printf("\nNow reading key events. Type ESC to finish this test.\n");
    while (1) {
        key_event_t evt = keyboard_read_event();
        printf("%s PS2_key: {%c,%c} Modifiers: 0x%x\n", evt.action.what == KEY_PRESS? "  Press" : "Release", evt.key.ch, evt.key.other_ch, evt.modifiers);
        if (evt.action.keycode == ESC_SCANCODE) break;
    }
    printf("Done with key events test.\n");
}

// I did a lot of visual testing this function here to ensure my keyboard driver
// works across various cases :)
static void test_keyboard_chars(void) {
    printf("\nNow reading chars. Type ESC to finish this test.\n");
    while (1) {
        char c = keyboard_read_next();
        if (c >= '\t' && c <= 0x80)
            printf("%c", c);
        else
            printf("[%02x]", c);
        if (c == ps2_keys[ESC_SCANCODE].ch) break;
    }
    printf("\nDone with key chars test.\n");
}

static void test_keyboard_assert(void) {
    char ch;
    printf("\nHold down Shift and type 'g'\n");
    ch = keyboard_read_next();
    assert(ch == 'G');  // confirm user can follow directions and correct key char generated

    printf("\nHold down Shift and type '4'\n");
    ch = keyboard_read_next();
    assert(ch == '$');  

    // verifying caps lock sticky key functionality
    printf("\nPress Caps Lock and then type 't'\n");
    ch = keyboard_read_next();
    assert(ch == 'T'); 

    printf("\nType 'y'\n");
    ch = keyboard_read_next();
    assert(ch == 'Y'); 

    printf("\nPress Caps Lock and then type 'x'\n");
    ch = keyboard_read_next();
    assert(ch == 'x'); 
}

// Testing shell evaluate functionality and individual shell commands
static void test_shell_evaluate(void) {
    shell_init(keyboard_read_next, printf);

    printf("\nTest shell_evaluate on fixed commands.\n");
    
    //// CLEAR
    int ret = shell_evaluate("clear ...args irrelevant here");
    printf("Command result is zero if successful, is it? %d\n", ret);

    //// ECHO
    ret = shell_evaluate("echo hello, world!");
    assert(ret == 0);

    //// HELP
    ret = shell_evaluate("help");
    assert(ret == 0);

    ret = shell_evaluate("help please");
    assert(ret == 1);

    ret = shell_evaluate("help peek");
    assert(ret == 0);

    //// PEEK
    // errors should be thrown
    ret = shell_evaluate("peek 4000000anasjdf");
    assert(ret == 1);

    ret = shell_evaluate("peek fred");
    assert(ret == 1);

    ret = shell_evaluate("peek 0xff");
    assert(ret == 1);

    ret = shell_evaluate("peek ");
    assert(ret == 1);

    // check first instruction of _start()
    ret = shell_evaluate("peek 0x40000000");
    assert(ret == 0);

    // check that default values of PB registers are being read properly
    ret = shell_evaluate("peek 0x02000030"); // config reg 0
    assert(ret == 0);

    ret = shell_evaluate("peek 0x02000040"); // data reg
    assert(ret == 0);

    ret = shell_evaluate("peek 0x02000044"); // multi driving reg 0
    assert(ret == 0);
    
    //// POKE
    // errors should be thrown
    ret = shell_evaluate("poke 0x02000044 helloo"); 
    assert(ret == 1);

    ret = shell_evaluate("poke 0fjkc 0x02000044"); 
    assert(ret == 1);

    ret = shell_evaluate("poke 11 0"); 
    assert(ret == 1);

    ret = shell_evaluate("poke 0x00044"); 
    assert(ret == 1);

    // test poke functionality with valid arguments using tested peek
    ret = shell_evaluate("poke 0x40000000 0x111"); 
    assert(ret == 0);

    // verify visually that value at address has changed accordingly
    ret = shell_evaluate("peek 0x40000000"); 
    assert(ret == 0);

    ret = shell_evaluate("poke 0x2000098 255"); 
    assert(ret == 0);

    // verify visually that value at address has changed accordingly
    ret = shell_evaluate("peek 0x2000098"); 
    assert(ret == 0);

    //// REBOOT
    shell_evaluate("reboot");
}

// This is an example of a "fake" input. When asked to "read"
// next character, returns char from a fixed string, advances index
static unsigned char read_fixed(void) {
    const char *input = "echo hello, world\nhelp\n";
    static int index;

    char next = input[index];
    index = (index + 1) % strlen(input);
    return next;
}

static void test_shell_readline_fixed_input(void) {
    char buf[80];
    size_t bufsize = sizeof(buf);

    shell_init(read_fixed, printf); // input is fixed sequence of characters

    printf("\nTest shell_readline, feed chars from fixed string as input.\n");
    printf("readline> ");
    shell_readline(buf, bufsize);
    printf("readline> ");
    shell_readline(buf, bufsize);
}

static void test_shell_readline_keyboard(void) {
    char buf[80];
    size_t bufsize = sizeof(buf);

    shell_init(keyboard_read_next, printf); // input from keybaord

    printf("\nTest shell_readline, type a line of input on ps2 keyboard.\n");
    printf("? ");
    shell_readline(buf, bufsize);
}

// Test repurposed from lab 4 exercise 2 main to validate tokenize and strndup functionality
// (malloc wasn't working when we tried to test in lab :))
static void test_tokenize(void) {
    const char *str = "Leland Stanford Junior University Established 1891";
    int max = strlen(str); // number of tokens is at most length of string
    
    char *array[max];   // declare stack array to hold strings

    int ntokens = tokenize(str, array, max);
    for (int i = 0; i < ntokens; i++) {
        printf("[%d] = %s\n", i, array[i]);
        free(array[i]);
    }
}

void main(void) {
    uart_init();
    keyboard_init(KEYBOARD_CLOCK, KEYBOARD_DATA);

    printf("Testing keyboard and shell.\n");

    test_keyboard_scancodes();
    timer_delay_ms(500);

    test_keyboard_sequences();
    timer_delay_ms(500);

    test_keyboard_events();
    timer_delay_ms(500);

    test_keyboard_chars();

    test_keyboard_assert();

    test_shell_evaluate();

    test_shell_readline_fixed_input();

    test_shell_readline_keyboard();

    // test_tokenize();

    printf("Finished executing main() in test_keyboard_shell.c\n");
}
