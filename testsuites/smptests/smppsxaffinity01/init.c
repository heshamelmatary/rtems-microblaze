/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define  _GNU_SOURCE

#include <tmacros.h>
#include <errno.h>
#include <pthread.h>
#include <sched.h>

const char rtems_test_name[] = "SMPPSXAFFINITY 1";

#if HAVE_DECL_PTHREAD_GETAFFINITY_NP

#define CPU_COUNT 4

pthread_t           Init_id;

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);
void Validate_attrgetaffinity_errors(void);
void Validate_attrsetaffinity_errors(void);
void Validate_attr(void);

void Validate_attrgetaffinity_errors(void)
{
  int                 sc;
  cpu_set_t           cpuset;
  pthread_attr_t      attr;

  /* Verify pthread_attr_getaffinity_np validates attr  */
  puts( "Init - pthread_attr_getaffinity_np - Invalid attr - EFAULT" );
  sc = pthread_attr_getaffinity_np( NULL, sizeof(cpu_set_t), &cpuset );
  rtems_test_assert( sc == EFAULT );

  /* Verify pthread_attr_getaffinity_np validates cpuset */
  puts( "Init - pthread_attr_getaffinity_np - Invalid attr - EFAULT" );
  sc = pthread_attr_getaffinity_np( &attr, sizeof(cpu_set_t), NULL );
  rtems_test_assert( sc == EFAULT );

  /* Verify pthread_attr_getaffinity_np validates cpusetsize */
  puts( "Init - pthread_attr_getaffinity_np - Invalid cpusetsize - EINVAL" );
  sc = pthread_attr_getaffinity_np( &attr, sizeof(cpu_set_t) * 2 , &cpuset );
  rtems_test_assert( sc == EINVAL );

}

void Validate_attrsetaffinity_errors(void)
{
  int                 sc;
  cpu_set_t           cpuset;
  pthread_attr_t      attr;

  /* Verify pthread_attr_setaffinity_np validates attr.  */
  puts( "Init - pthread_attr_setaffinity_np - Invalid attr - EFAULT" );
  sc = pthread_attr_setaffinity_np( NULL, sizeof(cpu_set_t), &cpuset );
  rtems_test_assert( sc == EFAULT );

  /* Verify pthread_attr_setaffinity_np validates cpuset    */
  puts( "Init - pthread_attr_setaffinity_np - Invalid attr - EFAULT" );
  sc = pthread_attr_setaffinity_np( &attr, sizeof(cpu_set_t), NULL );
  rtems_test_assert( sc == EFAULT );

  /* Verify pthread_attr_setaffinity_np validates cpusetsize */
  puts( "Init - pthread_attr_setaffinity_np - Invalid cpusetsize - EINVAL" );
  sc = pthread_attr_setaffinity_np( &attr, sizeof(cpu_set_t) * 2 , &cpuset );
  rtems_test_assert( sc == EINVAL );

  /* Verify pthread_attr_setaffinity_np validates cpuset greater than 0  */
  CPU_ZERO(&cpuset);
  puts( "Init - pthread_attr_setaffinity_np - No cpus in cpuset - EINVAL" );
  sc = pthread_attr_setaffinity_np( &attr, sizeof(cpu_set_t) , &cpuset );
  rtems_test_assert( sc == EINVAL );

  /* Verify pthread_attr_setaffinity_np validates invalid cpu in cpuset */
  CPU_FILL(&cpuset);
  puts( "Init - pthread_attr_setaffinity_np - Too many cpus in cpuset - EINVAL" );
  sc = pthread_attr_setaffinity_np( &attr, sizeof(cpu_set_t) , &cpuset );
  rtems_test_assert( sc == EINVAL );
}

void Validate_attr(void )
{
  int                 sc;
  pthread_attr_t      attr;
  uint32_t            cpus;
  cpu_set_t           cpuset1;
  cpu_set_t           cpuset2;
  int                 i;
  int                 priority;

  sc = pthread_getattr_np( Init_id, &attr );
  rtems_test_assert( sc == 0 );

  priority = sched_get_priority_min( SCHED_FIFO );
  rtems_test_assert( priority != -1 );


  cpus = rtems_get_processor_count();
  puts(
    "Init - Validate pthread_attr_setaffinity_np and "
    "pthread_attr_getaffinity_np"
  );

  /* Set each cpu seperately in the affinity set */
  for ( i=0; i<cpus; i++ ){
    CPU_ZERO(&cpuset1);
    CPU_SET(i, &cpuset1);

    sc = pthread_attr_setaffinity_np( &attr, sizeof(cpu_set_t), &cpuset1 );
    rtems_test_assert( sc == 0 );

    sc = pthread_attr_getaffinity_np( &attr, sizeof(cpu_set_t), &cpuset2 );
    rtems_test_assert( sc == 0 );

    rtems_test_assert( CPU_EQUAL(&cpuset1, &cpuset2) );
  }
}

void *POSIX_Init(
  void *ignored
)
{
  TEST_BEGIN();

  /* Initialize thread id */
  Init_id = pthread_self();

  Validate_attrsetaffinity_errors();
  Validate_attrgetaffinity_errors();
  Validate_attr();

  TEST_END();
  rtems_test_exit(0);
}

#else
void *POSIX_Init(
  void *ignored
)
{
  TEST_BEGIN();
  puts( " POSIX Affinity Methods NOT Supported");
  TEST_END();
  rtems_test_exit(0);
}

#endif
/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_SMP_APPLICATION

#define CONFIGURE_SMP_MAXIMUM_PROCESSORS CPU_COUNT

#define CONFIGURE_SCHEDULER_PRIORITY_AFFINITY_SMP

#define CONFIGURE_MAXIMUM_POSIX_THREADS  1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
