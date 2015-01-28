/*
 *  This file contains the TTY driver for the serial ports on the LEON.
 *
 *  This driver uses the termios pseudo driver.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Modified for LEON3 BSP.
 *  COPYRIGHT (c) 2011.
 *  Aeroflex Gaisler.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <leon.h>
#include <rtems/libio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <apbuart.h>
#include <apbuart_termios.h>

int debug_uart_index __attribute__((weak)) = 0;
static struct apbuart_regs *dbg_uart = NULL;

/* Before UART driver has registered (or when no UART is available), calls to
 * printk that gets to bsp_out_char() will be filling data into the
 * pre_printk_dbgbuf[] buffer, hopefully the buffer can help debugging the
 * early BSP boot.. At least the last printk() will be caught.
 */
static char pre_printk_dbgbuf[32] = {0};
static int pre_printk_pos = 0;

/* Initialize the BSP system debug console layer. It will scan AMBA Plu&Play
 * for a debug APBUART and enable RX/TX for that UART.
 */
void bsp_debug_uart_init(void)
{
  int i;
  struct ambapp_dev *adev;
  struct ambapp_apb_info *apb;

  /* Update debug_uart_index to index used as debug console.
   * Let user select Debug console by setting debug_uart_index. If the
   * BSP is to provide the default UART (debug_uart_index==0):
   *   non-MP: APBUART[0] is debug console
   *   MP: LEON CPU index select UART
   */
  if (debug_uart_index == 0) {
#if defined(RTEMS_MULTIPROCESSING)
    debug_uart_index = LEON3_Cpu_Index;
#else
    debug_uart_index = 0;
#endif
  } else {
    debug_uart_index = debug_uart_index - 1; /* User selected dbg-console */
  }

  /* Find APBUART core for System Debug Console */
  i = debug_uart_index;
  adev = (void *)ambapp_for_each(&ambapp_plb, (OPTIONS_ALL|OPTIONS_APB_SLVS),
                                 VENDOR_GAISLER, GAISLER_APBUART,
                                 ambapp_find_by_idx, (void *)&i);
  if (adev) {
    /* Found a matching debug console, initialize debug uart if present
     * for printk
     */
    apb = (struct ambapp_apb_info *)adev->devinfo;
    dbg_uart = (struct apbuart_regs *)apb->start;
    dbg_uart->ctrl |= APBUART_CTRL_RE | APBUART_CTRL_TE;
    dbg_uart->status = 0;
  }
}

/* putchar/getchar for printk */
static void bsp_out_char(char c)
{
  if (dbg_uart == NULL) {
    /* Local debug buffer when UART driver has not registered */
    pre_printk_dbgbuf[pre_printk_pos++] = c;
    pre_printk_pos = pre_printk_pos & (sizeof(pre_printk_dbgbuf)-1);
    return;
  }

  apbuart_outbyte_polled(dbg_uart, c, 1, 1);
}

/*
 *  To support printk
 */

#include <rtems/bspIo.h>

BSP_output_char_function_type BSP_output_char = bsp_out_char;

static int bsp_in_char(void)
{
  int tmp;

  if (dbg_uart == NULL)
    return EOF;

  while ((tmp = apbuart_inbyte_nonblocking(dbg_uart)) < 0)
    ;
  return tmp;
}

BSP_polling_getchar_function_type BSP_poll_char = bsp_in_char;
