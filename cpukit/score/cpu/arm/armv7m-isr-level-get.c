/**
 *  @file
 *
 *  @brief CPU Get ISR Level
 */

/*
 * Copyright (c) 2011 Sebastian Huber.  All rights reserved.
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

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/score/armv7m.h>

#ifdef ARM_MULTILIB_ARCH_V7M

uint32_t _CPU_ISR_Get_level( void )
{
  return 0;
}

#endif /* ARM_MULTILIB_ARCH_V7M */
