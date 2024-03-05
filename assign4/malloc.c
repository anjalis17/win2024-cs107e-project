/* File: malloc.c
 * --------------
 * Author: Anjali Sreenivas
 * 
 * The malloc.c file defines functions that can be used to dynamically
 * allocate (malloc()) and free (free()) fixed amounts of memory in 
 * the heap. The heap allocator includes redzone protection (to help the client 
 * detect memory errors) and memory reporting (to identify memory leaks)!
 *
 * Citation: I chose to split up the block headers to implement the extension, and 
 * this initially gave me trouble because unwanted padding was being embedded into my block
 * header structs automatically by the compiler. Talking to Kenny and Daniel during office 
 * hours helped me think about including the redzones in my block headers to preserve
 * the 8-byte alignment. :)
 */

#include "malloc.h"
#include "printf.h"
#include <stddef.h> // for NULL
#include "strings.h"
#include "backtrace.h"

#define frameCount 3

/*
 * Data variables private to this module used to track
 * statistics for debugging/validate heap:
 *    count_allocs, count_frees, total_bytes_requested
 */
static int count_allocs, count_frees, total_bytes_requested;
const unsigned int REDZONE_EXPECTED = 0x99999999;

// EXTENSION - Splitting block header to incorporate new block metadata and preserving 
// 8-byte alignment of payload. 
// 4-byte redzones embedded in block headers surround the payload space within each block 
// for memory error protection.
struct header_p1 {
    unsigned int payload_size;
    unsigned int redzone;
};                    // sizeof(struct header_p1) = 8 bytes


struct header_p2 {
    unsigned int redzone;
    unsigned int status;
    frame_t backtrace[3]; 
};                   // sizeof(struct header_p2) = 32 bytes

enum { IN_USE = 0, FREE = 1 };
void report_damaged_redzone (void *ptr, struct header_p2 *hdr2);

/*
 * The segment of memory available for the heap runs from &__heap_start
 * to &__heap_max (symbols from memmap.ld establish these boundaries)
 *
 * The variable cur_head_end is initialized to &__heap_start and this
 * address is adjusted upward as in-use portion of heap segment
 * enlarges. Because cur_head_end is qualified as static, this variable
 * is not stored in stack frame, instead variable is located in data segment.
 * The one variable is shared by all and retains its value between calls.
 */

// Call sbrk to enlarge in-use heap area
void *sbrk(size_t nbytes) {
    extern unsigned char __heap_start, __heap_max;  // symbols in linker script memmap.ld
    static void *cur_heap_end =  &__heap_start;     // IMPORTANT: static

    void *new_heap_end = (char *)cur_heap_end + nbytes;
    if (new_heap_end > (void *)&__heap_max)    // if request would extend beyond heap max
        return NULL;                // reject
    void *prev_heap_end = cur_heap_end;
    cur_heap_end = new_heap_end;
    return prev_heap_end;
}

// Simple macro to round up x to multiple of n.
// The efficient but tricky bitwise approach it uses
// works only if n is a power of two -- why?
#define roundup(x,n) (((x)+((n)-1))&(~((n)-1)))

// Helper function to get location of second part of block header based on `hdr1`, a pointer to 
// the first part of the block header
struct header_p2 *getHeaderPt2(struct header_p1 *hdr1) {
    struct header_p1 *hdr2_loc = hdr1 + 1 + hdr1->payload_size / sizeof(struct header_p1); 
    struct header_p2 *hdr2 = (struct header_p2*) hdr2_loc; // typecasting 
    return hdr2;
}

// Helper to get location of starting header of subsequent block based on part 2 header of current
// block `hdr2prev`
struct header_p1 *getNextBlockHd1(struct header_p2* hdr2curr) {
    struct header_p1 *hdr1_next;
    hdr1_next = (struct header_p1*) hdr2curr + (sizeof(struct header_p2) / sizeof(struct header_p1));
    return hdr1_next;
}

