/* File: backtrace_asm.s
 * ---------------------
 * ***** TODO: add your file header comment here *****
 */

.globl backtrace_get_fp
backtrace_get_fp:
    # move value in fp register to a0 register to be  
    # accessible from C code (via argument passing)
    mv a0, fp
    ret 
