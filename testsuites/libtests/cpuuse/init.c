/*
 *  This is a clone of sp04 which has been modified to use the cpu monitoring
 *  library.
 *
 *  Input parameters:
 *    argument - task argument
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define CONFIGURE_INIT
#include "system.h"

const char rtems_test_name[] = "CPUUSE";

rtems_extensions_table Extensions = {
  NULL,                      /* task create user extension */
  NULL,                      /* task start user extension */
  NULL,                      /* task restart user extension */
  NULL,                      /* task delete user extension */
  Task_switch,               /* task switch user extension */
  NULL,                      /* task begin user extension */
  NULL,                      /* task exitted user extension */
  NULL                       /* fatal error user extension */
};

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_status_code status;
  rtems_time_of_day time;

  TEST_BEGIN();
  build_time( &time, 12, 31, 1988, 9, 15, 0, 0 );

  status = rtems_clock_set( &time );
  directive_failed( status, "rtems_clock_set" );

  Extension_name[ 1 ] =  rtems_build_name( 'E', 'X', 'T', ' ' );

  status = rtems_extension_create(
    Extension_name[ 1 ],
    &Extensions,
    &Extension_id[ 1 ]
  );
  directive_failed( status, "rtems_extension_create" );

  Task_name[ 1 ] =  rtems_build_name( 'T', 'A', '1', ' ' );
  Task_name[ 2 ] =  rtems_build_name( 'T', 'A', '2', ' ' );
  Task_name[ 3 ] =  rtems_build_name( 'T', 'A', '3', ' ' );

  Run_count[ 1 ] = 0;
  Run_count[ 2 ] = 0;
  Run_count[ 3 ] = 0;

  status = rtems_task_create(
     Task_name[ 1 ],
     1,
     RTEMS_MINIMUM_STACK_SIZE,
     RTEMS_TIMESLICE,
     RTEMS_FLOATING_POINT,
     &Task_id[ 1 ]
  );
  directive_failed( status, "rtems_task_create of TA1" );

  status = rtems_task_create(
     Task_name[ 2 ],
     1,
     RTEMS_MINIMUM_STACK_SIZE,
     RTEMS_TIMESLICE,
     RTEMS_FLOATING_POINT,
     &Task_id[ 2 ]
  );
  directive_failed( status, "rtems_task_create of TA2" );

  status = rtems_task_create(
     Task_name[ 3 ],
     1,
     RTEMS_MINIMUM_STACK_SIZE,
     RTEMS_TIMESLICE,
     RTEMS_FLOATING_POINT,
     &Task_id[ 3 ]
  );
  directive_failed( status, "rtems_task_create of TA3" );

  status = rtems_task_start( Task_id[ 1 ], Task_1, 0 );
  directive_failed( status, "rtems_task_start of TA1" );

  status = rtems_task_start( Task_id[ 2 ], Task_2, 0 );
  directive_failed( status, "rtems_task_start of TA2" );

  status = rtems_task_start( Task_id[ 3 ], Task_3, 0 );
  directive_failed( status, "rtems_task_start of TA3" );

  /*
   * We suspend the Init task rather than delete it so it still
   * shows up in CPU Usage Report.  If we don't, the CPU Usage
   * times will not add up to the time since last CPU Usage Reset.
   */
  status = rtems_task_suspend( RTEMS_SELF );
  directive_failed( status, "rtems_task_delete of RTEMS_SELF" );
}
