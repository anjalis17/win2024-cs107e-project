/* File: printf.c
 * --------------
 * Author: Anjali Sreenivas
 * 
 * The printf.c file consists of a set of functions that can be used
 * to custom-print strings using different format codes!
 * 
 * The format codes supported within the format string are listed and translated 
 * as follows:
 * %c  single character
 * %s  string
 * %d  signed decimal int   (%ld long decimal), optional width*
 * %x  unsigned hex int     (%lx long hex), optional width*
 * %p  pointer
 * %%  percent symbol
 * *For the signed decimal and unsigned hex integers (or longs), an optional width
 * can be specified with a leading 0 following the %. For example, %014x outputs 
 * an unsigned hex int as a string of at least 14 characters (padded with leading zeroes).
 */

#include "printf.h"
#include <stdarg.h>
#include <stdint.h>
#include "strings.h"
#include "uart.h"

/* Prototypes for internal helpers.
 * Typically these would be qualified as static (private to module)
 * but, in order to call them from the test program, we declare them externally
 */
int unsigned_to_base(char *buf,
                     size_t bufsize,
                     unsigned long val,
                     int base, size_t
                     min_width);
int signed_to_base(char *buf,
                   size_t bufsize,
                   long val,
                   int base,
                   size_t min_width);


#define MAX_OUTPUT_LEN 1024

/* Converts an unsigned long `val` to a string of chars matching base 10 or 16, specified 
by the `base` param. 
Padding in the form of leading 0s is added to get the string to `min_width` length
if it's shorter. Function returns the length of the converted string as an int. 
The string is written to memory location `buf`, unless the converted string length meets or exceeds the 
buffer size constraint (`buf`). If so, only  `bufsize` - 1 number of chars in the converted string 
are written to `buf` */
int unsigned_to_base(char *buf, size_t bufsize, unsigned long val, int base, size_t min_width) {
    char temp_buf[MAX_OUTPUT_LEN];    // declaring a max-sized buffer on the stack for temp use
    size_t tempSize = sizeof(temp_buf);

    unsigned int indexTemp = tempSize - 1;  // keeping track of index in temp_buf to write to (writing back to front)
    if (val == 0) {
        temp_buf[indexTemp] = '0';
        indexTemp--;
    }
    while (val > 0) {
        char digit = val % base;
        if (digit >= 0 && digit <= 9) {
            digit += '0';      // conversion to ASCII: digit ('0' - '9') = digit (0 - 9) + '0' 
        }
        else {    // hex digit
            digit += 'a' - 10;      // digit = 'a' + (digit - 10)
        }
        temp_buf[indexTemp] = digit;
        val /= base;      // divide by base to move to next digit to process
        indexTemp--;
    }
    // handling min_width - padding as needed
    int charCount = tempSize - indexTemp - 1; // number of chars written so far
    while (charCount < min_width) {
        temp_buf[indexTemp] = '0';   
        indexTemp--; charCount++;
    }

    indexTemp++;    // move to last processed digit stored in temp_buf (left-most digit of val converted to ASCII)
    
    // only write to buf if bufsize > 0
    if (bufsize > 0) {
        // loop through written values in temp_buf (front to back)
        // copy to buf in accordance with what bufsize permits
        while (indexTemp != tempSize && bufsize > 1) {
            *buf = temp_buf[indexTemp];
            buf++; indexTemp++;
            bufsize--;  // used to keep track of remaining amount of space left in buf
        }
        *buf = '\0'; // add null terminator
    }
    
    // return max of num of chars in converted string excluding padding and min width 
    // (this gives length of converted string placed into temp_buf)
    return charCount < min_width ? min_width : charCount;
}

// Converts a signed long `val` to a string of chars of base specified by tbe `base` param, 
// adding padding based on the `min_width` param as needed.  
// Like unsigned_to_base, writing to `buf` is size constrained (via `bufsize` param), so converted
// string is truncated to fit into `buf` if necessary. Also returns length of full converted
// string (including the '-' sign if present).
int signed_to_base(char *buf, size_t bufsize, long val, int base, size_t min_width) {
    // if nonnegative val, functionality is same as unsigned to base
    if (val >= 0) {
        return unsigned_to_base(buf, bufsize, val, base, min_width);
    }
    // if no space to write characters (other than null terminator)...
    // call unsigned_to_base on bufsize to put null terminator or nothing into buf
    if (bufsize - 1 < 1) {
        return unsigned_to_base(buf, bufsize, (unsigned long) (val * -1), base, min_width) + 1;
    }
    // Put minus sign into buf, then call unsigned_to_base on positive form of val to fill in the rest.
    // accounting for the added '-'
    *buf = 45;  // ASCII value of minus sign ('-')
    if (min_width > 0) {
        min_width--;
    }
    return unsigned_to_base(buf + 1, bufsize - 1, (unsigned long) val * -1, base, min_width) + 1; 
}

