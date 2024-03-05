/* File: test_gl_console.c
 * -----------------------
 * Author: Anjali Sreenivas
 * 
 * The test_gl_console.c file consists of a myriad of tests used to 
 * test the fb, gl, and console libraries independently and all together!
 */
#include "assert.h"
#include "console.h"
#include "fb.h"
#include "gl.h"
#include "printf.h"
#include "strings.h"
#include "timer.h"
#include "uart.h"

// implemented in file timer_asm.s
extern unsigned long timer_get_ticks(void);

// void gl_draw_line_antialiased(int x1, int y1, int x2, int y2, color_t fg, color_t bg);
void gl_draw_line_aliased(int x1, int y1, int x2, int y2, color_t c);

static void pause(const char *message) {
    if (message) printf("\n%s\n", message);
    printf("[PAUSED] type any key in minicom/terminal to continue: ");
    int ch = uart_getchar();
    uart_putchar(ch);
    uart_putchar('\n');
}


static void test_fb(void) {
    const int SIZE = 500;
    fb_init(SIZE, SIZE, FB_SINGLEBUFFER); // init single buffer

    assert(fb_get_width() == SIZE);
    assert(fb_get_height() == SIZE);
    assert(fb_get_depth() == 4);

    unsigned char *cptr = fb_get_draw_buffer();
    assert(cptr != NULL);
    int nbytes = fb_get_width() * fb_get_height() * fb_get_depth();
    memset(cptr, 0x99, nbytes); // fill entire framebuffer with light gray pixels
    pause("Now displaying 500 x 500 screen of light gray pixels");

    fb_init(1280, 720, FB_DOUBLEBUFFER); // init double buffer
    cptr = fb_get_draw_buffer();
    nbytes =  fb_get_width() * fb_get_height() * fb_get_depth();
    memset(cptr, 0xff, nbytes); // fill one buffer with white pixels
    fb_swap_buffer();
    pause("Now displaying 1280 x 720 white pixels");

    cptr = fb_get_draw_buffer();
    memset(cptr, 0x33, nbytes); // fill other buffer with dark gray pixels
    fb_swap_buffer();
    pause("Now displaying 1280 x 720 dark gray pixels");

    for (int i = 0; i < 5; i++) {
        fb_swap_buffer();
        timer_delay_ms(250);
    }
}

static void test_gl(void) {
    const int WIDTH = 800;
    const int HEIGHT = 600;

    gl_init(WIDTH, HEIGHT, GL_SINGLEBUFFER);

    // Double buffer mode, also tested single buffer
    gl_init(WIDTH, HEIGHT, GL_DOUBLEBUFFER);
    assert(gl_get_height() == HEIGHT);
    assert(gl_get_width() == WIDTH);

    // Background is purple
    gl_clear(gl_color(0x55, 0, 0x55)); // create purple color

    // Draw green pixel in lower right
    gl_draw_pixel(WIDTH-10, HEIGHT-10, GL_GREEN);
    assert(gl_read_pixel(WIDTH-10, HEIGHT-10) == GL_GREEN);

    // Blue rectangle in center of screen
    gl_draw_rect(WIDTH/2 - 100, HEIGHT/2 - 50, 200, 100, GL_BLUE);

    // Single amber character
    gl_draw_char(60, 10, 'A', GL_AMBER);

    // Show buffer with drawn contents
    gl_swap_buffer();
    pause("Now displaying 1280 x 720, purple bg, single green pixel, blue center rect, amber letter A");
}

static void more_tests_gl(void) {
    const int WIDTH = 800;
    const int HEIGHT = 600;

    gl_init(WIDTH, HEIGHT, GL_DOUBLEBUFFER);

    gl_clear(GL_CYAN); // create purple color

    gl_draw_string(50, 50, "hellooo i'm drawing graphics!", GL_INDIGO);
    gl_draw_string(50, 125, "$p3c1@L cHarActers?!! =.,/=~", GL_PURPLE);

    // test clipping rect -- can only draw 400 pixels of width
    gl_draw_rect(WIDTH/2, HEIGHT/2, 450, 100, GL_GREEN);
    
    // NOTE: When initially testing the below function calls, I temporarily removed the return 0 if pixel not in valid bounds 
    // in gl_read_pixel to verify clipping functionality using sequence of asserts 

    // Also did a visual confirmation!
    // assert(gl_read_pixel(WIDTH/2 + 399, HEIGHT/2) == GL_GREEN);
    // assert(gl_read_pixel(WIDTH/2 + 400, HEIGHT/2) == GL_CYAN); // unchanged
    // assert(gl_read_pixel(WIDTH/2 + 450, HEIGHT/2) == GL_CYAN); // unchanged

    // test clipping string -- visually verified!
    gl_draw_string(650, 50, "hellooo i'm drawing graphics!", GL_INDIGO);     

    // attempt to draw off screen, no visual change
    gl_draw_rect(WIDTH, 50, 450, 100, GL_PURPLE);
    // assert(gl_read_pixel(WIDTH, 50) == GL_CYAN); // unchanged

    gl_draw_string(WIDTH + 50, 50, "this should not be seen", GL_INDIGO);    
    // assert(gl_read_pixel(WIDTH, 50) == GL_CYAN); // unchanged 

    // test clear after chars have been drawn -- background should be purple
    gl_clear(GL_PURPLE); // create purple color

    // CONGRATS BANNER!
    gl_draw_rect(WIDTH/2 - 175, HEIGHT/2 - 100, 350, 200, GL_AMBER);
    gl_draw_string(WIDTH/2 - 135, HEIGHT/2, "CONGRATULATIONS! :)", GL_INDIGO);

    gl_swap_buffer();
    pause("test");

}

