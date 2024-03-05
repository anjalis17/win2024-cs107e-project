/* File: fb.c
 * ----------
 * Author: Anjali Sreenivas
 * 
 * The fb.c module provide a hardware abstraction via framebuffer routines for 
 * displaying to the screen, forming the basis on which a graphics library (gl.c) can be
 * written. 
 */
#include "fb.h"
#include "de.h"
#include "hdmi.h"
#include "malloc.h"
#include "strings.h"

// module-level variables, you may add/change this struct as you see fit
static struct {
    int width;             // count of horizontal pixels
    int height;            // count of vertical pixels
    int depth;             // num bytes per pixel
    void *onScreenFramebuffer;      // address of active onScreen framebuffer memory 
    void *offScreenFramebuffer;     // address of offScreen framebuffer memory (used only in FB_DOUBLEBUFFER mode)
    fb_mode_t mode;                 // framebuffer mode (single or double buffer) 
} module;

// Required init for use of fb module
void fb_init(int width, int height, fb_mode_t mode) {
    // supports reinitialization by freeing previously deallocated memory
    if (module.onScreenFramebuffer != NULL) free(module.onScreenFramebuffer);
    if (module.offScreenFramebuffer != NULL) free(module.offScreenFramebuffer);

    module.mode = mode;
    module.width = width;
    module.height = height;
    module.depth = 4;
    int nbytes = module.width * module.height * module.depth;

    // if double buffer mode, config a second offscreen framebuffer
    if (module.mode == FB_DOUBLEBUFFER) {
        module.offScreenFramebuffer = malloc(nbytes);
        memset(module.offScreenFramebuffer, 0x0, nbytes);
    }
    module.onScreenFramebuffer = malloc(nbytes);
    memset(module.onScreenFramebuffer, 0x0, nbytes);

    hdmi_resolution_id_t id = hdmi_best_match(width, height);
    hdmi_init(id);
    de_init(width, height, hdmi_get_screen_width(), hdmi_get_screen_height());
    de_set_active_framebuffer(module.onScreenFramebuffer);
}

// Getters
int fb_get_width(void) {
    return module.width;
}

int fb_get_height(void) {
    return module.height;
}

int fb_get_depth(void) {
    return module.depth;
}

// Returns pointer to start of drawing buffer. If mode is double buffer, this is the off screen buffer; 
// otherwise, returns start of onscreen buffer.
void* fb_get_draw_buffer(void){
    if (module.mode == FB_DOUBLEBUFFER) return module.offScreenFramebuffer;
    return module.onScreenFramebuffer;
}

// "Swaps" on screen and off screen frame buffers by copying the contents of the onscreen frame buffer to the
// off screen one (while preserving the contents of offscreen buffer), and activating the newly configured
// on screen buffer for visual display.
// If single buffer mode, function does nothing.
void fb_swap_buffer(void) {
    if (module.mode == FB_DOUBLEBUFFER) {
        int nbytes = module.width * module.height * module.depth;
        memcpy(module.onScreenFramebuffer, module.offScreenFramebuffer, nbytes);

        de_set_active_framebuffer(module.onScreenFramebuffer);
    }
}
