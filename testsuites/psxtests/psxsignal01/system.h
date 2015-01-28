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

/* functions */

#include <pmacros.h>

void *POSIX_Init(
  void *argument
);

void *Task_1(
  void *argument
);

void *Task_2(
  void *argument
);

void *Task_3(
  void *argument
);


/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_MAXIMUM_POSIX_THREADS        4
#define CONFIGURE_MAXIMUM_POSIX_QUEUED_SIGNALS 5

#define CONFIGURE_POSIX_INIT_THREAD_TABLE
#define CONFIGURE_POSIX_INIT_THREAD_STACK_SIZE \
        (RTEMS_MINIMUM_STACK_SIZE * 4)

#define CONFIGURE_MAXIMUM_TIMERS        1

#include <rtems/confdefs.h>

/* global variables */

#ifdef CONFIGURE_INIT
#define TEST_EXTERN
#else
#define TEST_EXTERN extern
#endif
TEST_EXTERN rtems_id   Timer_id[ 1 ];     /* array of timer ids */
TEST_EXTERN rtems_name Timer_name[ 1 ];   /* array of timer names */

TEST_EXTERN pthread_t        Init_id;
TEST_EXTERN pthread_t        Task1_id;
TEST_EXTERN pthread_t        Task2_id;
TEST_EXTERN pthread_t        Task3_id;

/* end of include file */
