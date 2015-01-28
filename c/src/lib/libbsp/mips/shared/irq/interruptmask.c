/**
 *  @file
 *  
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <rtems.h>

/*
 *  This function returns a mask value which is used to select the bits
 *  in the processor status register that can be set to enable interrupts.
 *  The mask value should not include the 2 software interrupt enable bits.
 */

uint32_t mips_interrupt_mask( void )
{
  uint32_t interrupt_mask;

  interrupt_mask = 0x0000fc00;
  return(interrupt_mask);
}
