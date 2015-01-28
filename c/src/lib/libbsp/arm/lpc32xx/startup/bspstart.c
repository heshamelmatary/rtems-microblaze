/**
 * @file
 *
 * @ingroup arm_lpc32xx
 *
 * @brief Startup code.
 */

/*
 * Copyright (c) 2009-2014 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <rtems/counter.h>

#include <bsp.h>
#include <bsp/bootcard.h>
#include <bsp/irq-generic.h>

CPU_Counter_ticks _CPU_Counter_read(void)
{
  return lpc32xx_timer();
}

void bsp_start(void)
{
  rtems_counter_initialize_converter(LPC32XX_PERIPH_CLK);
  bsp_interrupt_initialize();
}
