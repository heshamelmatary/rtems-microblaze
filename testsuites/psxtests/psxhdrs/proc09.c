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

#include <sys/types.h>
#include <unistd.h>

int test( void );

int test( void )
{
  gid_t grouplist[ 20 ];
  int   gidsetsize;
  int   result;

  gidsetsize = 20;

  result = getgroups( gidsetsize, grouplist );

  return result;
}