static void test_console(void) {
    console_init(25, 50, GL_CYAN, GL_INDIGO);
    pause("Now displaying console: 25 rows x 50 columns, bg indigo, fg cyan");

    // Line 1: Hello, world!
    console_printf("Hello, world!\n");
    pause("Now displaying console: hello world");
    // console_printf("Hello, world!\n");

    // Add line 2: Happiness == CODING
    console_printf("Happiness");
    console_printf(" == ");
    console_printf("CODING\n");

    // // Add 2 blank lines and line 5: I am Pi, hear me roar!
    console_printf("\n\nI am Pi, hear me v\b \broar!\n"); // typo, backspace, correction
    pause("Console printfs");

    // // Clear all lines
    console_printf("\f");

    // // Line 1: "Goodbye"
    console_printf("Goodbye!\n");
    pause("Console clear");
}

// Visually checked updating of displays via pauses
static void more_test_console(void) {
    console_init(25, 50, GL_YELLOW, GL_BLACK);
    pause("Now displaying console: 25 rows x 50 columns, bg black, fg yellow");
    console_printf("Hello, world!! <3\nI love Schiff House :)\n");
    console_printf("\nthis line is WaAAAAAAAAAAAAAAAAAAaaaaaAAAAAAA@@@@YYYYYYYYYY longer than a single line - does it wrap around?!!!! :)\n");
    pause("Check state before formfeed");
    console_printf("\fIs proper printf functionality ~preserved~ %c If so, YAY TIMES %d\n", '?', 100);
    console_printf("More type \b\bo check\n");
    console_printf("CS 107E is fun and challeneing and rewarding... this assignment feels magical <3");
    console_printf("\n\n\n\n\n\nthis line is WaAAAAAAAAAAAAAAAAAAaaaaaAAAAAAA@@@@YYYYYYY %d\b long!@#*!(#!)", 2);
    pause("check display");
}

// Followed up with a LOT of visual testing using `make run` via layering of console.c on top of shell.c, and this was how I tested and debugged my
// vertical scrolling :)

/// EXTENSION TESTS 
static void draw_line_tests_aliased(void) {
    const int WIDTH = 800;
    const int HEIGHT = 600;

    gl_init(WIDTH, HEIGHT, GL_DOUBLEBUFFER);

    gl_clear(GL_WHITE); // create purple color
    //// abs(dx) > abs(dy)
    // case 1a - x increasing, y increasing
    gl_draw_line_aliased(0,0, 100, 50, GL_BLACK);
    gl_draw_line_aliased(0,0, 400, 50, GL_BLUE);

    // case 2a - horizontal line
    gl_draw_line_aliased(300, 500, 600, 500, GL_BLUE);

    // case 3a - x increasing, y decreasing
    gl_draw_line_aliased(300, 500, 600, 250, GL_GREEN);

    // case 4 - x decreasing, y increasing
    gl_draw_line_aliased(300, 0, 100, 50, GL_AMBER);

    /// abs(dy) > abs(dx)
    // test clipping
    gl_draw_line_aliased(400, 400, 600, 800, GL_BLACK);
    
    // case 1b - x increasing, y increasing
    gl_draw_line_aliased(400, 500, 600, 100, GL_YELLOW);

    gl_swap_buffer();
    pause("does it work?");
}

