#ifndef BACKTRACE_H
#define BACKTRACE_H

/*
 * Functions for harvesting a debugging backtrace from the call stack.
 *
 * Students implement this module in assignment 4.
 *
 * Author: Julie Zelenski <zelenski@cs.stanford.edu>
 */

#include <stdint.h>

/*
 * Type: `frame_t`
 *
 * This struct stores the information for a function that has a frame on the
 * call stack. The `resume_addr` is taken from saved ra in the callee frame.
 * The return address in the caller's sequence of instructions where control
 * will resume after the callee returns. The type `uintptr_t` is an
 * unsigned int/long of the appropriate bitwidth to store an address. The
 * `uintptr_t` type is used for an address that you intend to treat numerically.
 */
typedef struct {
    uintptr_t resume_addr;
} frame_t;

/*
 * `backtrace_gather_frames`
 *
 * Gathers a backtrace by harvesting frames from the current call stack and
 * storing into the frame array `f`.
 *
 * A backtrace is the sequence of currently active function calls. Each element
 * in the array `f` is of type `frame_t` (struct described above). Each struct
 * represents a caller who has a frame on the stack.
 *
 * The `max_frames` argument specifies the maximum number of frames to harvest.
 * If the backtrace extends more than `max_frames`, only the `max_frames`
 * topmost calls are stored into the array.
 *
 * The function returns the count of frames written to `f`.
 *
 * @param f            array in which to write stack frames
 * @param max_frames   maximum number of frames that can be stored in array
 * @return             count of frames written to array
 */
int backtrace_gather_frames(frame_t f[], int max_frames);

/*
 * `backtrace_print_frames`
 *
 * Given an array of frames as filled in by a call to backtrace_gather_frames(),
 * this function prints the backtrace,  one line per frame, using the format:
 *
 *      #1  0x40000124
 *      #2  0x400001d8
 *      #3  0x4000004c
 *      #4  0x40000010
 *
 * If symbols are available, each frame is augmented with its label.
 * The label is the name of the function and offset for resume_addr.
 * The offset is the number of bytes from function start to resume_addr.
 * This offset represents how far control has advanced into the caller
 * function before it invoked the callee. The label information is added
 * to the end of the line using this format:
 *
 *     #2 0x400001d8 at main+28
 *
 * @param f     array of stack frames
 * @param n     number of frames in array
 */
void backtrace_print_frames(frame_t f[], int n);

/*
 * `backtrace_print`
 *
 * Convenience function that calls `backtrace_gather_frames` and
 * `backtrace_print_frames` to display current call stack.
 */
void backtrace_print(void);

#endif
