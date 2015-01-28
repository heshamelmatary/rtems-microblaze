/**
 * @file
 * @ingroup m68k_mvme162
 * @brief C Overhead definitions
 */

/* 
 *
 *  This include file has defines to represent the overhead associated
 *  with calling a particular directive from C on this target.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef __COVERHD_h
#define __COVERHD_h

#ifdef __cplusplus
extern "C" {
#endif

#define CALLING_OVERHEAD_INITIALIZE_EXECUTIVE      2
#define CALLING_OVERHEAD_SHUTDOWN_EXECUTIVE        1
#define CALLING_OVERHEAD_TASK_CREATE               3
#define CALLING_OVERHEAD_TASK_IDENT                2
#define CALLING_OVERHEAD_TASK_START                2
#define CALLING_OVERHEAD_TASK_RESTART              2
#define CALLING_OVERHEAD_TASK_DELETE               1
#define CALLING_OVERHEAD_TASK_SUSPEND              1
#define CALLING_OVERHEAD_TASK_RESUME               2
#define CALLING_OVERHEAD_TASK_SET_PRIORITY         2
#define CALLING_OVERHEAD_TASK_MODE                 2
#define CALLING_OVERHEAD_TASK_GET_NOTE             2
#define CALLING_OVERHEAD_TASK_SET_NOTE             2
#define CALLING_OVERHEAD_TASK_WAKE_WHEN            4
#define CALLING_OVERHEAD_TASK_WAKE_AFTER           1
#define CALLING_OVERHEAD_INTERRUPT_CATCH           2
#define CALLING_OVERHEAD_CLOCK_GET                 4
#define CALLING_OVERHEAD_CLOCK_SET                 4
#define CALLING_OVERHEAD_CLOCK_TICK                1

#define CALLING_OVERHEAD_TIMER_CREATE              2
#define CALLING_OVERHEAD_TIMER_IDENT               1
#define CALLING_OVERHEAD_TIMER_DELETE              2
#define CALLING_OVERHEAD_TIMER_FIRE_AFTER          2
#define CALLING_OVERHEAD_TIMER_FIRE_WHEN           5
#define CALLING_OVERHEAD_TIMER_RESET               1
#define CALLING_OVERHEAD_TIMER_CANCEL              1
#define CALLING_OVERHEAD_SEMAPHORE_CREATE          2
#define CALLING_OVERHEAD_SEMAPHORE_IDENT           1
#define CALLING_OVERHEAD_SEMAPHORE_DELETE          2
#define CALLING_OVERHEAD_SEMAPHORE_OBTAIN          2
#define CALLING_OVERHEAD_SEMAPHORE_RELEASE         1
#define CALLING_OVERHEAD_MESSAGE_QUEUE_CREATE      2
#define CALLING_OVERHEAD_MESSAGE_QUEUE_IDENT       2
#define CALLING_OVERHEAD_MESSAGE_QUEUE_DELETE      1
#define CALLING_OVERHEAD_MESSAGE_QUEUE_SEND        2
#define CALLING_OVERHEAD_MESSAGE_QUEUE_URGENT      2
#define CALLING_OVERHEAD_MESSAGE_QUEUE_BROADCAST   2
#define CALLING_OVERHEAD_MESSAGE_QUEUE_RECEIVE     2
#define CALLING_OVERHEAD_MESSAGE_QUEUE_FLUSH       2

#define CALLING_OVERHEAD_EVENT_SEND                2
#define CALLING_OVERHEAD_EVENT_RECEIVE             2
#define CALLING_OVERHEAD_SIGNAL_CATCH              2
#define CALLING_OVERHEAD_SIGNAL_SEND               2
#define CALLING_OVERHEAD_PARTITION_CREATE          3
#define CALLING_OVERHEAD_PARTITION_IDENT           2
#define CALLING_OVERHEAD_PARTITION_DELETE          2
#define CALLING_OVERHEAD_PARTITION_GET_BUFFER      2
#define CALLING_OVERHEAD_PARTITION_RETURN_BUFFER   2
#define CALLING_OVERHEAD_REGION_CREATE             3
#define CALLING_OVERHEAD_REGION_IDENT              2
#define CALLING_OVERHEAD_REGION_DELETE             1
#define CALLING_OVERHEAD_REGION_GET_SEGMENT        3
#define CALLING_OVERHEAD_REGION_RETURN_SEGMENT     2
#define CALLING_OVERHEAD_PORT_CREATE               3
#define CALLING_OVERHEAD_PORT_IDENT                2
#define CALLING_OVERHEAD_PORT_DELETE               2
#define CALLING_OVERHEAD_PORT_EXTERNAL_TO_INTERNAL 2
#define CALLING_OVERHEAD_PORT_INTERNAL_TO_EXTERNAL 2

#define CALLING_OVERHEAD_IO_INITIALIZE             3
#define CALLING_OVERHEAD_IO_OPEN                   2
#define CALLING_OVERHEAD_IO_CLOSE                  2
#define CALLING_OVERHEAD_IO_READ                   2
#define CALLING_OVERHEAD_IO_WRITE                  2
#define CALLING_OVERHEAD_IO_CONTROL                2
#define CALLING_OVERHEAD_FATAL_ERROR_OCCURRED      1
#define CALLING_OVERHEAD_RATE_MONOTONIC_CREATE     2
#define CALLING_OVERHEAD_RATE_MONOTONIC_IDENT      2
#define CALLING_OVERHEAD_RATE_MONOTONIC_DELETE     1
#define CALLING_OVERHEAD_RATE_MONOTONIC_CANCEL     1
#define CALLING_OVERHEAD_RATE_MONOTONIC_PERIOD     2
#define CALLING_OVERHEAD_MULTIPROCESSING_ANNOUNCE  1

#ifdef __cplusplus
}
#endif

#endif
