/* File: config_fp.s
 * ------------------
 * Author: Anjali Sreenivas
 *
 * Routing to turn on FS register in mstatus to configure hardware floating point.
 */

.attribute arch, "rv64imf_zicsr"

.globl enable_floating_point
enable_floating_point:
    li a0, 1<<13      #  load 1 << 13 into reg a0 
    csrs mstatus, a0  #  write to FS (13th bit of mstatus register)
    ret


