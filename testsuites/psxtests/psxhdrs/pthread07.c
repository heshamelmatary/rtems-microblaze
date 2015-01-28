/*
 *  This test file is used to verify that the header files associated with
 *  invoking this function are correct.
 *
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>

#ifndef _POSIX_THREADS
#error "rtems is supposed to have pthread_create"
#endif

int test( void );
void *test_task( void * arg );

void *test_task(
  void * arg
)
{
  for ( ; ; )
    ;
  return NULL;
}

int test( void )
{
  pthread_t       thread;
  pthread_attr_t  attribute;
  void           *arg = NULL;
  int             result;

  result = pthread_create( &thread, &attribute, test_task, arg );

  return result;
}
