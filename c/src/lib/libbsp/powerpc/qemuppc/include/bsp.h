/*
 *  This include file contains some definitions specific to the
 *  qemu powerpc Prep simulator
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _BSP_H
#define _BSP_H

#ifndef ASM

#include <bspopts.h>
#include <bsp/default-initial-extension.h>

#include <rtems.h>
#include <rtems/iosupp.h>
#include <rtems/console.h>
#include <rtems/clockdrv.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  Convert decrementer value to tenths of microseconds (used by shared timer
 *  driver).
 */
#define BSP_Convert_decrementer( _value ) \
  ((int) (((_value) * 10) / bsp_clicks_per_usec))

/*
 * Prototypes for methods that are referenced from .S
 */
void cmain(void);

#ifdef __cplusplus
}
#endif

#endif /* !ASM */

#endif
