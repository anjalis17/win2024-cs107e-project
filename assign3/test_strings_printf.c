/* File: test_strings_printf.c
 * ---------------------------
 * Authors: CS107E Course Staff & Anjali Sreenivas
 * 
 * This file contains a breadth of test cases used to verify functionality of 
 * the strings.c and printf.c modules.
 */
#include "assert.h"
#include "printf.h"
#include <stddef.h>
#include "strings.h"
#include "uart.h"

// Prototypes copied from printf.c to allow unit testing of helper functions
int unsigned_to_base(char *buf, size_t bufsize, unsigned long val, int base, size_t min_width);
int signed_to_base(char *buf, size_t bufsize, long val, int base, size_t min_width);

static void test_memset(void) {
    char buf[12];
    size_t bufsize = sizeof(buf);

    memset(buf, 0x77, bufsize); // fill buffer with repeating value
    for (int i = 0; i < bufsize; i++)
        assert(buf[i] == 0x77); // confirm value

    // confirm memset only copies over least significant byte 
    memset(buf, 0x12378, bufsize);
    for (int i = 0; i < bufsize; i++)
        assert(buf[i] == 0x78); // confirm value
 
    memset(buf, 0xf, bufsize);
    for (int i = 0; i < bufsize; i++)
        assert(buf[i] == 0xf); // confirm value
}

static void test_strcmp(void) {
    assert(strcmp("apple", "apple") == 0);
    assert(strcmp("apple", "applesauce") < 0);
    assert(strcmp("pears", "apples") > 0);
    assert(strcmp("", "hi") < 0);
    assert(strcmp("Apple", "apple") < 0);
    assert(strcmp("", "") == 0);
    assert(strcmp("strcmp", "strcm") > 0);
}

static void test_strlcat(void) {
    char buf[20];
    size_t bufsize = sizeof(buf);

    memset(buf, 0x77, bufsize); // init contents with known value

    buf[0] = '\0'; // null in first index makes empty string
    assert(strlen(buf) == 0);

    strlcat(buf, "CS", bufsize); // append CS
    assert(strlen(buf) == 2);
    assert(strcmp(buf, "CS") == 0);

    strlcat(buf, "107e", bufsize); // append 107e
    assert(strlen(buf) == 6);
    assert(strcmp(buf, "CS107e") == 0);

    strlcat(buf, " is the best", bufsize); // append " is the best"
    assert(strlen(buf) == 18);
    assert(strcmp(buf, "CS107e is the best") == 0);

    // Case: bufsize < space needed to append full src string
    strlcat(buf, "! So cool.", bufsize); // should only append "!" due to bufsize constraint
    assert(strlen(buf) == 19);
    assert(strcmp(buf, "CS107e is the best!") == 0);

}

static void test_strtonum(void) {
    long val = strtonum("013", NULL);
    assert(val == 13);

    const char *input = "107rocks";
    const char *rest = NULL;

    val = strtonum(input, &rest);
    assert(val == 107);
    // rest was modified to point to first non-digit character
    assert(rest == &input[3]);

    // testing hex prefix
    val = strtonum("0xff", NULL);
    assert(val == 255); 

    // testing string with invalid character
    const char *invalid_input = "0xffg7a";
    val = strtonum(invalid_input, &rest);
    assert(val == 255);   
    assert(rest == &invalid_input[4]);  

    // testing base 10 string with invalid hex character
    const char *b10_invalid = "78213b50";
    val = strtonum(b10_invalid, &rest);
    assert(val == 78213);   
    assert(rest == &b10_invalid[5]);

    // testing string with invalid first char
    const char *first_invalid = "x78b50";
    val = strtonum(first_invalid, &rest);
    assert(val == 0);   
    assert(rest == &first_invalid[0]);

    // verify hex digits case insensitive
    val = strtonum("0x0aB", NULL);
    assert(val == 171); 

    // retest cycle
    const char *str = "42";
    val = strtonum(str, &rest);
    assert(rest == &str[2]);

    const char *empty_str = "";
    val = strtonum(empty_str, &rest);
    assert(rest == &empty_str[0]);

    const char *valid_hex = "0xabc";
    strtonum(valid_hex, &rest);
    assert(rest == &valid_hex[5]);
}

