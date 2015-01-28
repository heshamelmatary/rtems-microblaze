/**
 *  @file
 *  @brief Stub printk() support
 *
 *  This file contains a stub for the required printk() support.
 *  It is NOT functional!!!
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

/*
 *  To support printk
 */

#include <rtems.h>
#include <rtems/bspIo.h>

static void BSP_output_char_f(char c)
{
  /* the character just needs to disappear */
}

BSP_output_char_function_type           BSP_output_char = BSP_output_char_f;
BSP_polling_getchar_function_type       BSP_poll_char = NULL;
