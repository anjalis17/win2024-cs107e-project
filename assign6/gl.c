/* File: gl.c
 * ----------
 * Author: Anjali Sreenivas
 * 
 * Citations: All resources consulted for extension have been included in 
 * README.md! :)
 *
 * The gl.c module includes a set of functions layered on top of fb.c
 * that supports drawing graphics onto an HDMI display by changing
 * pixel color patterns in a framebuffer. 
 * EXTENSION: Anti-aliased, aliased lines, anti-aliased triangles, and hardware floating point
 * are all supported!!!
 */
#include "gl.h"
#include "font.h"
#include "strings.h"
#include "printf.h"

// extern void enable_floating_point(void);

// Required init for use of gl module
void gl_init(int width, int height, gl_mode_t mode) {
    fb_init(width, height, mode);
    // enable_floating_point();
}

// Returns width (count of horizontal pixels on screen as initially configured by user)
int gl_get_width(void) {
    return fb_get_width();
}

// Returns height (count of vertical pixels on screen as initially configured by user)
int gl_get_height(void) {
    return fb_get_height();
}

// Combines char rgb values (range 0-255) into a single color_t 32-bit 
// BGRA color where B is the least significant byte and A is most significant.
color_t gl_color(unsigned char r, unsigned char g, unsigned char b) {
    color_t color = 0xFF000000;
    color |= r << 16;
    color |= g << 8;
    color |= b;
    return color;
}

// Swaps onscreen and offscreen buffers -- function is called to display changed visual
// output on screen
void gl_swap_buffer(void) {
    fb_swap_buffer();
}

// Clear all pixels in frame buffer 
void gl_clear(color_t c) {
    // Note - can't use memset for all colors due to unsigned int to int conversion!
    unsigned int per_row = fb_get_width();
    unsigned int (*im)[per_row] = fb_get_draw_buffer();

    for (int x = 0; x < fb_get_width(); x++) {
        for (int y = 0; y < fb_get_height(); y++) {
            im[y][x] = c;
        }
    }
}

// If and only if (x, y) location is within bounds of framebuffer, draw pixel by
// modifying correspondig draw framebuffer value to be color_t `c`
void gl_draw_pixel(int x, int y, color_t c) {
    unsigned int per_row = fb_get_width();
    unsigned int (*im)[per_row] = fb_get_draw_buffer();
    // if location outside the bounds of framebuffer, nothing is drawn
    if (y >= fb_get_height() || x >= fb_get_width()) return;
    im[y][x] = c;
}

// Return the color of the pixel at location (x, y); returns 0 if the
// location is outside the bounds of the framebuffer
color_t gl_read_pixel(int x, int y) {
    unsigned int per_row = fb_get_width();
    unsigned int (*im)[per_row] = fb_get_draw_buffer();
    // if location outside the bounds of framebuffer, return 0
    if (y >= fb_get_height() || x >= fb_get_width()) return 0;
    return (color_t) im[y][x];
}

// Draw filled rectangle at location (x, y) with width `w` and height `h` filled with color c.
// All pixels in the rectangle that lie within the bounds of the
// framebuffer are drawn and pixels that lie outside are clipped.
void gl_draw_rect(int x, int y, int w, int h, color_t c) {
    int x_max = (x + w) <= fb_get_width() ? (x + w) : fb_get_width(); 
    int y_max = (y + h) <= fb_get_height() ? (y + h) : fb_get_height(); 

    unsigned int per_row = fb_get_width();
    unsigned int (*im)[per_row] = fb_get_draw_buffer();

    // Pre-calculating clipped bounds and directly setting color of those pixels
    // to produce faster ouput than calling gl_draw_pixel on full bounds
    for (int loc_x = x; loc_x < x_max; loc_x++) {
        for (int loc_y = y; loc_y < y_max; loc_y++) {
            im[loc_y][loc_x] = c;
        }
    }
}

// Draw a single character at location (x, y) in color c; clips pixels outside framebuffer bounds
void gl_draw_char(int x, int y, char ch, color_t c) {
    unsigned char buf[font_get_glyph_size()];
    font_get_glyph(ch, buf, sizeof(buf));

    // glyph points to start of buf partitioned into array of size glyph width
    unsigned char (*glyph)[font_get_glyph_width()] = (unsigned char (*)[font_get_glyph_width()]) buf;

    for (int loc_y = y; loc_y < y + font_get_glyph_height(); loc_y++) {
        for (int loc_x = x; loc_x < x + font_get_glyph_width(); loc_x++) {
            if (glyph[loc_y - y][loc_x - x] == 0xff) gl_draw_pixel(loc_x, loc_y, c);
        }
    }
}

