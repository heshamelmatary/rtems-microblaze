/*  system.h
 *
 *  This include file contains information that is included in every
 *  function in the test set.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <tmacros.h>

/* functions */

rtems_task Init(
  rtems_task_argument argument
);

rtems_task Test_task(
  rtems_task_argument argument
);

rtems_timer_service_routine Delayed_send_event(
  rtems_id  ignored_id,
  void     *ignored_address
);

/* configuration information */

#define CONFIGURE_MP_APPLICATION

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS               2
#define CONFIGURE_MAXIMUM_TIMERS              1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#include <rtems/confdefs.h>

/* variables */

TEST_EXTERN rtems_id   Task_id[ 4 ];     /* array of task ids */
TEST_EXTERN rtems_name Task_name[ 4 ];   /* array of task names */

TEST_EXTERN rtems_id   Timer_id[ 2 ];    /* array of timer ids */
TEST_EXTERN rtems_name Timer_name[ 2 ];  /* array of timer names */

TEST_EXTERN uint32_t    remote_node;
TEST_EXTERN rtems_id          remote_tid;

/* end of include file */
