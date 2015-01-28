/**
 * @file
 *
 * @ingroup mpc55xx
 */

/*
 * Copyright (c) 2012 embedded brains GmbH.  All rights reserved.
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

#include <bsp.h>
#include <bsp/bootcard.h>
#include <bsp/linker-symbols.h>

LINKER_SYMBOL(bsp_section_work_bonus_begin);
LINKER_SYMBOL(bsp_section_work_bonus_size);

void bsp_work_area_initialize(void)
{
  Heap_Area areas [] = {
    {
      bsp_section_work_begin,
      (uintptr_t) bsp_section_work_size
    }, {
      bsp_section_work_bonus_begin,
      (uintptr_t) bsp_section_work_bonus_size
    }
  };

  #ifdef BSP_INTERRUPT_STACK_AT_WORK_AREA_BEGIN
    {
      uint32_t stack_size = rtems_configuration_get_interrupt_stack_size();

      areas [0].begin = (char *) areas [0].begin + stack_size;
      areas [0].size -= stack_size;
    }
  #endif

  bsp_work_area_initialize_with_table(
    areas,
    sizeof(areas) / sizeof(areas [0])
  );
}
