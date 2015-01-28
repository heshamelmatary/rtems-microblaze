/*
 *  This file contains a typical set of register access routines which may be
 *  used with the m48t08 chip if accesses to the chip are as follows:
 *
 *    + registers are accessed as bytes
 *    + registers are only byte-aligned (no address gaps)
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <rtems.h>
#include <libchip/rtc.h>
#include <libchip/m48t08.h>

#ifndef _M48T08_MULTIPLIER
#define _M48T08_MULTIPLIER 1
#define _M48T08_NAME(_X) _X
#define _M48T08_TYPE uint8_t
#endif

#define CALCULATE_REGISTER_ADDRESS( _base, _reg ) \
  (_M48T08_TYPE *)((_base) + ((_reg) * _M48T08_MULTIPLIER ))

/*
 *  M48T08 Get Register Routine
 */

uint32_t   _M48T08_NAME(m48t08_get_register)(
  uintptr_t   ulCtrlPort,
  uint8_t     ucRegNum
)
{
  _M48T08_TYPE *port;

  port = CALCULATE_REGISTER_ADDRESS( ulCtrlPort, ucRegNum );

  return *port;
}

/*
 *  M48T08 Set Register Routine
 */

void  _M48T08_NAME(m48t08_set_register)(
  uintptr_t   ulCtrlPort,
  uint8_t     ucRegNum,
  uint32_t    ucData
)
{
  _M48T08_TYPE *port;

  port = CALCULATE_REGISTER_ADDRESS( ulCtrlPort, ucRegNum );

  *port = ucData;
}