static void test_unsigned_to_base(void) {
    char buf[5];
    size_t bufsize = sizeof(buf);

    memset(buf, 0x77, bufsize); // init contents with known value
    
    int n = unsigned_to_base(buf, bufsize, 135, 10, 1);
    assert(strcmp(buf, "135") == 0)
    assert(n == 3);

    n = unsigned_to_base(buf, bufsize, 2176, 10, 4);
    assert(strcmp(buf, "2176") == 0)
    assert(n == 4);

    // padding necessary for min_width param
    n = unsigned_to_base(buf, bufsize, 152, 10, 4);
    assert(strcmp(buf, "0152") == 0)
    assert(n == 4);

    n = unsigned_to_base(buf, bufsize, 13, 10, 4);
    assert(strcmp(buf, "0013") == 0)
    assert(n == 4);

    // truncation required due to bufsize 
    n = unsigned_to_base(buf, bufsize, 13123, 10, 4);
    assert(strcmp(buf, "1312") == 0)
    assert(n == 5);

    // padding present, but bufsize takes precedence over min_width
    n = unsigned_to_base(buf, bufsize, 13123, 10, 6);
    assert(strcmp(buf, "0131") == 0)
    assert(n == 6);

    n = unsigned_to_base(buf, bufsize, 17, 10, 6);
    assert(strcmp(buf, "0000") == 0)
    assert(n == 6);

    // Testing hex conversions
    n = unsigned_to_base(buf, bufsize, 255, 16, 2);
    assert(strcmp(buf, "ff") == 0)
    assert(n == 2);

    n = unsigned_to_base(buf, bufsize, 11248, 16, 2);
    assert(strcmp(buf, "2bf0") == 0)
    assert(n == 4);

    // Verifying min_width padding and bufsize conditions on hex conversions
    n = unsigned_to_base(buf, bufsize, 11248, 16, 5);
    assert(strcmp(buf, "02bf") == 0)
    assert(n == 5);

    n = unsigned_to_base(buf, bufsize, 13, 16, 3);
    assert(strcmp(buf, "00d") == 0)
    assert(n == 3);

    // for bufsize of 0, contents at buf should remain unchanged
    n = unsigned_to_base(buf, 0, 1354, 10, 1);
    assert(strcmp(buf, "00d") == 0)
    assert(n == 4);

    // for bufsize of 1, null terminator should be located at buf
    n = unsigned_to_base(buf, 1, 1354, 10, 1);
    assert(strcmp(buf, "") == 0)
    assert(n == 4);
}

static void test_signed_to_base(void) {
    char buf[5];
    size_t bufsize = sizeof(buf);

    memset(buf, 0x77, bufsize); // init contents with known value

    int n = signed_to_base(buf, bufsize, -255, 16, 1);
    assert(strcmp(buf, "-ff") == 0)
    assert(n == 3);

    n = signed_to_base(buf, bufsize, -100, 10, 4);
    assert(strcmp(buf, "-100") == 0)
    assert(n == 4);

    // with padding
    n = signed_to_base(buf, bufsize, -255, 16, 4);
    assert(strcmp(buf, "-0ff") == 0)
    assert(n == 4);

    n = signed_to_base(buf, bufsize, -1, 16, 3);
    assert(strcmp(buf, "-01") == 0)
    assert(n == 3);

    // testing truncation
    n = signed_to_base(buf, bufsize, -9999, 10, 3);
    assert(strcmp(buf, "-999") == 0)
    assert(n == 5);

    // padding with truncation
    n = signed_to_base(buf, bufsize, -9999, 10, 6);
    assert(strcmp(buf, "-099") == 0)
    assert(n == 6);

    // positive value
    n = signed_to_base(buf, bufsize, 35, 10, 0);
    assert(strcmp(buf, "35") == 0)
    assert(n == 2);

    // inadequate bufsizes - sneaky edge cases
    n = signed_to_base(buf, 0, 35, 10, 0);
    assert(strcmp(buf, "35") == 0)  // contents should be unchanged with bufsize 0
    assert(n == 2);

    n = signed_to_base(buf, 1, 35, 10, 0);
    assert(strcmp(buf, "") == 0)   // null terminator at buf
    assert(n == 2);

    n = signed_to_base(buf, 2, -35, 10, 0);
    assert(strcmp(buf, "-") == 0)  
    assert(n == 3);

    n = signed_to_base(buf, 1, -35, 16, 0);
    assert(strcmp(buf, "") == 0)   // null terminator at buf
    assert(n == 3);
}

static void snprintf_retest_cycle(void) {
    char buf[50];
    size_t bufsize = sizeof(buf);
    memset(buf, 0x77, bufsize); // init contents with known value

    // // test val of 0 with %d and %x
    // snprintf(buf, bufsize, "%d", 0);
    // assert(strcmp(buf, "0") == 0);

    // memset(buf, 0x77, bufsize); // reset buf contents

    // snprintf(buf, bufsize, "%x", 0);
    // assert(strcmp(buf, "0") == 0);

    // // %ld, %lx test
    // snprintf(buf, bufsize, "%ld", 999888777666555444L);
    // assert(strcmp(buf, "999888777666555444") == 0);

    // snprintf(buf, 5, "%ld", 999888777666555444L);
    // assert(strcmp(buf, "9998") == 0);

    // snprintf(buf, 5, "%lx", ~0L);
    // assert(strcmp(buf, "ffff") == 0);

    // snprintf(buf, 20, "%lx", ~0L);
    // assert(strcmp(buf, "ffffffffffffffff") == 0);

    // // %x 
    // snprintf(buf, bufsize, "%x", 0x9abcdef0);
    // assert(strcmp(buf, "9abcdef0") == 0);

    // snprintf(buf, 20, "%x", 0xffffffff);
    // assert(strcmp(buf, "ffffffff") == 0);
    
    // printf("%x", 0x9abcdef0);
    unsigned long val = 0x9abcdef0;
    unsigned int n = unsigned_to_base(buf, bufsize, val, 16, 0);
    assert(strcmp(buf, "9abcdef0") == 0);


    // snprintf(buf, 10, "%x", 0x9abcdef0);
    // assert(strcmp(buf, "9abcdef0") == 0);
}

