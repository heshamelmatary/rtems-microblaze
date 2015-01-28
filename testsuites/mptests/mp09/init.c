/*  Init
 *
 *  This routine is the initialization task for this test program.
 *  It is a user initialization task and has the responsibility for creating
 *  and starting the tasks that make up the test.  If the time of day
 *  clock is required for the test, it should also be set to a known
 *  value by this function.
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

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_status_code status;

  printf(
    "\n\n*** TEST 9 -- NODE %" PRId32 " ***\n",
    Multiprocessing_configuration.node
  );

  Task_name[ 1 ] = rtems_build_name( '1', '1', '1', ' ' );
  Task_name[ 2 ] = rtems_build_name( '2', '2', '2', ' ' );

  Queue_name[ 1 ] = rtems_build_name( 'M', 'S', 'G', ' ' );

  if ( Multiprocessing_configuration.node == 1 ) {
    puts( "Creating Message Queue (Global)" );
    status = rtems_message_queue_create(
      Queue_name[ 1 ],
      3,
      16,
      RTEMS_GLOBAL,
      &Queue_id[ 1 ]
    );
    directive_failed( status, "rtems_message_queue_create" );
  }

  puts( "Creating Test_task (local)" );
  status = rtems_task_create(
    Task_name[Multiprocessing_configuration.node],
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_TIMESLICE,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[ 1 ]
  );
  directive_failed( status, "rtems_task_create" );

  puts( "Starting Test_task (local)" );
  status = rtems_task_start( Task_id[ 1 ], Test_task, 0 );
  directive_failed( status, "rtems_task_start" );

  puts( "Deleting initialization task" );
  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "rtems_task_delete of RTEMS_SELF" );
}
