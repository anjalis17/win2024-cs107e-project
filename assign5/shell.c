/* File: shell.c
 * -------------
 * Author: Anjali Sreenivas
 * 
 * Citation (with retest cycle): I kept getting a Valgrind report when I was using peek 
 * and poke commands in my shell saying that I was attempting to free an invalid address,
 * and I was stuck for a super long time as to why. I thought it had something to do with my
 * mallocing and freeing, but I'd traced through it all several times. I explained the issue 
 * and my approach to Daniel, and he helped me realize that my strtonum end pointer initialization 
 * was the issue -- something I hadn't even considered. I originally had the end pointer initalized 
 * to be within the array itself, so my array of tokens was being inadvertently modified, creating 
 * freeing issues later on.
 * 
 * The shell.c file consists of a set of functions that provides 
 * functionality and interface to a simple shell module!
 */
#include "shell.h"
#include "shell_commands.h"
#include "uart.h"
#include "strings.h"
#include "malloc.h"
#include "mango.h"

#define LINE_LEN 80
#define SPACE 32

// Module-level global variables for shell
static struct {
    input_fn_t shell_read;
    formatted_fn_t shell_printf;
} module;


// NOTE TO STUDENTS:
// Your shell commands output various information and respond to user
// error with helpful messages. The specific wording and format of
// these messages would not generally be of great importance, but
// in order to streamline grading, we ask that you aim to match the
// output of the reference version.
//
// The behavior of the shell commands is documented in "shell_commands.h"
// https://cs107e.github.io/header#shell_commands
// The header file gives example output and error messages for all
// commands of the reference shell. Please match this wording and format.
//
// Your graders thank you in advance for taking this care!

// command table
static const command_t commands[] = {
    {"help",  "help [cmd]",  "print command usage and description", cmd_help},
    {"echo",  "echo [args]", "print arguments", cmd_echo},
    {"clear", "clear",       "clear screen (if your terminal supports it)", cmd_clear},
    {"reboot", "reboot",     "reboot the Mango Pi", cmd_reboot},
    {"peek", "peek [addr]",   "print contents of memory at address", cmd_peek},
    {"poke", "poke [addr] [val]",   "store value into memory at address", cmd_poke},
};

const size_t SIZE_COMMANDS_ARR = sizeof(commands) / sizeof(command_t);

// Function handles 'echo' command -- prints arguments to shell
int cmd_echo(int argc, const char *argv[]) {
    for (int i = 1; i < argc; ++i)
        module.shell_printf("%s ", argv[i]);
    module.shell_printf("\n");
    return 0;
}

// Function handles 'help' command
// If no additional arguments, prints usage and description for all entries in command table
// If an additional argument was entered, prints usage and description for that specific command if it exists
// Returns 0 if successfully printed; returns 1 on command not found (error thrown)
int cmd_help(int argc, const char *argv[]) {
    // additional arg was entered
    if (argc > 1) {
        for (int i = 0; i < SIZE_COMMANDS_ARR; i++) {
            // print info for matching command
            if (strcmp(argv[1], commands[i].name) == 0) {
                module.shell_printf("%s     %s\n", commands[i].usage, commands[i].description);
                return 0;
            }
        }
        // throw error if no such command
        module.shell_printf("error: no such command '%s'\n", argv[1]);
        return 1;
    }
    // No additional arg entered -- prints info for all commands
    for (int i = 0; i < SIZE_COMMANDS_ARR; i++) {
        module.shell_printf("%s     %s\n", commands[i].usage, commands[i].description);
    }
    return 0;
}

// Handling 'clear' command -- clears screen
int cmd_clear(int argc, const char* argv[]) {
    // const char *ANSI_CLEAR = "\033[2J"; // if your terminal does not handle formfeed, can try this alternative?
    // module.shell_printf(ANSI_CLEAR);

    module.shell_printf("\f");   // minicom will correctly respond to formfeed character
    return 0;
}

// Handles 'reboot' command - reboots the Mango Pi
int cmd_reboot(int argc, const char *argv[]) {
    module.shell_printf("Rebooting...\n");
    mango_reboot();
}

// Helper function to check that (1) strToNum didn't stop at a non-digit character (confirming that original arg address
// is a valid number), and (2) that the converted address is 4-byte aligned. If yes to both, true is returned indicating that
// the original user-inputted address is valid; else, returns false. 
static bool checkValidAddress(const char** strToNumEndPtr, const char* origArg, long address, const char* funcName) {
     if (**strToNumEndPtr != '\0') {
        module.shell_printf("error: %s cannot convert '%s'\n", funcName, origArg);
        return false;
    } 
    // if address is not 4-byte aligned, throw error
    if (address % 4 != 0) {
        module.shell_printf("error: %s address must be 4-byte aligned\n", funcName);
        return false;
    }
    return true;
}

// Handles 'peek' command -- prints contents of memory at address specified by argv[1]
// Error thrown if invalid address (argv[1] cannot be processed as a number or isn't 4-byte aligned), 
// or if no address arg is provided
int cmd_peek(int argc, const char *argv[]) {
    // if only commmand and no additional arguments, throw error
    if (argc == 1) {
        module.shell_printf("error: peek expects 1 argument [addr]\n");
        return 1;
    }
    const char *endptr = NULL; 
    long address = strtonum(argv[1], &endptr); // function modifies *endptr
    
    if (!checkValidAddress(&endptr, argv[1], address, "peek")) return 1;
    module.shell_printf("%lx:     %08x\n", address, *(int *)address);
    return 0;
}