/* `malloc` services a dynamic allocation request when called, by returning the address
of a memory block of at least `nbytes` contiguous bytes in the heap. 
-----------------------------------------------------------------------------------------
Block headers are placed adjacent to each memory block allocated to toggle free/in use 
status when memory is later freed by the user. For each allocation request, the block headers 
are traversed starting from the bottom (start) of the heap to find the first free block that 
can satisfy the user's memory request. If no existing block fits the criteria, a block is added to 
the top of the heap if possible via the function `sbrk`. If not possible (request too big to 
fit within heap space) or if `nbytes` is 0, NULL is returned. */
void *malloc (size_t nbytes) {
    if (nbytes == 0) {
        return NULL;
    }
    extern unsigned char __heap_start;
    count_allocs++;
    total_bytes_requested += nbytes;
    nbytes = roundup(nbytes, 8);

    struct header_p1 *hdr1 = (struct header_p1*) &__heap_start; // Initialize pointer at bottom of heap
    while (hdr1 < (struct header_p1*) sbrk(0)) { // Traverse all existing blocks within heap
        // Get location of second part of header 
        struct header_p2 *hdr2 = getHeaderPt2(hdr1);
        // If we've found a viable space in heap... 
        if (hdr2->status == FREE && hdr1->payload_size >= nbytes) {
            // Capacity for payload in remainder block if we were to split
            signed int rem_payload_cap = hdr1->payload_size - (unsigned int) nbytes - (unsigned int) sizeof(struct header_p1) 
                                        - (unsigned int) sizeof(struct header_p2);

            // If there's enough excess leftover space to split off a remainder as a second block, then do so!
            // The 8 below is the min space needed for a payload (since we always round up to the next multiple of 8 bytes).
            if (rem_payload_cap >= 8) {     
                // Handle current block (IN_USE)                     
                hdr1->payload_size = nbytes; // change payload size
                memset(&hdr1->redzone, 0x99, 4);  // set up redzone #1
                
                struct header_p2 *hdr2 = getHeaderPt2(hdr1);
                memset(&hdr2->redzone, 0x99, 4);  // set up redzone #2
                hdr2->status = IN_USE;
                
                struct header_p1 *payloadLoc = hdr1 + 1; 

                // Get backtrace frames for curr block for hdr2
                frame_t arr[3];
                backtrace_gather_frames(arr, 3);
                memcpy(hdr2->backtrace, arr, sizeof(frame_t) * 3);

                // Handle remainder block (FREE)
                // Advance hdr1 to location of remainder block header pt 1
                hdr1 = getNextBlockHd1(hdr2);
                hdr1->payload_size = rem_payload_cap;
                memset(&hdr1->redzone, 0x99, 4);  // set up redzone #1 - remainder block

                hdr2 = getHeaderPt2(hdr1);
                memset(&hdr2->redzone, 0x99, 4);  // set up redzone #2 - remainder block
                hdr2->status = FREE;

                return payloadLoc; 
            }
            hdr2->status = IN_USE;
            return hdr1 + 1; // Starting address of payload (offset scaled by size of header_p1 struct)
        }
        hdr1 = getNextBlockHd1(hdr2);
    }

    // At this point, we have reached the top of our heap (sbrk(0)) after traversal through existing blocks
    hdr1 = sbrk(sizeof(struct header_p1) + nbytes + sizeof(struct header_p2));
    if (hdr1 == NULL) { // request cannot be satisfied -- happens when nbytes is too big to fit into remaining heap space
        count_allocs--; // decrement count_allocs static variable that was incremented at top of function 
        return NULL;
    }
    hdr1->payload_size = nbytes;
    memset(&hdr1->redzone, 0x99, 4);  // set up redzone #1

    struct header_p2 *hdr2 = getHeaderPt2(hdr1);
    memset(&hdr2->redzone, 0x99, 4);  // set up redzone #2
    hdr2->status = IN_USE;

    // Get backtrace frames
    frame_t arr[3];
    backtrace_gather_frames(arr, 3);
    memcpy(hdr2->backtrace, arr, sizeof(frame_t) * 3);

    return hdr1 + 1;
}

