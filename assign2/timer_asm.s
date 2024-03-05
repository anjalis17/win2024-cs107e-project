/* File: timer_asm.s
 * ------------------
 * ***** TODO: add your file header comment here *****
 */

.attribute arch, "rv64imac_zicsr"

.globl timer_get_ticks
timer_get_ticks:
    # Take care to use correct dest register for return value
    # so as to interoperate with C calling conventions
    csrr a0, time;
    ret
