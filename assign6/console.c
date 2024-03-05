/* File: console.c
 * ---------------
 * Author: Anjali Sreenivas
 *
 * The console.c file provides a graphical interface to a text console for
 * display on a screen, layering on top of the gl.c and fb.c modules!
 */
#include "console.h"
#include "gl.h"
#include "printf.h"
#include "strings.h"
#include "malloc.h"

#define MAX_OUTPUT_LEN 1024
static void process_char(char ch);
static void redrawConsole();
static void track_char(char ch);

// module-level variables, you may add/change this struct as you see fit!
static struct {
    color_t bg_color, fg_color;
    int line_height;
    unsigned int cursorX;
    unsigned int cursorY;
    char* chars;
    int charIndex;
    int width;
    int height;
} module;

// Required init for usage of console module
void console_init(int nrows, int ncols, color_t foreground, color_t background) {
    const static int LINE_SPACING = 5;     // amount of space between console rows
    module.line_height = gl_get_char_height() + LINE_SPACING;
    module.width = ncols * gl_get_char_width();
    module.height = nrows * module.line_height;

    module.fg_color = foreground;
    module.bg_color = background;
    console_clear();

    // Initializing heap-allocated memory of max number of characters that can be displayed on screen
    // to track text user displays in console
    unsigned int nbytes = nrows * ncols;
    module.chars = malloc(nbytes);
    memset(module.chars, '\0', nbytes);

    gl_init(module.width, module.height, GL_DOUBLEBUFFER);
    gl_swap_buffer();
}

// Clear console and reset cursorX, cursorY positions  
void console_clear(void) {
    gl_clear(module.bg_color);
    module.cursorX = 0;
    module.cursorY = 0;
}

// Public function which user can call to print fromatted text to console; relies on vsnprintf
// Returns num of characters written to the console (signed int)
int console_printf(const char *format, ...) {
    char buf[MAX_OUTPUT_LEN];

    va_list ap; // declare va_list
    va_start(ap, format); // init va_list, read arguments following argument named format
    // Packaged variadic arguments into va_list `ap` to call vsnprintf helper
    vsnprintf(buf, MAX_OUTPUT_LEN, format, ap);
    va_end(ap);

    int origCharIndex = module.charIndex;

    for (int i = 0; i < strlen(buf); i++) {
         // Support vertical scrolling
        if ((module.cursorY + module.line_height) > module.height) {
            printf("%s\n", module.chars);
            redrawConsole();
            module.cursorX = 0;
            module.cursorY = module.height - module.line_height;
        }
        // Keep track of character user displays and handle graphical/framebuffer processing 
        track_char(buf[i]);
        process_char(buf[i]);
    }
    // Update visual display
    gl_swap_buffer();
	return module.charIndex - origCharIndex;
}

// Handles internal tracking of characters displayed to screen by user to support vertical line scrolling
static void track_char(char ch) {
    if (ch == '\b') {
        if (module.charIndex != 0) module.chars[--module.charIndex] = '\0';
    }
    if (ch == '\f') {
        int nrows = module.height / gl_get_char_height();
        int ncols = module.width / gl_get_char_width();
        unsigned int nbytes = nrows * ncols;

        memset(module.chars, '\0', nbytes);
        module.charIndex = 0;
    }
    // new line or ordinary character
    else {
        module.chars[module.charIndex++] = ch;
    }
}

// Helper function redraws the console screen during a vertical scrolling operation
static void redrawConsole() {
    // find where second line of console chars starts 
    unsigned int secondLineStartInd = module.width / gl_get_char_width();
    for (int i = 0; i < (module.width / gl_get_char_width()); i++) {
        // handle case where new line could have been displayed before first full line filled up
        if (module.chars[i] == '\n') { 
            secondLineStartInd = i + 1;
            break;
        }
    }

    // reconfigure module.chars to stored chars typed from 2nd line onwards
    int nrows = module.height / gl_get_char_height();
    int ncols = module.width / gl_get_char_width();
    unsigned int nbytes = nrows * ncols;
    char* newChars = malloc(nbytes);
    memset(newChars, '\0', nbytes);
    memcpy(newChars, module.chars + secondLineStartInd, nbytes - secondLineStartInd);
    free(module.chars);
    module.chars = newChars;
    module.charIndex -= secondLineStartInd;

    // redraw console from original line 2
    console_clear();
    for (int i = 0; i < strlen(module.chars); i++) {
        process_char(module.chars[i]);
    }
}

// Visually process and display character printed to screen by user
static void process_char(char ch) {
    // Special handling for backspace - move cursor back and clear exisiting character with space;
    // Space graphically represented by rectangle matching character width/height with fill bg color
    if (ch == '\b') {
        module.cursorX -= gl_get_char_width();
        gl_draw_rect(module.cursorX, module.cursorY, gl_get_char_width(), gl_get_char_height(), module.bg_color);
    }
    // Appropriately adjusts cursorX and cursorY for new line character
    else if (ch == '\n') {
        module.cursorX = 0;
        module.cursorY += module.line_height;
    }
    else if (ch == '\f') {
        console_clear();
    }
    else { // ordinary char
        // supports horizontal wraparound
        if (module.cursorX >= module.width) {
            module.cursorX = 0;
            module.cursorY += module.line_height;
        }
        gl_draw_char(module.cursorX, module.cursorY, ch, module.fg_color);
        module.cursorX += gl_get_char_width();
    }
}