static void draw_line_antialias_tests(void) {
    const int WIDTH = 800;
    const int HEIGHT = 600;

    gl_init(WIDTH, HEIGHT, GL_DOUBLEBUFFER);

    gl_clear(GL_YELLOW); 

    //// PART A - abs(dx) >= abs(dy)
    // case 1 - x increasing, y increasing
    gl_draw_line(0, 0, 100, 50, GL_BLACK);
    gl_draw_line(0, 0, 400, 50, GL_BLUE);
    // visually check if antialias line drawings work on top of other drawings
    gl_draw_rect(WIDTH/2 - 175, HEIGHT/2 - 100, 350, 200, GL_INDIGO);
    gl_draw_line(0, 0, 500, 400, GL_MAGENTA);
    gl_draw_line(0, 0, 600, 500, GL_GREEN);
    gl_swap_buffer();
    pause("case 1");

    // // case 2 - x increasing, y decreasing
    gl_clear(GL_WHITE);
    gl_draw_line(0, 100, 100, 0, GL_CYAN);
    gl_draw_line(0, 350, 400, 50, GL_GREEN);
    gl_swap_buffer();
    pause("case 2");
    
    // case 3 - x decreasing, y decreasing
    gl_clear(GL_BLACK);
    gl_draw_line(340, 100, 100, 0, GL_CYAN);
    gl_draw_line(400, 350, 100, 50, GL_YELLOW);
    gl_swap_buffer();
    pause("case 3");

    // case 4 - x decreasing, y increasing
    gl_draw_line(500, 100, 100, 400, GL_GREEN);
    gl_draw_line(350, 150, 100, 260, GL_PURPLE);
    gl_swap_buffer();
    pause("case 4");

    //// PART B - abs(dy) >= abs(dx) -- line is steep
    // case 1 - x increasing, y increasing
    gl_draw_line(0, 0, 50, 100, GL_BLACK);
    gl_draw_line(0, 0, 50, 450, GL_BLUE);
    // visually check if antialias line drawings work on top of other drawings
    gl_draw_rect(WIDTH/2 - 175, HEIGHT/2 - 100, 350, 200, GL_INDIGO);
    gl_draw_line(0, 0, 400, 500, GL_MAGENTA);
    gl_draw_line(50, 0, 400, 500, GL_GREEN);
    gl_swap_buffer();
    pause("case 1");

    // case 2 - x increasing, y decreasing
    gl_clear(GL_BLACK);
    gl_draw_line(0, 400, 100, 0, GL_CYAN);
    gl_draw_line(10, 350, 100, 50, GL_GREEN);
    gl_swap_buffer();
    pause("case 2");
    
    // // case 3 - x decreasing, y decreasing
    // gl_clear(GL_BLACK);
    gl_draw_line(340, 500, 100, 0, GL_CYAN);
    gl_draw_line(700, 450, 400, 50, GL_YELLOW);
    gl_swap_buffer();
    pause("case 3");


    // case 4 - x decreasing, y increasing
    gl_clear(GL_PURPLE);
    gl_draw_line(300, 100, 100, 400, GL_GREEN);
    gl_draw_line(350, 150, 100, 560, GL_BLUE);
    gl_swap_buffer();
    pause("case 4");
}

static void test_triangle(void) {
    unsigned long timer_curr_ticks = timer_get_ticks();

    const int WIDTH = 800;
    const int HEIGHT = 600;

    // tested with both single and double buffer
    gl_init(WIDTH, HEIGHT, GL_SINGLEBUFFER);

    gl_clear(GL_INDIGO); 
    gl_draw_triangle(0, 0, 100, 250, 350, 400, GL_AMBER);
    gl_swap_buffer();
    // pause("amber triangle?");

    gl_draw_triangle(415, 320, 200, 250, 600, 500, GL_CYAN);
    gl_swap_buffer();
    // pause("cyan triangle?!");

    gl_draw_triangle(350, 400, 600, 400, 525, 10, GL_PURPLE);
    gl_swap_buffer();
    // pause("purple triangle?! YAY");

    gl_draw_triangle(700, 200, 310, 300, 0, 780, GL_WHITE);
    gl_swap_buffer();

    gl_draw_triangle(0, 600, 800, 0, 650, 520, GL_MAGENTA);
    gl_swap_buffer();
    // pause("clipped white triangle?! YAY");
    printf("number of microseconds: %ld", ((timer_get_ticks() - timer_curr_ticks) / (TICKS_PER_USEC)));
    pause("done");
}

void main(void) {
    timer_init();
    uart_init();
    printf("Executing main() in test_gl_console.c\n");

    // test_fb();
    // test_gl();
    // more_tests_gl();
    // test_console();
    // more_test_console();
    // draw_line_tests_aliased();
    // draw_line_antialias_tests();
    test_triangle();

    printf("Completed main() in test_gl_console.c\n");
}
