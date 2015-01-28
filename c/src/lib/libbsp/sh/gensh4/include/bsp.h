/*
 *  generic sh4 BSP
 *
 *  This include file contains all board IO definitions.
 */

/*
 *  Copyright (C) 2001 OKTET Ltd., St.-Petersburg, Russia
 *  Author: Victor V. Vengerov <vvv@oktet.ru>
 *
 *  Based on work:
 *  Author: Ralf Corsepius (corsepiu@faw.uni-ulm.de)
 *
 *  COPYRIGHT (c) 1997-1998, FAW Ulm, Germany
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *
 *  COPYRIGHT (c) 1998-2001.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 *
 *  Minor adaptations for sh2 by:
 *  John M. Mills (jmills@tga.com)
 *  TGA Technologies, Inc.
 *  100 Pinnacle Way, Suite 140
 *  Norcross, GA 30071 U.S.A.
 *
 *  This modified file may be copied and distributed in accordance
 *  the above-referenced license. It is provided for critique and
 *  developmental purposes without any warranty nor representation
 *  by the authors or by TGA Technologies.
 */

#ifndef _BSP_H
#define _BSP_H

#include <rtems.h>
#include <rtems/clockdrv.h>
#include <rtems/console.h>
#include <bspopts.h>
#include <bsp/default-initial-extension.h>
#include <termios.h> /* for tcflag_t */

#include "rtems/score/sh7750_regs.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Constants */

/*
 * Defined in the linker script 'linkcmds'
 */

extern void *CPU_Interrupt_stack_low;
extern void *CPU_Interrupt_stack_high;

/*
 * Defined in start.S
 */
extern uint32_t   boot_mode;
#define SH4_BOOT_MODE_FLASH 0
#define SH4_BOOT_MODE_IPL   1

/*
 *  Device Driver Table Entries
 */

/*
 * We redefine CONSOLE_DRIVER_TABLE_ENTRY to redirect /dev/console
 */
#undef CONSOLE_DRIVER_TABLE_ENTRY
#define CONSOLE_DRIVER_TABLE_ENTRY \
  { console_initialize, console_open, console_close, \
      console_read, console_write, console_control }

/*
 * BSP methods that cross file boundaries.
 */
void bsp_hw_init(void);
void early_hw_init(void);
void bsp_cache_on(void);
extern int _sci_get_brparms(
  tcflag_t      cflag,
  unsigned char *smr,
  unsigned char *brr
);

#ifdef __cplusplus
}
#endif

#endif
