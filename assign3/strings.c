/* File: strings.c
 * ---------------
 * Author: Anjali Sreenivas
 * 
 * The strings.c file consists of a basic set of functions that can 
 * be used to work with C strings. 
 */
#include "strings.h"

// This function copies `n` bytes of data from the memory location `src` 
// to the memory location `dst`. 
void *memcpy(void *dst, const void *src, size_t n) {
    /* Copy contents from src to dst one byte at a time */
    char *d = dst;
    const char *s = src;
    while (n--) {
        *d++ = *s++;
    }
    return dst;
}

// This function writes `n` bytes of value `val` to the memory area `dst`.
void *memset(void *dst, int val, size_t n) {
    char *d = dst;
    // Since 'val' is passed as an int (due to historical artifact), 
    // change to char type to only copy least significant byte
    const char val_c = val;
    while (n > 0) {
        *d = val_c;
        d++; n--;
    }
    return dst;
}

// The strlen function computes and returns the length of the
// null-terminated string starting at memory location `str`.
size_t strlen(const char *str) {
    /* Implementation a gift to you from lab3 */
    size_t n = 0;
    while (str[n] != '\0') {
        n++;
    }
    return n;
}

// strcmp lexicographically compares the null-terminated strings `s1` and `s2`. 
// Return value - positive int if `s1` > `s2`, negative int if `s1` < `s2`, and 0 if the
// strings are equivalent.
int strcmp(const char *s1, const char *s2) {
    // Keep incrementing s1 and s2 pointers to move to next character in each string
    // until s1 pointer has reached null terminator or s1 and s2 pointers point to 
    // different characters 
    // (Latter check also implicitly handles the case where s2 points to null terminator 
    // when s1 doesn't)
    while (*s1 != '\0' && *s1 == *s2) {
        s1++;
        s2++;
    }
    return *s1 - *s2;
}

// This function appends the null-terminated string `src` to destination buffer `dst` with 
// buffer size `dstsize`. At most dstsize - strlen(dst) - 1 chars will be appended from src.
size_t strlcat(char *dst, const char *src, size_t dstsize) {
    size_t dstLenOrig = strlen(dst);
    // If no null terminator within first dstsize characters of 'dst', write nothing to 'dst'
    if (dstLenOrig >= dstsize) {
        return dstsize + strlen(src);
    }

    // copySize is min(remaining space in dst buffer (-1 accounts for null terminator), len of src string).
    // copySize represents num of bytes from src string that will be appended to end of dst string
    size_t copySize = (dstsize - dstLenOrig - 1) < strlen(src) ? 
                        (dstsize - dstLenOrig - 1) : strlen(src);
    char *d = dst;

    // Move dst pointer to end of dst string (where null terminator is)
    while(*d != '\0') {
        d++;
    }
    // Append src (or portion of src) string to end of dst string
    memcpy(d, src, copySize);
    // Add null terminator
    *(d + copySize) = '\0';

    return dstLenOrig + strlen(src);
}

// Helper function that returns the corresponding digit value for an ascii value 'ch'
// 'ch' must be the ascii value for a valid decimal or hexadecimal digit
static char asciiToDigit(const char ch) {
    // ch within '0' to '9' range
    if (ch >= 48 && ch <= 57) {
        return ch - 48;       // ch - '0'
    }
    // ch within 'A' to 'F' range
    if (ch >= 65 && ch <= 70) {
        return ch - 55;       // 10 + (ch - 'A')
    }
    // ch within 'a' to 'f' range
    if (ch >= 97 && ch <= 102) {
        return ch - 87;       // 10 + (ch - 'a')
    }
    return 0;
}

// Helper function to compute and return `base` to the power of
// `pow` as an int 
static unsigned int pow_base(char base, int pow) {
    unsigned int val = 1;
    for (int i = 1; i <= pow; i++) {
        val *= base;
    }
    return val;
}

// Converts digit characters in `str` to corresponding unsigned long value. 
// If `str` begins with the prefix "0x", then the digit chars are interpreted as
// hex digits. After the function, `endptr` (if passed in as non-NULL) points to the
// first invalid char in `str` that could not be processed as a valid digit, indicating where 
// conversion stopped. 
unsigned long strtonum(const char *str, const char **endptr) {
    unsigned long num = 0;
    char base = 10;
    unsigned int numPlacesShifted = 0;

    // if hex number
    if (*str == '0' && *(str + 1 ) == 'x') {
        str += 2;    // move to char in `str` after '0x'
        base = 16;
    }

    // get to null terminator in str and count number of places shifted
    while (*str != '\0') {
        char ch = *str;
        // if ch is an invalid character...
        if (!(ch >= 48 && ch <= 57)) { // ASCII range for digits '0' - '9'
            // if base 10 OR if not a hex digit ('A' - 'F' or 'a' - 'f'), break and accordingly
            // modify address value contained within endptr
            if (base == 10 || (!((ch >= 65 && ch <= 70) || (ch >= 97 && ch <= 102)))) {
                break;
            }
        }
        str++; numPlacesShifted++;
    }
    *endptr = str;
    str--; // go back to last valid char in str

    // convert to number; parsing digits of `str` in reverse order
    for (int i = 0; i < numPlacesShifted; i++) {
        char ch = *str;
        char digit = asciiToDigit(ch);
        num += digit * (pow_base(base, i)); // digit multiplied by place value
        str--;
    }
    return num;
}
