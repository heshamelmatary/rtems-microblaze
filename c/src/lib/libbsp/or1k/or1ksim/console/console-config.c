/**
 * @file
 *
 * @ingroup or1ksim_uart
 *
 * @brief Console Configuration.
 */

/*
 * Copyright (c) 2014 Hesham ALMatary
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE
 */

#include <rtems/bspIo.h>

#include <libchip/serial.h>

#include <bspopts.h>
#include <bsp/uart.h>
#include <bsp/or1ksim.h>

console_tbl Console_Configuration_Ports [] = {
    {
      .sDeviceName = "/dev/ttyS0",
      .deviceType = SERIAL_CUSTOM,
      .pDeviceFns = &or1ksim_uart_fns,
      .deviceProbe = NULL,
      .pDeviceFlow = NULL,
      .ulCtrlPort1 = OR1KSIM_BSP_UART_BASE,
      .ulCtrlPort2 = 0,
      .ulClock = OR1KSIM_UART_DEFAULT_BAUD,
      .ulIntVector = OR1KSIM_BSP_UART_IRQ
    }
};

#define PORT_COUNT \
  (sizeof(Console_Configuration_Ports) \
    / sizeof(Console_Configuration_Ports [0]))

unsigned long Console_Configuration_Count = PORT_COUNT;

static void output_char(char c)
{
  const console_fns *con =
    Console_Configuration_Ports [Console_Port_Minor].pDeviceFns;

  if (c == '\n') {
    con->deviceWritePolled((int) Console_Port_Minor, '\r');
  }
  con->deviceWritePolled((int) Console_Port_Minor, c);
}

BSP_output_char_function_type BSP_output_char = output_char;

BSP_polling_getchar_function_type BSP_poll_char = NULL;
