/*  bsp.h
 *
 *  This include file contains all mrm board IO definitions.
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _BSP_H
#define _BSP_H

#include <bspopts.h>
#include <bsp/default-initial-extension.h>

#include <rtems.h>
#include <rtems/bspIo.h>
#include <rtems/clockdrv.h>
#include <rtems/console.h>
#include <rtems/iosupp.h>
#include <mrm332.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CONSOLE_SCI

/* externals */

extern char _etext[];
extern char _copy_start[];
extern char _edata[];
extern char _clear_start[];
extern char end[];
extern bool _copy_data_from_rom;

/* constants */

#ifdef __START_C__
#define STACK_SIZE "#0x800"
#else
#define STACK_SIZE 0x800
#endif

/* macros */

#define RAW_PUTS(str) \
  { register char *ptr = str; \
    while (*ptr) SCI_output_char(*ptr++); \
  }

#define RAW_PUTI(n) { \
    register int i, j; \
    \
    RAW_PUTS("0x"); \
    for (i=28;i>=0;i -= 4) { \
      j = (n>>i) & 0xf; \
      SCI_output_char( (j>9 ? j-10+'a' : j+'0') ); \
    } \
  }

/* miscellaneous stuff assumed to exist */

extern rtems_isr_entry M68Kvec[];   /* vector table address */

extern int stack_size;
extern int stack_start;

/* functions */

rtems_isr_entry set_vector(
  rtems_isr_entry     handler,
  rtems_vector_number vector,
  int                 type
);

void Spurious_Initialize(void);

void _UART_flush(void);

void outbyte(char);

#ifdef __cplusplus
}
#endif

#endif
