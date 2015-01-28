/**
 * @file
 * 
 * @brief Intel I386 Interrupt Macros
 *
 * Formerly contained in and extracted from libcpu/i386/cpu.h
 */

/*
 *  COPYRIGHT (c) 1998 valette@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 *
 *  Applications must not include this file directly.
 */

#ifndef _RTEMS_SCORE_INTERRUPTS_H
#define _RTEMS_SCORE_INTERRUPTS_H

#ifndef ASM

struct 	__rtems_raw_irq_connect_data__;

typedef void (*rtems_raw_irq_hdl)		(void);
typedef void (*rtems_raw_irq_enable)		(const struct __rtems_raw_irq_connect_data__*);
typedef void (*rtems_raw_irq_disable)		(const struct __rtems_raw_irq_connect_data__*);
typedef int  (*rtems_raw_irq_is_enabled)	(const struct __rtems_raw_irq_connect_data__*);

/**
 * @name Interrupt Level Macros
 * 
 */
/**@{**/

#define i386_disable_interrupts( _level ) \
  { \
    __asm__ volatile ( "pushf ; \
                    cli ; \
                    pop %0" \
                   : "=rm" ((_level)) \
    ); \
  }

#define i386_enable_interrupts( _level )  \
  { \
    __asm__ volatile ( "push %0 ; \
                    popf" \
                    : : "rm" ((_level)) : "cc" \
    ); \
  }

#define i386_flash_interrupts( _level ) \
  { \
    __asm__ volatile ( "push %0 ; \
                    popf ; \
                    cli" \
                    : : "rm" ((_level)) : "cc" \
    ); \
  }

#define i386_get_interrupt_level( _level ) \
  do { \
    register uint32_t   _eflags; \
    \
    __asm__ volatile ( "pushf ; \
                    pop %0" \
                    : "=rm" ((_eflags)) \
    ); \
    \
    _level = (_eflags & EFLAGS_INTR_ENABLE) ? 0 : 1; \
  } while (0)

#define _CPU_ISR_Disable( _level ) i386_disable_interrupts( _level )
#define _CPU_ISR_Enable( _level ) i386_enable_interrupts( _level )

/** @} */

#endif
#endif
