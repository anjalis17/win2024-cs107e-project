#include "malloc.h"
void memory_report (void); // available in malloc.c but not public interface

extern void main(void);

void _cstart(void) {
    extern char __bss_start, __bss_end;

    for (char *cur = &__bss_start; cur < &__bss_end; cur++) {
        *cur = 0;
    }
    main();
    memory_report();
}
