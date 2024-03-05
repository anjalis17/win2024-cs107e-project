/* File: test_backtrace_malloc.c
 * -----------------------------
 * Author: Anjali Sreenivas
 * 
 * The test_backtrace_malloc.c contains a set of functions used to 
 * test the malloc.c (dynamic memory allocation/deallocation) and 
 * backtrace.c modules.
 */
#include "assert.h"
#include "backtrace.h"
#include "malloc.h"
#include "printf.h"
#include <stdint.h>
#include "strings.h"
#include "uart.h"

void heap_dump(const char *label); // available in malloc.c but not public interface
void memory_report (void); // also available in malloc.c 

static void function_A(int nframes);
static void function_B(int nframes);

static void check_backtrace(int nframes) {
    frame_t f[nframes];
    int frames_filled = backtrace_gather_frames(f, nframes);

    assert(frames_filled == nframes);
    // check_backtrace is only ever called from function_A, confirm resume addr is within function_A
    assert(f[0].resume_addr > (uintptr_t)function_A && f[0].resume_addr < (uintptr_t)function_B);
    printf("Backtrace containing %d frame(s):\n", frames_filled);
    backtrace_print_frames(f, frames_filled);
    printf("\n");
}

static void function_A(int nframes) {
    check_backtrace(nframes);
}

static void function_B(int nframes) {
    function_A(nframes);
}

static int recursion(int n) {
    printf("\nEnter call recursion(%d):\n", n);
    backtrace_print();
    if (n == 0) {
        return 0;
    } else if (n % 2 == 0) {  // even
        return 2 * recursion(n-1);
    } else {                   // odd
        return 1 + recursion(n-1);
    }
}

static void test_backtrace(void) {
    function_B(1);  // grab only topmost frame
    function_B(6);  // now grab several
    recursion(4);
}

static void test_recycle(void) {
    heap_dump("Empty heap");

    char *q = malloc(64);
    memset(q, 12, 64);
    heap_dump("After q = malloc(64)");

    int *p = malloc(sizeof(int));
    *p = 1234;
    heap_dump("After p = malloc(4)");

    char *r = malloc(12);
    memcpy(r, "hellloooooooooo", 12);
    heap_dump("After r = malloc(12)");

    free(p);
    heap_dump("After free(p)");

    // Should be placed in same block as original p 
    int *s = malloc(sizeof(int));
    *s = 5678;
    heap_dump("After p = malloc(4)");

    // Should be placed at top of heap
    int *t = malloc(sizeof(int));
    *t = 2351;
    heap_dump("After t = malloc(4)");

    free(q);
    heap_dump("After free(q)");

    // Check if remainder block created (orig q partitioned into two)
    int *u = malloc(sizeof(int));
    *u = 12341;
    heap_dump("After u = malloc(4)");
}

static void test_coalesce(void) {
    heap_dump("Empty heap");

    int *p = malloc(sizeof(int));
    *p = 0;

    char *q = malloc(16);
    memcpy(q, "aaaaaaaaaaaaaaa", 16);

    long *r = malloc(sizeof(long));
    *r = 139182309;

    heap_dump("After requesting several small blocks - all blocks in use");

    free(r);
    heap_dump("After free(r)");

    // Expect q and r to coalesce
    free(q);
    heap_dump("After free(q)");

    // p, q, r should all coalesce into one free block
    free(p);
    heap_dump("After free(p)");
}

static void test_coalesce2(void) {
    heap_dump("Empty heap");

    int *p = malloc(sizeof(int));
    *p = 0;

    char *q = malloc(16);
    memcpy(q, "aaaaaaaaaaaaaaa", 16);

    long *r = malloc(sizeof(long));
    *r = 139182309;

    heap_dump("After requesting several small blocks (p, q, r) - all in use");

    free(q);
    heap_dump("After free(q)");

    // no coalescing should happen
    free(r);
    heap_dump("After free(r)");

    // p, q, r should all coalesce into one free block
    free(p);
    heap_dump("After free(p)");
}

static void test_heap_dump(void) {
    heap_dump("Empty heap");

    int *p = malloc(sizeof(int));
    *p = 0;
    heap_dump("After p = malloc(4)");

    char *q = malloc(16);
    memcpy(q, "aaaaaaaaaaaaaaa", 16);
    heap_dump("After q = malloc(16)");

    free(p);
    heap_dump("After free(p)");

    free(q);
    heap_dump("After free(q)");
}

