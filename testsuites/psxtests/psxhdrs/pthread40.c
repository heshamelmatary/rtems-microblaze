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

#if HAVE_DECL_PTHREAD_ATTR_GETGUARDSIZE
#include <pthread.h>

#ifndef _POSIX_THREADS
#error "rtems is supposed to have pthread_getstacksize"
#endif

int test( void );

int test( void )
{
  pthread_attr_t  attribute;
  size_t          size;
  int             result;

  result = pthread_attr_getguardsize( &attribute, &size );

  return result;
}
#endif