// Handles 'poke' command -- stores value (specified by argv[2]) into memory at address specified
// by argv[1]
// Error thrown if invalid address (argv[1] cannot be processed as a number or isn't 4-byte aligned), 
// invalid value (argv[2] can't be processed as a number), or one or both arguments are missing
int cmd_poke(int argc, const char *argv[]) {
    // need command + 2 arguments
    if (argc < 3) {
        module.shell_printf("error: poke expects 2 arguments [addr] and [val]\n");
        return 1;
    }

    // attempt conversion of address to an int via strtonum()
    const char *endptr = NULL; 
    // const char* addressArg = argv[1];
    long address = strtonum(argv[1], &endptr); // function modifies *endptr
    if (!checkValidAddress(&endptr, argv[1], address, "poke")) return 1;

    // attempt conversion of value to an int via strtonum()
    long val = strtonum(argv[2], &endptr); // function modifies parameters

    // if value string contains characters that aren't valid digits, throw error 
    if (*endptr != '\0') {
        module.shell_printf("error: poke cannot convert '%s'\n", argv[2]);
        return 1;
    } 

    // write val to address
    *(int *)address = (int) val;
    return 0;
}

// Required initialization for shell
void shell_init(input_fn_t read_fn, formatted_fn_t print_fn) {
    module.shell_read = read_fn;
    module.shell_printf = print_fn;
}

// Called to indicate attempt at illegal/unsupported action
void shell_bell(void) {
    uart_putchar('\a');
}

// Operates as a read-eval-print loop to read a single line of input from the user
// Reads charactres entered by user and stores into destination buffer `buf` 
// Reading stops when user enters return ('\n'). If user enters more characters than what fits in buf 
// (> `bufsize` - 1), the excess characters are rejected and not written to `buf`. Non-ASCII chars > 0x7f are discarded.
void shell_readline(char buf[], size_t bufsize) {
    // initialize `buf` with null terminators
    for (int i = 0; i < bufsize; i++) {
        buf[i] = '\0';
    }
    size_t bufIndexer = 0;

    while (1) {
        unsigned char nextChar = module.shell_read();
        // stop reading when user enters return
        if (nextChar == '\n') {
            module.shell_printf("%c", nextChar);
            break;
        }
        // handle backspaces
        if (nextChar == '\b') {
            // disallow backspacing through shell prompt or to prev line
            // + 2 offset to account for prompt size (e.g. "? ")  
            if (bufIndexer == 0 || (bufIndexer + 2) % LINE_LEN == 0) {
                shell_bell();
                continue;
            }
            module.shell_printf("\b \b");
            bufIndexer--;
            buf[bufIndexer] = '\0';
            continue;
        } 
        // disallow typing more characters than fit in buffer
        if (bufIndexer >= bufsize - 1) {
            shell_bell();
            continue;
        }
        // skip over non-ASCII characters
        if (nextChar > 0x7f) continue;
        // 'normal' case - ASCII character: write to buf, display
        buf[bufIndexer++] = nextChar;
        module.shell_printf("%c", nextChar);
    }
}

// The following three helper functions (strndup, isspace, tokenize) have been repurposed from lab 4. 
static char *strndup(const char *src, size_t n) {
    char* str = malloc(n + 1);
    size_t copySize = n < strlen(src) ? n : strlen(src);
    str = memcpy(str, src, copySize);
    str[copySize] = '\0';
    return str;
}

static bool isspace(char ch) {
    return ch == ' ' || ch == '\t' || ch == '\n';
}

static int tokenize(const char *line, char *array[],  int max) {
    int ntokens = 0;
    const char *cur = line;

    while (ntokens < max) {
        while (isspace(*cur)) cur++;    // skip spaces (stop non-space/null)
        if (*cur == '\0') break;        // no more non-space chars
        const char *start = cur;
        while (*cur != '\0' && !isspace(*cur)) cur++; // advance to end (stop space/null)
        array[ntokens++] = strndup(start, cur - start);   // make heap-copy, add to array
    }
    return ntokens;
}

// Parse `line` and execute command
int shell_evaluate(const char *line) {
    // divide line into array of tokens
    int max = strlen(line); // number of tokens is at most length of line
    char *array[max];   // declare stack array to hold strings
    memset(array, 0, sizeof(array));
    
    int ntokens = tokenize(line, array, max);
    int result = 2; // result used to store function return value
    if (ntokens != 0) {
        // call function to execute specified command (determined using first token)
        const char* command = array[0];
        for (int i = 0; i < SIZE_COMMANDS_ARR; i++) {
            if (strcmp(command, commands[i].name) == 0) {
                result = commands[i].fn(ntokens, (const char **) array);
            }
        }
        // output error message if no matching command found
        // if a matching command was found, then result would have become 0 (function success) or -1 (failure of matching command)
        if (result == 2) module.shell_printf("error: no such command '%s'\n", command);
    }
    // if line contains no tokens (string all empty or all whitespace), return -1; no command executed
    else result = -1;

    // free heap-allocated strings 
    for (int i = 0; i < ntokens; i++) {
        free(array[i]);
    }
    return result;
}

// Main function of shell module; infinite loop
// Enters a read-eval-print loop that repeatedly cycles between `shell_readline` and `shell_evaluate`
// Function call must be preceded by calls to `shell_init` and `keyboard_init`
void shell_run(void) {
    module.shell_printf("Welcome to the CS107E shell. Remember to type on your PS/2 keyboard!\n");
    while (1)
    {
        char line[LINE_LEN];

        module.shell_printf("Pi> ");
        shell_readline(line, sizeof(line));
        shell_evaluate(line);
    }
}