static void test_heap_simple(void) {
    // allocate a string and array of ints
    // assign to values, check, then free
    const char *alphabet = "abcdefghijklmnopqrstuvwxyz";
    int len = strlen(alphabet);

    char *str = malloc(len + 1);
    memcpy(str, alphabet, len + 1);

    int n = 10;
    int *arr = malloc(n*sizeof(int));
    for (int i = 0; i < n; i++) {
        arr[i] = i;
    }

    assert(strcmp(str, alphabet) == 0);
    free(str);
    assert(arr[0] == 0 && arr[n-1] == n-1);
    free(arr);
}

static void test_heap_oddballs(void) {
    // test oddball cases
    char *ptr;

    ptr = malloc(900000000); // request too large to fit
    assert(ptr == NULL); // should return NULL if cannot service request
    heap_dump("After reject too-large request");

    ptr = malloc(0); // legal request, though weird
    heap_dump("After malloc(0)");
    free(ptr);

    free(NULL); // legal request, should do nothing
    heap_dump("After free(NULL)");
}

static void test_heap_multiple(void) {
    // array of dynamically-allocated strings, each
    // string filled with repeated char, e.g. "A" , "BB" , "CCC"
    // Examine each string, verify expected contents intact.

    int n = 8;
    char *arr[n];

    for (int i = 0; i < n; i++) {
        int num_repeats = i + 1;
        char *ptr = malloc(num_repeats + 1);
        memset(ptr, 'A' - 1 + num_repeats, num_repeats);
        ptr[num_repeats] = '\0';
        arr[i] = ptr;
    }
    heap_dump("After all allocations");
    for (int i = n-1; i >= 0; i--) {
        int len = strlen(arr[i]);
        char first = arr[i][0], last = arr[i][len -1];
        assert(first == 'A' - 1 + len);  // verify payload contents
        assert(first == last);
        free(arr[i]);
    }
    heap_dump("After all frees");
}


void test_heap_redzones(void) {
    // DO NOT ATTEMPT THIS TEST unless your heap has red zone protection!
    char *ptr;

    ptr = malloc(9);
    memset(ptr, 'a', 9); // write into payload
    free(ptr); // ptr is OK

    ptr = malloc(5);
    ptr[-1] = 0x45; // write before payload
    free(ptr);      // ptr is NOT ok

    ptr = malloc(12);
    ptr[13] = 0x45; // write after payload
    free(ptr);      
}

// Verify display of compromised redzones in memory error alert
void more_redzone_tests(void) {
    char *ptr;

    ptr = malloc(7);
    memcpy(ptr, "11111111111111", 12);
    free(ptr); // ptr is not OK  

    ptr = malloc(7);
    memcpy(ptr - 2, "1111111111", 4);
    free(ptr); // ptr is not OK  
}

// testing coalesce, redzone, memory reporting, everything together :)
static void test_all(void) {
    heap_dump("Empty heap");

    char *o = malloc(12);
    memcpy(o, "abcabcbaajkahfksadfh", 12); 

    char *p = malloc(16);
    memcpy(p, "5", 17); // not okay -- should get flag during freeing attempt

    int *q = malloc(sizeof(int));
    *q = 0;

    long *r = malloc(sizeof(long));
    *r = 139182309;

    heap_dump("After requesting several small blocks (o, p, q, r) - all in use");

    free(r);
    heap_dump("After free(r)");

    // q, r should coalesce
    free(q);
    heap_dump("After free(q)");

    // Memory warning & report with 2 leaks should print out
    free(p);
    heap_dump("After free(p)");
}

// Preliminary test used to verify leak reporting capabilities before 
// modifying cstart.c & Makefile
void test_mem_leak_report(void) {
    char *ptr;

    ptr = malloc(9);
    memset(ptr, 'a', 9);

    ptr = malloc(12);
    memset(ptr, 'b', 12); 
    free(ptr);

    heap_dump("After allocs and frees");
}

void main(void) {
    uart_init();
    uart_putstring("Start execute main() in test_backtrace_malloc.c\n");

    // test_backtrace();

    // test_heap_dump();
    // test_recycle();
    // test_coalesce();
    // test_coalesce2();
    // test_heap_simple();
    // test_heap_oddballs();
    // test_heap_multiple();
    // more_redzone_tests();
    test_all();

    // test_heap_redzones(); // DO NOT USE unless you have implemented red zone protection!
    // test_mem_leak_report();

    uart_putstring("\nSuccessfully finished executing main() in test_backtrace_malloc.c\n");
}