// Draw a string of characters using gl_draw_char as a helper
void gl_draw_string(int x, int y, const char* str, color_t c) {
    for (int i = 0; i < strlen(str); i++) {
        gl_draw_char(x + i * font_get_glyph_width(), y, str[i], c);
    }
}

// Font width/height getters -- font has standard width and height for all characters
int gl_get_char_height(void) {
    return font_get_glyph_width();
}

int gl_get_char_width(void) {
    return font_get_glyph_height();
}


//// EXTENSION CODE 
// ANTI-ALIASED LINE DRAWING!
// Rounds num to nearest integer 
static int check_color_validity(int color_val) {
    if (color_val > 255) return 255;
    if (color_val < 0) return 0;
    return color_val; // valid rgb color between 0 and 255
}

// Draw pixel of color `fg` by blending with existing pixel color through weighted average defined by `intensity` float
void gl_draw_pixel_with_intensity(int x, int y, color_t fg, float intensity) {
    unsigned int per_row = fb_get_width();
    unsigned int (*im)[per_row] = fb_get_draw_buffer();
    // if location outside the bounds of framebuffer, nothing is drawn
    if (y >= fb_get_height() || x >= fb_get_width()) return;

    // get rgb values of foreground color_t 
    int fg_r = (fg >> 16) & 0xFF;
    int fg_g = (fg >> 8) & 0xFF;
    int fg_b = fg & 0xFF;

    // background color_t is existing pixel color at location (x, y)
    color_t bg = gl_read_pixel(x, y);
    // get rgb values of background color_t 
    int bg_r = (bg >> 16) & 0xFF;
    int bg_g = (bg >> 8) & 0xFF;
    int bg_b = bg & 0xFF;

    // bound intensity to be between 0 and 1
    if (intensity < 0) intensity = 0;
    if (intensity > 1) intensity = 1;

    // blend foreground color_t rgb values with background rgb as a linear combination weighted by intensity
    int blended_r = check_color_validity((int)(intensity * fg_r + (1 - intensity) * bg_r));
    int blended_g = check_color_validity((int)(intensity * fg_g + (1 - intensity) * bg_g));
    int blended_b = check_color_validity((int)(intensity * fg_b + (1 - intensity) * bg_b));

    color_t blended_color = gl_color(blended_r, blended_g, blended_b);

    im[y][x] = blended_color;
}

// rounds to nearest integer
static int round(float num) {
    // addings 0.5 supports rounding up as appropriate
    num += 0.5; 
    return (int) num;
}

// e.g. if num = 3.7, returns 0.7
static float getFractionalPart(float num) {
    return (num - (int) num);
}

// e.g. if num = 3.7, returns 0.3
static float getFractionalComplement(float num) {
    return 1 - getFractionalPart(num);
}

