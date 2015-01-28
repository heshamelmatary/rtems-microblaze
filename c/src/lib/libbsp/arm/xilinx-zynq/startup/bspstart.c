/*
 * Copyright (c) 2013-2014 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/bootcard.h>
#include <bsp/arm-a9mpcore-clock.h>
#include <bsp/irq-generic.h>
#include <bsp/linker-symbols.h>

__attribute__ ((weak)) uint32_t zynq_clock_cpu_1x(void)
{
  return ZYNQ_CLOCK_CPU_1X;
}

void bsp_start(void)
{
  a9mpcore_clock_initialize_early();
  bsp_interrupt_initialize();
  rtems_cache_coherent_add_area(
    bsp_nocache_heap_begin,
    (uintptr_t) bsp_nocache_heap_size
  );
}
