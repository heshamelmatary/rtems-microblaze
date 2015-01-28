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

#ifndef _POSIX_THREAD_PRIORITY_SCHEDULING
#error "RTEMS is supposed to have pthread_attr_getscope"
#endif

int test( void );

int test( void )
{
  pthread_attr_t  attr;
  int             contentionscope;
  int             result;

  result = pthread_attr_getscope( &attr, &contentionscope );

  return result;
}