// Helper function assumes x1 <= x2 and dx >= dy
// 'steep' boolean param indicates whether or not x and y coordinates were switched before calling this function 
// (cases where original abs(dy) > abs(dx) when gl_draw_line was called)
static void gl_draw_line_antialiased(int x1, int y1, int x2, int y2, color_t c, bool steep) {
    float dy = y2 - y1;
    float dx = x2 - x1;

    // handles vertical line div by 0 case
    float gradient = 1.0;
    if (dx != 0.0) gradient = dy / dx; 

    // Establishing integer pixel boundaries (endpoints of line) to config loop
    // STEP 1 - DRAW STARTING PIXEL -- (x1, y1) rounded to nearest ints 
    int x_start = round(x1);
    // calculated y given slope of line and location of x_start (rounded x1)
    float y_startCalc = y1 + gradient * (x_start - x1);
    // truncate to int to get top y val of pixel where line start point is located
    int y_start = (int) y_startCalc;
    // portion of pixel covered by line is proportionate to y-coordinate of line at
    // left border of pixel (x_start)
    float y_intersect = y_startCalc; 

    // fractional distance between line spot in current pixel and next pixel to the right
    // another factor determining current pixel intensity
    float xgap = getFractionalComplement(x1 + 0.5);

    // Wu's algorithm plots in pairs of two pixels with intensities proportional to fractional part 
    // and fractional complement which correspond to how much a line covers/is present within a given pixel

    // If steep is true, we flip x and y coordinates when plotting; undoes the flipping that we perform for 
    // calculation purposes when calling this helper function via gl_draw_line.  
    if (steep) {
        gl_draw_pixel_with_intensity(y_start, x_start, c, getFractionalComplement(y_intersect) * xgap);
        gl_draw_pixel_with_intensity(y_start + 1, x_start, c, getFractionalPart(y_intersect) * xgap);
    }
    else {
        gl_draw_pixel_with_intensity(x_start, y_start, c, getFractionalComplement(y_intersect) * xgap);
        gl_draw_pixel_with_intensity(x_start, y_start + 1, c, getFractionalPart(y_intersect) * xgap);
    }
    y_intersect += gradient; // move to next y coordinate with x displacement of 1

    // STEP 2 - DRAW ENDING PIXEL
    int x_end = round(x2);
    // calculated y given slope of line and location of x_start (rounded x1)
    float y_endCalc = y2 + gradient * (x_end - x2);
    // truncate to int to get top y val of pixel where line start point is located
    int y_end = (int) y_endCalc;

    // fractional distance between line spot in current pixel and next pixel to the right
    // another factor determining current pixel intensity
    xgap = getFractionalComplement(x1 + 0.5);

    if (steep) {
        gl_draw_pixel_with_intensity(y_end, x_end, c, getFractionalComplement(y_intersect) * xgap);
        gl_draw_pixel_with_intensity(y_end + 1, x_end, c, getFractionalPart(y_intersect) * xgap);
    }
    else {
        gl_draw_pixel_with_intensity(x_end, y_end, c, getFractionalComplement(y_endCalc) * xgap);
        gl_draw_pixel_with_intensity(x_end, y_end + 1, c, getFractionalPart(y_endCalc) * xgap);
    }

    // STEP 3 - PLOT MIDPOINTS now that we have the integer pixel endpoints
    // Iterate in the x direction, since when we call this function (via gl_draw_line), we've adjusted the ordering of the 
    // vertice coordinates to the configuration where dx is >= dy and x2 >= x1
    for (int x = x_start + 1; x <= x_end - 1; x++) {
        if (steep) { // flip x and y coordinates when plotting
            gl_draw_pixel_with_intensity((int) y_intersect, x, c, getFractionalComplement(y_intersect));
            gl_draw_pixel_with_intensity((int) y_intersect + 1, x, c, getFractionalPart(y_intersect));
        }
        else {
            gl_draw_pixel_with_intensity(x, (int) y_intersect, c, getFractionalComplement(y_intersect));
            gl_draw_pixel_with_intensity(x, (int) y_intersect + 1, c, getFractionalPart(y_intersect));
        }    
        y_intersect += gradient; // increment y_intersect to store next exact y coordinate when we move x by 1
    }

}

// Draws antialiased lines between two (x, y) coordinates; inspiration from Wu's algorithm!! 
void gl_draw_line(int x1, int y1, int x2, int y2, color_t c) {
    // compute absolute val of x and y displacements in line to be drawn
    float dx = x2 - x1;
    float dy = y2 - y1;
    if (dx < 0) dx = -dx; 
    if (dy < 0) dy = -dy;

    // Reorienting coordinate positions to fit helper function config
    // case - abs(slope) > 1; line is steep so calculations and looping in helper line drawing function
    // must be done relative to y coordinates, not x coordinate
    if (dy > dx) {
        // orient x coordinate positions as needed so that difference between x2 and x1 values in helper 
        // function always yield positive value
        if (y1 > y2) gl_draw_line_antialiased(y2, x2, y1, x1, c, true);
        else gl_draw_line_antialiased(y1, x1, y2, x2, c, true);
        return;
    }
    // case - abs(slope) <= 1
    if (x1 > x2) gl_draw_line_antialiased(x2, y2, x1, y1, c, false);
    else gl_draw_line_antialiased(x1, y1, x2, y2, c, false);  // "standard" case supported by helper function
}

// ALIASED LINES - Using Bresenham's Algorithm
// Helper aliased line drawing function for cases where abs value of slope of line is <= 1 (dx > dy)
static void gl_draw_line_long(int x1, int y1, int x2, int y2, color_t c) {
    int dx = x2 - x1;
    int dy = y2 - y1;
    int y_step = 1;
    if (dy < 0) {
        y_step = -1;
        dy *= -1; // dy must be positive for all calculations; sign difference handled by y_step
    }
    // difference in function value between midpoint of two candidate next points and current point
    // f(x, y) = (dy)x - (dx)y + (dx)b; point on line if f(x, y) is 0
    int diff =  2 * dy - dx;
    
    while (x1 != x2) {
        gl_draw_pixel(x1, y1, c);
        // midpoint on positive-half plane, so we choose lower point (new y)
        if (diff > 0) {
            diff += 2 * dy - 2 * dx;
            y1 += y_step;
        }
        // midpoint on negative-half plane, so we choose upper point (same y)
        else diff += 2 * dy;
        x1 += 1; 
    }
    
}  