// Workhorse of printf -- Formats variadic args (passed in va_list form) according to format string, and 
// writes formatted string into `buf` constrained by `bufsize`. Returns length of full formatted 
// string (regardless of whether its entirety fit into `buf`).
int vsnprintf(char *buf, size_t bufsize, const char *format, va_list args) {
    // initialize all of temp buffer on stack to null terminator
    // max output length = 1024, so buffer size of 1024 + 1 (for null terminator) allocated 
    char temp[MAX_OUTPUT_LEN + 1];
    for (int i = 0; i < bufsize; i++) {
        temp[i] = '\0';
    }
    size_t tempIndexer = 0;

    // Process full format string 
    while (*format != '\0') {
        // In each case, advance (1) format string pointer and (2) tempIndexer by length of formatted string appended to temp
        // character (%c)
        if (*format == '%' && *(format + 1) == 'c') {
            char arg = va_arg(args, int);
            temp[tempIndexer] = arg;
            format += 2; tempIndexer++; 
        }
        // percent symbol (%%)
        else if (*format == '%' && *(format + 1) == '%') {
            temp[tempIndexer] = '%';
            format += 2; tempIndexer++; 
        }
        // string (%s)
        else if (*format == '%' && *(format + 1) == 's') {
            char* string = va_arg(args, char*);
            strlcat(temp + tempIndexer, string, (MAX_OUTPUT_LEN + 1) - tempIndexer);
        
            format += 2; tempIndexer += strlen(string);
        }
        // signed decimal int (%d)
        else if (*format == '%' && *(format + 1) == 'd') {
            int signed_num = va_arg(args, int);   // signed decimal int
            int lenSignedToBase = signed_to_base(temp + tempIndexer, (MAX_OUTPUT_LEN + 1) - tempIndexer, signed_num, 10, 0); 

            format += 2; tempIndexer += lenSignedToBase;
        }
        // unsigned hex int (%x)
        else if (*format == '%' && *(format + 1) == 'x') {
            unsigned int unsigned_hex = va_arg(args, int);   // unsigned hex int
            int lenUnsignedToBase = unsigned_to_base(temp + tempIndexer, (MAX_OUTPUT_LEN + 1) - tempIndexer, unsigned_hex, 16, 0); 

            format += 2; tempIndexer += lenUnsignedToBase;
        }
        // signed decimal long or unsigned hex long; no width
        else if (*format == '%' && *(format + 1) == 'l') {
            format += 2;
            
            int base = 10;
            if (*format == 'x') base = 16;

            long num = 0; int lenNumToBase;
            if (base == 16) { // read as unsigned long
                num = (unsigned long) va_arg(args, long);
                lenNumToBase = unsigned_to_base(temp + tempIndexer, (MAX_OUTPUT_LEN + 1) - tempIndexer, num, base, 0);
            }
            else { // base 10, read as signed long
                num = va_arg(args, long);
                lenNumToBase = signed_to_base(temp + tempIndexer, (MAX_OUTPUT_LEN + 1) - tempIndexer, num, base, 0);
            }
            format++; tempIndexer += lenNumToBase;
        }
        // signed decimal or unsigned hex int or long with optional width
        else if (*format == '%' && *(format + 1) == '0') {
            // creating format_buf on stack to keep track of portion of format code containing width data and optional 'l' (long)
            char format_buf[10]; 
            char* pointerToFormatBuf = format_buf;  // pointer to move through format_buf in while loop
            const char** doublePointerToFormatBuf = (const char**) &pointerToFormatBuf; 

            format += 2;
            // copy format characters of code into format_buf
            while (!(*format == 'd' || *format == 'x')) {
                *pointerToFormatBuf = *format;
                format++; pointerToFormatBuf++;
            }
            *pointerToFormatBuf = '\0';

            // doublePointerToFormatBuf gives access to first invalid digit char in format_buf after strtonum is called
            int width = strtonum(format_buf, doublePointerToFormatBuf);
            int base = 10;
            if (*format == 'x') base = 16;
            format++;

            long num = 0; // initializing container to store the arg to process
            int lenNumToBase;
            if (**doublePointerToFormatBuf == 'l') { // arg to process is a long
                if (base == 16) { // read as unsigned long
                    num = (unsigned long) va_arg(args, long);
                    lenNumToBase = unsigned_to_base(temp + tempIndexer, (MAX_OUTPUT_LEN + 1) - tempIndexer, num, base, width);
                }
                else { // base 10, read as signed long
                    num = va_arg(args, long);
                    lenNumToBase = signed_to_base(temp + tempIndexer, (MAX_OUTPUT_LEN + 1) - tempIndexer, num, base, width);
                }
            }
            else { // arg to process is an int
                num = (int) va_arg(args, int);
                lenNumToBase = signed_to_base(temp + tempIndexer, (MAX_OUTPUT_LEN + 1) - tempIndexer, num, base, width);
            }

            tempIndexer += lenNumToBase;
        }
        // pointer (%p)
        else if (*format == '%' && *(format + 1) == 'p') {
            // append 0x
            temp[tempIndexer] = '0'; temp[tempIndexer + 1] = 'x';
            tempIndexer += 2; 

            // output width-8 hex string address
            void* pointer = va_arg(args, void*);   // signed decimal int
            int lenUnsignedToBase = unsigned_to_base(temp + tempIndexer, (MAX_OUTPUT_LEN + 1) - tempIndexer, (unsigned long) pointer, 16, 8); 
             
            format+=2; tempIndexer += lenUnsignedToBase;
        }
        else { // normal processing (copy from format string as is)
            temp[tempIndexer] = *format;
            format++; tempIndexer++;
        }
    }

    // tempIndexer at this point = # chars written to temp buffer
    // copySize is min of tempIndexer and bufSize - 1
    size_t copySize = tempIndexer < (bufsize - 1) ? tempIndexer : bufsize - 1;

    // only write to buf if bufsize > 0
    if (bufsize > 0) {
        memcpy(buf, temp, copySize);
        *(buf + copySize) = '\0';
    }

    return tempIndexer;
}

