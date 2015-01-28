/*
 *  COPYRIGHT (c) 1989-2013.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <coverhd.h>
#include <errno.h>
#include <timesys.h>
#include <tmacros.h>
#include <sched.h>
#include <pthread.h>
#include <rtems/btimer.h>
#include "test_support.h"

const char rtems_test_name[] = "PSXTMRWLOCK 05";

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);
void *Middle(void *argument);
void *Low(void *argument);

pthread_rwlock_t     rwlock;
struct timespec      abstime;

void *Low(
  void *argument
)
{
  benchmark_timer_t end_time;

  /*
   * Now we have finished the thread startup overhead,
   * so let other threads run.  When we return, we can
   * finish the benchmark.
   */
  sched_yield();
    /* let other threads run */

  end_time = benchmark_timer_read();

  put_time(
    "pthread_rwlock_timedwrlock: not available blocks",
    end_time,
    OPERATION_COUNT,
    0,
    0
  );

  TEST_END();

  rtems_test_exit( 0 );
  return NULL;
}

void *Middle(
  void *argument
)
{
  int status;

  /*
   * Now we have finished the thread startup overhead,
   * so let other threads run.  When we return, we can
   * finish the benchmark.
   */
  sched_yield();
    /* let other threads run */

  /* This timed write lock operation will be blocked 
   * because the other write operation has the lock
   */
    status = pthread_rwlock_timedwrlock(&rwlock, &abstime);
  rtems_test_assert( status == 0 );
  return NULL;
}

void *POSIX_Init(
  void *argument
)
{
  int        i;
  int        status;
  pthread_t  threadId;
  pthread_rwlockattr_t attr;

  TEST_BEGIN();

  for ( i=0 ; i < OPERATION_COUNT - 1 ; i++ ) {
    status = pthread_create( &threadId, NULL, Middle, NULL );
    rtems_test_assert( !status );
  }
  
  status = pthread_create( &threadId, NULL, Low, NULL );
  rtems_test_assert( !status );

  /*
   *  Timeout for 5 seconds from now.
   */
  status = clock_gettime( CLOCK_REALTIME, &abstime );
  rtems_test_assert( !status );
  abstime.tv_sec += 5;

  /*
   * Deliberately create the rwlock after the threads.  This way if the
   * threads do run before we intend, they will get an error.
   */
  status = pthread_rwlockattr_init( &attr );
  rtems_test_assert( status == 0 );
    status = pthread_rwlock_init( &rwlock, &attr );
  rtems_test_assert( status == 0 );
  /*
   * Let the other threads start so the thread startup overhead,
   * is accounted for.  When we return, we can start the benchmark.
   */
  sched_yield();
    /* let other threads run */

  
  /* start the timer and switch through all the other tasks */
  benchmark_timer_initialize();

  /* write lock operation, this could be any write lock
   * I decided to use timedwrlock just to continue in the timed line  */
    status = pthread_rwlock_timedwrlock(&rwlock,0);
  rtems_test_assert( status == 0 );
  return NULL;
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_TIMER_DRIVER

#define CONFIGURE_MAXIMUM_POSIX_THREADS     OPERATION_COUNT + 2
#define CONFIGURE_MAXIMUM_POSIX_RWLOCKS     1
#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
  /* end of file */
