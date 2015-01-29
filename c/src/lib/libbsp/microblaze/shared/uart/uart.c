/******************************************************************************
*
* (c) Copyright 2002-2013 Xilinx, Inc. All rights reserved.
*
* This file contains confidential and proprietary information of Xilinx, Inc.
* and is protected under U.S. and international copyright and other
* intellectual property laws.
*
* DISCLAIMER
* This disclaimer is not a license and does not grant any rights to the
* materials distributed herewith. Except as otherwise provided in a valid
* license issued to you by Xilinx, and to the maximum extent permitted by
* applicable law: (1) THESE MATERIALS ARE MADE AVAILABLE "AS IS" AND WITH ALL
* FAULTS, AND XILINX HEREBY DISCLAIMS ALL WARRANTIES AND CONDITIONS, EXPRESS,
* IMPLIED, OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
* MERCHANTABILITY, NON-INFRINGEMENT, OR FITNESS FOR ANY PARTICULAR PURPOSE;
* and (2) Xilinx shall not be liable (whether in contract or tort, including
* negligence, or under any other theory of liability) for any loss or damage
* of any kind or nature related to, arising under or in connection with these
* materials, including for any direct, or any indirect, special, incidental,
* or consequential loss or damage (including loss of data, profits, goodwill,
* or any type of loss or damage suffered as a result of any action brought by
* a third party) even if such damage or loss was reasonably foreseeable or
* Xilinx had been advised of the possibility of the same.
*
* CRITICAL APPLICATIONS
* Xilinx products are not designed or intended to be fail-safe, or for use in
* any application requiring fail-safe performance, such as life-support or
* safety devices or systems, Class III medical devices, nuclear facilities,
* applications related to the deployment of airbags, or any other applications
* that could lead to death, personal injury, or severe property or
* environmental damage (individually and collectively, "Critical
* Applications"). Customer assumes the sole risk and liability of any use of
* Xilinx products in Critical Applications, subject only to applicable laws
* and regulations governing limitations on product liability.
*
* THIS COPYRIGHT NOTICE AND DISCLAIMER MUST BE RETAINED AS PART OF THIS FILE
* AT ALL TIMES.
*
******************************************************************************/
/****************************************************************************/
/**
*
* @file xuartlite_l.c
*
* This file contains low-level driver functions that can be used to access the
* device.  The user should refer to the hardware device specification for more
* details of the device operation.

* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- -----------------------------------------------
* 1.00b rpm  04/25/02 First release
* 1.12a rpm  07/16/07 Fixed arg type for RecvByte
* 2.00a ktn  10/20/09 The macros have been renamed to remove _m from the name.
* </pre>
*
******************************************************************************/

/***************************** Include Files *********************************/

#include <libchip/sersupp.h>
#include <bsp.h>
//#include <bsp/irq.h>
#include <bsp/uart.h>
#include <rtems/score/isr.h>

/************************** Constant Definitions *****************************/


/**************************** Type Definitions *******************************/


/***************** Macros (Inline Functions) Definitions *********************/


/************************** Function Prototypes ******************************/


/************************** Variable Prototypes ******************************/


/****************************************************************************/

void XUartLite_Initialize(int minor)
{

	/* Write to the control register to disable the interrupts, don't
	 * reset the FIFOs are the user may want the data that's present
	 */
	XUartLite_WriteReg(UART_BASEADDRESS, XUL_CONTROL_REG_OFFSET, 0);
}

/**
*
* This functions sends a single byte using the UART. It is blocking in that it
* waits for the transmitter to become non-full before it writes the byte to
* the transmit register.
*
* @param	BaseAddress is the base address of the device
* @param	Data is the byte of data to send
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void XUartLite_SendByte(int minor, char Data)
{
	while (XUartLite_IsTransmitFull(UART_BASEADDRESS));

	XUartLite_WriteReg(UART_BASEADDRESS, XUL_TX_FIFO_OFFSET, Data);
}


/****************************************************************************/
/**
*
* This functions receives a single byte using the UART. It is blocking in that
* it waits for the receiver to become non-empty before it reads from the
* receive register.
*
* @param	BaseAddress is the base address of the device
*
* @return	The byte of data received.
*
* @note		None.
*bsp/irq.h:
******************************************************************************/
char XUartLite_RecvByte(int minor)
{
	while (XUartLite_IsReceiveEmpty(UART_BASEADDRESS));

	return (char)XUartLite_ReadReg(UART_BASEADDRESS, XUL_RX_FIFO_OFFSET);
}

static ssize_t uart_write(
  int minor,
  const char *s,
  size_t n
)
{
  ssize_t i = 0;

  for (i = 0; i < n; ++i){
    XUartLite_SendByte(minor, s [i]);
  }

  return n;
}

static int uart_set_attributes(int minor, const struct termios *term)
{
  return -1;
}

static int uart_first_open(int major, int minor, void *arg)
{
  rtems_libio_open_close_args_t *oc = (rtems_libio_open_close_args_t *) arg;
  struct rtems_termios_tty *tty = (struct rtems_termios_tty *) oc->iop->data1;
  const console_tbl *ct = Console_Port_Tbl [minor];
  console_data *cd = &Console_Port_Data [minor];

  cd->termios_data = tty;
  rtems_termios_set_initial_baud(tty, ct->ulClock);

  return 0;
}

static int uart_last_close(int major, int minor, void *arg)
{
  return 0;
}

const console_fns microblaze_uart_fns = {
  .deviceProbe = libchip_serial_default_probe,
  .deviceFirstOpen = uart_first_open,
  .deviceLastClose = uart_last_close,
  .deviceRead = XUartLite_RecvByte,
  .deviceWrite = uart_write,
  .deviceInitialize = XUartLite_Initialize,
  .deviceWritePolled = XUartLite_SendByte,
  .deviceSetAttributes = uart_set_attributes,
  .deviceOutputUsesInterrupts = false
};
