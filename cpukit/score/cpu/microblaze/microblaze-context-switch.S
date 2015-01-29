/*
 * COPYRIGHT (c) 2014 Hesham ALMatary <heshamelmatary@gmail.com>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/asm.h>

.text
.align 4

PUBLIC(_CPU_Context_switch)
PUBLIC(_CPU_Context_restore)
PUBLIC(_CPU_Context_restore_fp)
PUBLIC(_CPU_Context_save_fp)

SYM(_CPU_Context_switch):
    swi     r1,  r5, 0
    swi     r13, r5, 4
    swi     r14, r5, 8
    swi     r15, r5, 12    
    swi     r16, r5, 16
    swi     r17, r5, 20
    swi     r18, r5, 24    
    swi     r19, r5, 28
    swi     r20, r5, 32
    swi     r21, r5, 36
    swi     r22, r5, 40
    swi     r23, r5, 44
    swi     r24, r5, 48
    swi     r25, r5, 52
    swi     r26, r5, 56
    swi     r27, r5, 60    
    swi     r28, r5, 64    
    swi     r29, r5, 68    
    swi     r30, r5, 72
    swi     r31, r5, 76
 
SYM(restore):   
    lwi     r1,  r6, 0
    lwi     r13, r6, 4
    lwi     r14, r6, 8
    lwi     r15, r6, 12    
    lwi     r16, r6, 16
    lwi     r17, r6, 20
    lwi     r18, r6, 24    
    lwi     r19, r6, 28
    lwi     r20, r6, 32
    lwi     r21, r6, 36
    lwi     r22, r6, 40
    lwi     r23, r6, 44
    lwi     r24, r6, 48
    lwi     r25, r6, 52
    lwi     r26, r6, 56
    lwi     r27, r6, 60    
    lwi     r28, r6, 64    
    lwi     r29, r6, 68    
    lwi     r30, r6, 72
    lwi     r31, r6, 76    

    rtsd    r15, 8
    
SYM(_CPU_Context_restore):
    add   r6, r5, r0
    brai  restore

