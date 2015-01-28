/*
 * Copyright (c) 2014 On-Line Applications Research Corporation (OAR).
 * Copyright (c) 2013-2014 embedded brains GmbH.  All rights reserved.
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

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include "tmacros.h"

const char rtems_test_name[] = "SMPUNSUPPORTED 1";

static void test(void)
{
  rtems_status_code sc;
  rtems_mode mode;
  rtems_id id;

  rtems_test_assert(rtems_configuration_is_smp_enabled());

  sc = rtems_task_mode(RTEMS_NO_PREEMPT, RTEMS_PREEMPT_MASK, &mode);
  rtems_test_assert(sc == RTEMS_NOT_IMPLEMENTED);

  sc = rtems_task_create(
    rtems_build_name('T', 'A', 'S', 'K'),
    RTEMS_MINIMUM_PRIORITY,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_NO_PREEMPT,
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  rtems_test_assert(sc == RTEMS_UNSATISFIED);
}

static void Init(rtems_task_argument arg)
{
  TEST_BEGIN();

  test();

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_SMP_APPLICATION

#define CONFIGURE_SMP_MAXIMUM_PROCESSORS 1

#define CONFIGURE_MAXIMUM_TASKS 2

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
