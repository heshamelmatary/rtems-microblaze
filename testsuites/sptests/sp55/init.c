/**
 *  @file
 *
 *  Broadcast to a message queue with pending messages
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <tmacros.h>

const char rtems_test_name[] = "SP 55";

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);

rtems_task Init(
  rtems_task_argument ignored
)
{
  rtems_status_code    status;
  rtems_id             id;
  int                  msg = 1;
  uint32_t             count = 20;

  TEST_BEGIN();

  puts( "Init - rtems_message_queue_create - OK" );
  status = rtems_message_queue_create(
    rtems_build_name( 'Q', '1', ' ', ' ' ),
    2,
    sizeof(int),
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  directive_failed( status, "rtems_message_queue_create" );

  puts( "Init - rtems_message_queue_send - make message pending - OK" );
  status = rtems_message_queue_send( id, &msg, sizeof(msg) );
  directive_failed( status, "rtems_message_queue_send" );

  puts( "Init - rtems_message_queue_broadcast - with message pending - OK" );
  status = rtems_message_queue_broadcast( id, &msg, sizeof(msg), &count );
  directive_failed( status, "rtems_message_queue_broadcast" );
  if ( count != 0 ) {
    puts( "broadcast with message pending FAILED" );
    rtems_test_exit(0);
  }

  puts( "Init - rtems_message_queue_delete - OK" );
  status = rtems_message_queue_delete( id );
  directive_failed( status, "rtems_message_queue_delete" );

  TEST_END();
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS              1
#define CONFIGURE_MAXIMUM_MESSAGE_QUEUES     1
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_MESSAGE_BUFFER_MEMORY \
        CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE( 2, sizeof(int) )

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
