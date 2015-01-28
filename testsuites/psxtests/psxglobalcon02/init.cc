/*
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
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

const char rtems_test_name[] = "PSXGLOBALCON 2";

class A {
  public:
    A()
    {
      ++i;
    }

    static int i;
};

int A::i;

static A a;

static bool rtems_init_done;

extern "C" void Init(rtems_task_argument argument)
{
  TEST_BEGIN();

  rtems_test_assert(a.i == 1);

  rtems_init_done = true;

  rtems_task_delete(RTEMS_SELF);
  rtems_test_assert(0);
}

static void *POSIX_Init(void *argument)
{
  rtems_test_assert(rtems_init_done);
  rtems_test_assert(a.i == 1);

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 1
#define CONFIGURE_MAXIMUM_POSIX_THREADS 1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
