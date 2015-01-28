/*
 *  Spurious Trap Handler Assistant
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <rtems/bspIo.h>

/*
 *  bsp_spurious_handler_assistant
 *
 *  We can't recover so just return to gdb.
 */
void bsp_spurious_handler_assistant(
  rtems_vector_number  vector
)
{
  /* XXX do something here */
}