// Same functionality as vsnprintf; calls vsnprintf to process formatting with a known
// number of arguments.
int snprintf(char *buf, size_t bufsize, const char *format, ...) {
    va_list ap; // declare va_list
    va_start(ap, format); // init va_list, read arguments following argument named format
    // Packaged variadic arguments into va_list `ap` to call vsnprintf helper
    int len = vsnprintf(buf, bufsize, format, ap);
    va_end(ap);

    return len;
}

// Prints formatted string to UART 
// Calls vsnprinf helper to format variadic args according to `format`
int printf(const char *format, ...) {
    char arr[MAX_OUTPUT_LEN + 1];
    size_t bufsize = sizeof(arr);
    
    va_list ap; // declare va_list
    va_start(ap, format); // init va_list, read arguments following argument named format
    int len = vsnprintf(arr, bufsize, format, ap);
    va_end(ap);

    uart_putstring(arr);
    return len;
}


/* From here to end of file is some sample code and suggested approach
 * for those of you doing the disassemble extension. Otherwise, ignore!
 *
 * The struct insn bitfield is declared using exact same layout as bits are organized in
 * the encoded instruction. Accessing struct.field will extract just the bits
 * apportioned to that field. If you look at the assembly the compiler generates
 * to access a bitfield, you will see it simply masks/shifts for you. Neat!
 */
/*
static const char *reg_names[32] = {"zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
                                    "s0/fp", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
                                    "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
                                    "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6" };

struct insn  {
    uint32_t opcode: 7;
    uint32_t reg_d:  5;
    uint32_t funct3: 3;
    uint32_t reg_s1: 5;
    uint32_t reg_s2: 5;
    uint32_t funct7: 7;
};

void sample_use(unsigned int *addr) {
    struct insn in = *(struct insn *)addr;
    printf("opcode is 0x%x, reg_dst is %s\n", in.opcode, reg_names[in.reg_d]);
}
*/
