/**
 * @file score/src/ts64addto.c
 *
 * @brief Add to a Timestamp
 * @ingroup SuperCore
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/timestamp.h>

#if CPU_TIMESTAMP_USE_INT64 == TRUE
void _Timestamp64_Add_to(
  Timestamp64_Control       *_time,
  const Timestamp64_Control *_add
)
{
  _Timestamp64_implementation_Add_to( _time, _add );
}
#endif
