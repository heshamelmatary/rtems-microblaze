/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/console-polled.h>
#include <rtems/libio.h>
#include <bsp/syscall.h>

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
  TRAP0(SYS_write, 1, &ch, 1);
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
  char ch;
  int  rc;

  rc = TRAP0 (SYS_read, 0, &ch, 1);

  if ( rc != 1 )
    return -1;
  return ch;
}

#include <rtems/bspIo.h>

static void console_output_char(char c) { console_outbyte_polled( 0, c ); }

BSP_output_char_function_type           BSP_output_char = console_output_char;
BSP_polling_getchar_function_type       BSP_poll_char = NULL;
