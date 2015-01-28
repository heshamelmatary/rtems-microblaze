/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id: console-io.c,v 1.2 2011/01/31 17:41:09 joel Exp $
 */

#include <bsp.h>
#include <rtems/libio.h>
#include <stdlib.h>
#include <assert.h>
#include <reent.h>

/*
 *  console_initialize_hardware
 *
 *  This routine initializes the console hardware.
 */
void console_initialize_hardware(void)
{
}

/*
 *  console_outbyte_polled
 *
 *  This routine transmits a character using polling.
 */
void console_outbyte_polled(
  int  port,
  char ch
)
{
  unsigned int *stat = (unsigned int *)0x40600008;
  unsigned int *tx = (unsigned int *)0x40600004;

  while ( *stat )
    ;
  *tx = ch;
}

/*
 *  console_inbyte_nonblocking
 *
 *  This routine polls for a character.
 */

int console_inbyte_nonblocking(
  int port
)
{
  return -1;
}

#include <rtems/bspIo.h>

void console_output_char(char c) { console_outbyte_polled( 0, c ); }

BSP_output_char_function_type           BSP_output_char = console_output_char;
BSP_polling_getchar_function_type       BSP_poll_char = NULL;