// Helper aliased line drawing function for cases where abs value of slope of line is > 1 (dy > dx)
static void gl_draw_line_tall(int x1, int y1, int x2, int y2, color_t c) {
    int dx = x2 - x1;
    int dy = y2 - y1;
    int x_step = 1;
    if (dx < 0) {
        x_step = -1;
        dx *= -1; // dx must be positive for all calculations; sign difference handled by x_step
    }
    // difference in function value between midpoint of two candidate next points and current point
    int diff =  2 * dx - dy;
    
    while (y1 != y2) {
        gl_draw_pixel(x1, y1, c);
        // midpoint on positive-half plane, so we choose lower point (new y)
        if (diff > 0) {
            diff += 2 * dx - 2 * dy;
            x1 += x_step;
        }
        // midpoint on negative-half plane, so we choose upper point (same y)
        else diff += 2 * dx;
        y1 += 1; 
    }
    
}  

// Draws aliased lines between 2 vertices
void gl_draw_line_aliased(int x1, int y1, int x2, int y2, color_t c) {
    int dx = x2 - x1;
    int dy = y2 - y1;
    // Calculate abs value for dx, dy
    if (dx < 1) dx *= -1;
    if (dy < 1) dy *= -1;
    
    // abs(dx) > abs(dy) 
    if (dx > dy) {
        if (x2 >= x1) gl_draw_line_long(x1, y1, x2, y2, c);
        // flip drawing orientation so destination x > start x
        else gl_draw_line_long(x2, y2, x1, y1, c);
    }
    // abs(dy) > abs(dx)
    else {
        if (y2 >= y1) gl_draw_line_tall(x1, y1, x2, y2, c);
        else gl_draw_line_tall(x2, y2, x1, y1, c);
    }
}

// Helper to return min of three integer numbers passed in as params
static int min(int num1, int num2, int num3) {
    int min = num1;
    if (num2 < min) min = num2;
    if (num3 < min) min = num3;
    return min;
}

// Helper to return max of three integer numbers passed in as params
static int max(int num1, int num2, int num3) {
    int max = num1;
    if (num2 > max) max = num2;
    if (num3 > max) max = num3;
    return max;
}

// Calculate and return area of triangle with vertices (x1, y1), (x2, y2), and (x3, y3)
static float calculateTriangleArea(int x1, int y1, int x2, int y2, int x3, int y3) {
    float triangleArea = (x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2)) / 2.0;
    if (triangleArea < 0) triangleArea = -triangleArea;
    return triangleArea;
}

// Check if candidate point P(candX, candY) lies in triangle ABC where vertices are A(x1, y1), 
// B(x2, y2), and C(x3, y3); returns boolean indicating yes or no
static bool checkIfInTriangle(int x1, int y1, int x2, int y2, int x3, int y3, int candX, int candY) {
    float triangleArea = calculateTriangleArea(x1, y1, x2, y2, x3, y3);

    // Calculate triangle PBC area
    float A1 = calculateTriangleArea(candX, candY, x2, y2, x3, y3);

    // Calculate triangle PAC area 
    float A2 = calculateTriangleArea(x1, y1, candX, candY, x3, y3);

    // Calculate triangle PAB area  
    float A3 = calculateTriangleArea(x1, y1, x2, y2, candX, candY);

    // If point is in triangle, then the whole triangle ABC area should equal the sum of the three fragmented 
    // subareas within the triangle which join at point P
    return (A1 + A2 + A3 == triangleArea);
}

// Draw antialiased triangle with vertices (x1, y1), (x2, y2), and (x3, y3) filled with color `c`
void gl_draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3, color_t c) {
    // Define rectangular bounding box around triangle
    int minX = min(x1, x2, x3);
    int maxX = max(x1, x2, x3);
    int minY = min(y1, y2, y3);
    int maxY = max(y1, y2, y3);

    // Loop through all pixels in rectangular bounding box and determine if in triangle
    for (int x = minX; x <= maxX; x++) {
        for (int y = minY; y <= maxY; y++) {
            if (checkIfInTriangle(x1, y1, x2, y2, x3, y3, x, y)) {
                gl_draw_pixel(x, y, c);
            }
        }
    }
    // Draw clean antialiased line borders for filled triangle
    gl_draw_line(x1, y1, x2, y2, c);
    gl_draw_line(x1, y1, x3, y3, c);
    gl_draw_line(x2, y2, x3, y3, c);
}

