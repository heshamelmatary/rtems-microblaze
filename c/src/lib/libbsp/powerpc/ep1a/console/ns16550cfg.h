/*
 *  This include file contains all console driver definitions for the ns16550.
 */

/*
 * COPYRIGHT (c) 1989-2008.
 * On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef __NS16550_CONFIG_H
#define __NS16550_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  Board specific register access routines
 */

uint8_t Read_ns16550_register(
  uintptr_t  ulCtrlPort,
  uint8_t    ucRegNum
);

void  Write_ns16550_register(
  uintptr_t  ulCtrlPort,
  uint8_t    ucRegNum,
  uint8_t    ucData
);

extern const console_fns ns16550_fns_8245;
extern const console_fns ns16550_fns_polled_8245;

#ifdef __cplusplus
}
#endif

#endif