/* `free` deallocates the memory block at the address `ptr` by toggling status from 'IN_USE' to 'FREE'. 
Each time the function is called, the freed block is coalesced with all adjacent forward-located free blocks,
if present. `free` assumes `ptr` was previously returned by a call to `malloc`. If `ptr` is NULL, the operation
does nothing. */
void free (void *ptr) {
    if (ptr == NULL) { 
        return;
    }
    // get block headers
    struct header_p1 *hdr1 = (struct header_p1*) ptr - 1;  
    struct header_p2 *hdr2 = getHeaderPt2(hdr1);
    // check if redzone compromised -- memory error prevention
    if (hdr1->redzone != REDZONE_EXPECTED || hdr2->redzone != REDZONE_EXPECTED) {
        report_damaged_redzone(ptr, hdr2);
        return;
    }
    hdr2->status = FREE;
    count_frees++;

    // Go to pt 1 header of forward neighbor of block that was just freed
    struct header_p1 *new_hdr1 = getNextBlockHd1(hdr2);
    // Loop to coalesce all adjacent forward free blocks with block we freed
    while (new_hdr1 < (struct header_p1*) sbrk(0)) {
        struct header_p2 *hdr2 = getHeaderPt2(new_hdr1);
        if (hdr2->status != FREE) {
            break;
        }
        // Add all of new block space (including metadata space) to space associated with the block we freed
        hdr1->payload_size += new_hdr1->payload_size + sizeof(struct header_p1) + sizeof(struct header_p2);  
        new_hdr1 = getNextBlockHd1(hdr2);
    }
}

// This function can be used to print the current state of the heap (from bottom up),
// providing a visual dump of heap contents. 
void heap_dump (const char *label) {
    extern unsigned char __heap_start;
    printf("\n---------- HEAP DUMP (%s) ----------\n", label);
    printf("Heap segment at %p - %p\n", &__heap_start, sbrk(0));

    struct header_p1 *hdr1 = (struct header_p1*) &__heap_start; // initialize pointer at bottom of heap
    while (hdr1 < (struct header_p1*) sbrk(0)) {
        printf("---NEW BLOCK---\n");
        printf("Payload size: %d\n", hdr1->payload_size);
        printf("Payload address: %p\n", hdr1 + 1);
        struct header_p2 *hdr2 = getHeaderPt2(hdr1);

        printf("Status: %d\n", hdr2->status);
        backtrace_print_frames(hdr2->backtrace, frameCount);

        // move to next block header start
        hdr1 = getNextBlockHd1(hdr2);
    }

    printf("----------  END DUMP (%s) ----------\n", label);
    printf("Stats: %d in-use (%d allocs, %d frees), %d total bytes requested\n\n",
        count_allocs - count_frees, count_allocs, count_frees, total_bytes_requested);
}

// Reports memory leaks at end of program
void memory_report (void) {
    extern unsigned char __heap_start;
    printf("\n=============================================\n");
    printf(  "         Mini-Valgrind Memory Report         \n");
    printf(  "=============================================\n");
    printf("final stats: %d allocs, %d frees, %d total bytes requested\n\n",
        count_allocs, count_frees, total_bytes_requested);

    struct header_p1 *hdr1 = (struct header_p1*) &__heap_start; // initialize pointer at bottom of heap
    while (hdr1 < (struct header_p1*) sbrk(0)) { // traverse all existing blocks within heap
        struct header_p2 *hdr2 = getHeaderPt2(hdr1);
        // If we find a block which isn't free, report the memory leak 
        if (hdr2->status != FREE) {
            printf("%d bytes are lost, allocated by\n", hdr1->payload_size);
            backtrace_print_frames(hdr2->backtrace, frameCount);
            printf("\n");
        }
        // Move to next block 
        hdr1 = getNextBlockHd1(hdr2);
    }
}

// Reports damaged redzones; called when user tries to free block in which they've written beyond the 
// payload bounds they initially specified
void report_damaged_redzone (void *ptr, struct header_p2 *hdr2) {
    struct header_p1* hdr1 = (struct header_p1*) ptr - 1;
    printf("\n=============================================\n");
    printf(  " **********  Mini-Valgrind Alert  ********** \n");
    printf(  "=============================================\n");
    printf("Attempt to free address %p that has damaged red zone(s): [%x] [%x]\n", ptr, hdr1->redzone, hdr2->redzone);
    printf("Block of size %d bytes, allocated by\n", hdr1->payload_size);
    backtrace_print_frames(hdr2->backtrace, frameCount);
}
