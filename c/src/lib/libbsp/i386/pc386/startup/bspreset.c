/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <rtems.h>
#include <bsp.h>
#include <bsp/bootcard.h>

void bsp_reset(void)
{
  /* shutdown and reboot */
  #if (BSP_IS_EDISON == 0)
    outport_byte(0x64, 0xFE);        /* use keyboard controller */
  #else
   *((uint32_t*)0xff009000) = 0xf3;  /* use watchdog */
  #endif
}