static void test_snprintf(void) {
    char buf[100];
    size_t bufsize = sizeof(buf);

    memset(buf, 0x77, bufsize); // init contents with known value

    // Start off simple...
    snprintf(buf, bufsize, "Hello, world!");
    assert(strcmp(buf, "Hello, world!") == 0);

    // Character
    snprintf(buf, bufsize, "%c", 'A');
    assert(strcmp(buf, "A") == 0);

    snprintf(buf, bufsize, "My name is Anjali%c", '!');
    assert(strcmp(buf, "My name is Anjali!") == 0);

    snprintf(buf, bufsize, "%c%c = 100%% fresh", 'C', 'S');
    assert(strcmp(buf, "CS = 100% fresh") == 0);

    // Decimal
    snprintf(buf, bufsize, "%d", 45);
    assert(strcmp(buf, "45") == 0);

    snprintf(buf, bufsize, "I turn %d on February %dth!", 19, 17);
    assert(strcmp(buf, "I turn 19 on February 17th!") == 0);

    snprintf(buf, bufsize, "test w negative: %d", -675);
    assert(strcmp(buf, "test w negative: -675") == 0);

    snprintf(buf, bufsize, "%05d", 123);
    assert(strcmp(buf, "00123") == 0);

    snprintf(buf, bufsize, "is this really working?!! %010d", 0xff);
    assert(strcmp(buf, "is this really working?!! 0000000255") == 0);

    // Hexadecimal
    snprintf(buf, bufsize, "%x", 255);
    assert(strcmp(buf, "ff") == 0);

    snprintf(buf, bufsize, "It's 0x%x!!", 2024);
    assert(strcmp(buf, "It's 0x7e8!!") == 0);

    snprintf(buf, bufsize, "%04x", 0xef);
    assert(strcmp(buf, "00ef") == 0);

    snprintf(buf, bufsize, "%04lx", ~0L);
    assert(strcmp(buf, "ffffffffffffffff") == 0);

    // Pointer
    snprintf(buf, bufsize, "%p", (void *) 0x20200004);
    assert(strcmp(buf, "0x20200004") == 0);

    // String
    snprintf(buf, bufsize, "%s", "binky");
    assert(strcmp(buf, "binky") == 0);

    snprintf(buf, bufsize, "CS107e is the %s of all time!", "BEST");
    assert(strcmp(buf, "CS107e is the BEST of all time!") == 0);

    // Format string with intermixed codes
    snprintf(buf, bufsize, "CS%d%c!", 107, 'e');
    assert(strcmp(buf, "CS107e!") == 0);

    // Test return value
    assert(snprintf(buf, bufsize, "Hello") == 5);
    assert(snprintf(buf, 2, "Hello") == 5);

    // Test with limited bufsize
    char buf_lim[10];
    size_t bufsize_lim = sizeof(buf_lim);

    memset(buf_lim, 0x77, bufsize_lim); 
    int length = snprintf(buf_lim, bufsize_lim, "CS%d%c is %s", 107, 'e', "SO COOL");
    assert(strcmp(buf_lim, "CS107e is") == 0);
    assert(length == 17);
}

void test_printf(void) {
    printf("CS107e is the %s of all time!\n", "BEST");
    printf("CS%d%c is %s", 107, 'e', "SO COOL\n");
    printf("PRINTF IS WORKING%s YAY TIMES %d\n", "?!", 0x186a0);
}

// This function just here as code to disassemble for extension
int sum(int n) {
    int result = 6;
    for (int i = 0; i < n; i++) {
        result += i * 3;
    }
    return result + 729;
}

void test_disassemble(void) {
    const unsigned int add = 0x00f706b3;
    const unsigned int xori = 0x0015c593;
    const unsigned int bne = 0xfe061ce3;
    const unsigned int sd = 0x02113423;

    // If you have not implemented the extension, core printf
    // will output address not disassembled followed by I
    // e.g.  "... disassembles to 0x07ffffd4I"
    printf("Encoded instruction %x disassembles to %pI\n", add, &add);
    printf("Encoded instruction %x disassembles to %pI\n", xori, &xori);
    printf("Encoded instruction %x disassembles to %pI\n", bne, &bne);
    printf("Encoded instruction %x disassembles to %pI\n", sd, &sd);

    unsigned int *fn = (unsigned int *)sum; // disassemble instructions from sum function
    for (int i = 0; i < 10; i++) {
        printf("%p:  %x  %pI\n", &fn[i], fn[i], &fn[i]);
    }
}


void main(void) {
    uart_init();
    uart_putstring("Start execute main() in test_strings_printf.c\n");

    // test_memset();
    // test_strcmp();
    // test_strlcat();
    // test_strtonum();
    // test_unsigned_to_base();
    // test_signed_to_base();
    // test_snprintf();
    snprintf_retest_cycle();
    // test_printf();
   // test_disassemble();


    uart_putstring("Successfully finished executing main() in test_strings_printf.c\n");
}
