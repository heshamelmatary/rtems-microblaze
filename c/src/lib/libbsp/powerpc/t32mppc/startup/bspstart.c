/*
 * Copyright (c) 2012-2014 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <rtems/config.h>
#include <rtems/counter.h>

#include <bsp.h>
#include <bsp/vectors.h>
#include <bsp/bootcard.h>
#include <bsp/irq-generic.h>
#include <bsp/linker-symbols.h>

LINKER_SYMBOL(bsp_exc_vector_base);

/*
 * Configuration parameter for clock driver.  The Trace32 PowerPC simulator has
 * an odd decrementer frequency.  The time base frequency is one tick per
 * instruction.  The decrementer frequency is one tick per ten instructions.
 * The clock driver assumes that the time base and decrementer frequencies are
 * equal.  For now we simulate processor that issues 10000000 instructions per
 * second.
 */
uint32_t bsp_time_base_frequency = 10000000 / 10;

void BSP_panic(char *s)
{
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);
  (void) level;

  printk("%s PANIC %s\n", rtems_get_version_string(), s);

  while (1) {
    /* Do nothing */
  }
}

void _BSP_Fatal_error(unsigned n)
{
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);
  (void) level;

  printk("%s PANIC ERROR %u\n", rtems_get_version_string(), n);

  while (1) {
    /* Do nothing */
  }
}

void bsp_start(void)
{
  get_ppc_cpu_type();
  get_ppc_cpu_revision();

  rtems_counter_initialize_converter(bsp_time_base_frequency);

  /* Initialize exception handler */
  ppc_exc_initialize_with_vector_base(
    (uintptr_t) bsp_section_work_begin,
    rtems_configuration_get_interrupt_stack_size(),
    bsp_exc_vector_base
  );

  /* Initalize interrupt support */
  bsp_interrupt_initialize();
}
