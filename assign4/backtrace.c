/* File: backtrace.c
 * -----------------
 * Author: Anjali Sreenivas
 * 
 * The backtrace.c module contains a set of functions that can be used to 
 * harvest a backtrace from the call stack, serving as a potentially useful 
 * debugging tool. A backtrace is the sequence of currently active function 
 * calls.
 *
 * Citation: Julie!! :) I was perplexed as to why my backtrace wasn't working
 * and it turned out to be an oversight on my part in that I thought the addresses 
 * were growing the opposite way (so I was offsetting by +1 and +2 from my fp 
 * instead of -1 and -2). It was a magical moment when it all worked after Julie 
 * got me to realize that; thank you! :)
 */
#include "backtrace.h"
#include "printf.h"
#include "symtab.h"

// helper function implemented in file backtrace_asm.s
extern unsigned long backtrace_get_fp(void);
extern void symtab_label_for_addr(char *buf, size_t bufsize, uintptr_t addr);

// This function harvests frames from the current call stack and stores them into 
// the frame array `f` to gather a backtrace. If the backtrace extends beyond the 
// `max_frames` specified, only the `max_frames` topmost calls on the stack are stored
// in the array. The count of frames written to `f` is returned.
int backtrace_gather_frames (frame_t f[], int max_frames) {
    unsigned long* fp = (unsigned long*) backtrace_get_fp(); // get current value of frame pointer
    fp = (unsigned long*) *(fp - 2); // skip backtrace frame

    int frameCount = 0;
    while (frameCount < max_frames && fp != NULL) {
        uintptr_t ra = *(fp - 1); // scaling by 8 bytes
        frame_t frame;
        frame.resume_addr = ra;
        f[frameCount++] = frame;
        fp = (unsigned long*) *(fp - 2); // update fp for next frame
    }
    return frameCount;
}

// This function prints the backtrace in order, one line per frame, given an array 
// of frames as filled in by a call to `backtrace_gather_frames()`. 
void backtrace_print_frames (frame_t f[], int n) {
    for (int i = 0; i < n; i++) {
        char temp_buf[20]; // adequately sized buff to write frame label to
        symtab_label_for_addr(temp_buf, sizeof(temp_buf), f[i].resume_addr);
        printf("#%d %p at %s\n", i, (void*) f[i].resume_addr, temp_buf);
    }
}

void backtrace_print (void) {
    int max = 50;
    frame_t arr[max];

    int n = backtrace_gather_frames(arr, max);
    backtrace_print_frames(arr+1, n-1);   // print frames starting at this function's caller
}
