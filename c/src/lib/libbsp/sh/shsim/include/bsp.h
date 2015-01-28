/*
 *  SH-gdb simulator BSP
 *
 *  This include file contains all board IO definitions.
 */

/*
 *  Author: Ralf Corsepius (corsepiu@faw.uni-ulm.de)
 *
 *  COPYRIGHT (c) 2001, Ralf Corsepius, Ulm, Germany
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *  COPYRIGHT (c) 2001.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _BSP_H
#define _BSP_H

#ifndef ASM

#include <rtems.h>
#include <rtems/clockdrv.h>
#include <rtems/console.h>

#include <bspopts.h>
#include <bsp/default-initial-extension.h>

/*
 * FIXME: One of these would be enough.
 */
#include <rtems/devnull.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Constants */

Thread clock_driver_sim_idle_body(uintptr_t);
#define BSP_IDLE_TASK_BODY clock_driver_sim_idle_body

/*
 * Defined in the linker script 'linkcmds'
 */
extern void *CPU_Interrupt_stack_low;
extern void *CPU_Interrupt_stack_high;

/*
 * BSP methods that cross file boundaries.
 */
int _sys_exit (int n);
void bsp_hw_init(void);

#ifdef __cplusplus
}
#endif

#endif /* !ASM */
#endif
