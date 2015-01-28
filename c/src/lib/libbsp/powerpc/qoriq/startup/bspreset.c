/**
 * @file
 *
 * @ingroup QorIQ
 *
 * @brief BSP reset.
 */

/*
 * Copyright (c) 2010 embedded brains GmbH.  All rights reserved.
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

#include <stdbool.h>

#include <bsp/bootcard.h>

void bsp_reset(void)
{
  while (true) {
    /* Do nothing */
  }
}
