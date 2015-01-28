/**
 * @file
 *
 * @ingroup sparc_erc32
 *
 * @brief Global BSP Definitions.
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 *
 *  Ported to ERC32 implementation of the SPARC by On-Line Applications
 *  Research Corporation (OAR) under contract to the European Space
 *  Agency (ESA).
 *
 *  ERC32 modifications of respective RTEMS file: COPYRIGHT (c) 1995.
 *  European Space Agency.
 */

#ifndef _BSP_H
#define _BSP_H

#include <bspopts.h>
#include <bsp/default-initial-extension.h>

#include <rtems.h>
#include <rtems/iosupp.h>
#include <erc32.h>
#include <rtems/clockdrv.h>
#include <rtems/console.h>
#include <rtems/irq-extension.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup sparc_erc32 ERC32 Support
 *
 * @ingroup bsp_sparc
 *
 * @brief ERC32 Support Package
 */

/*
 *  BSP provides its own Idle thread body
 */
void *bsp_idle_thread( uintptr_t ignored );
#define BSP_IDLE_TASK_BODY bsp_idle_thread

/*
 * Network driver configuration
 */
struct rtems_bsdnet_ifconfig;
extern int rtems_erc32_sonic_driver_attach(
  struct rtems_bsdnet_ifconfig *config
);
#define RTEMS_BSP_NETWORK_DRIVER_NAME   "sonic1"
#define RTEMS_BSP_NETWORK_DRIVER_ATTACH rtems_erc32_sonic_driver_attach

/* Constants */

/*
 *  Information placed in the linkcmds file.
 */

extern int   RAM_START;
extern int   RAM_END;
extern int   RAM_SIZE;

extern int   PROM_START;
extern int   PROM_END;
extern int   PROM_SIZE;

extern int   CLOCK_SPEED;

extern int   end;        /* last address in the program */

/* functions */

rtems_isr_entry set_vector(                     /* returns old vector */
    rtems_isr_entry     handler,                /* isr routine        */
    rtems_vector_number vector,                 /* vector number      */
    int                 type                    /* RTEMS or RAW intr  */
);

void BSP_fatal_exit(uint32_t error);

void bsp_spurious_initialize( void );

/* Allocate 8-byte aligned non-freeable pre-malloc() memory. The function
 * can be called at any time. The work-area will shrink when called before
 * bsp_work_area_initialize(). malloc() is called to get memory when this
 * function is called after bsp_work_area_initialize().
 */
void *bsp_early_malloc(int size);

/* Interrupt Service Routine (ISR) pointer */
typedef void (*bsp_shared_isr)(void *arg);

/* Initializes the Shared System Interrupt service */
extern void BSP_shared_interrupt_init(void);

/* Registers a shared IRQ handler, and enable it at IRQ controller. Multiple
 * interrupt handlers may use the same IRQ number, all ISRs will be called
 * when an interrupt on that line is fired.
 *
 * Arguments
 *  irq       System IRQ number
 *  info      Optional Name of IRQ source
 *  isr       Function pointer to the ISR
 *  arg       Second argument to function isr
 */
static __inline__ int BSP_shared_interrupt_register
       (
       int irq,
       const char *info,
       bsp_shared_isr isr,
       void *arg
       )
{
       return rtems_interrupt_handler_install(irq, info,
                                       RTEMS_INTERRUPT_SHARED, isr, arg);
}

/* Unregister previously registered shared IRQ handler.
 *
 * Arguments
 *  irq       System IRQ number
 *  isr       Function pointer to the ISR
 *  arg       Second argument to function isr
 */
static __inline__ int BSP_shared_interrupt_unregister
       (
       int irq,
       bsp_shared_isr isr,
       void *arg
       )
{
       return rtems_interrupt_handler_remove(irq, isr, arg);
}

/* Clear interrupt pending on IRQ controller, this is typically done on a
 * level triggered interrupt source such as PCI to avoid taking double IRQs.
 * In such a case the interrupt source must be cleared first on LEON, before
 * acknowledging the IRQ with this function.
 *
 * Arguments
 *  irq       System IRQ number
 */
extern void BSP_shared_interrupt_clear(int irq);

/* Enable Interrupt. This function will unmask the IRQ at the interrupt
 * controller. This is normally done by _register(). Note that this will
 * affect all ISRs on this IRQ.
 *
 * Arguments
 *  irq       System IRQ number
 */
extern void BSP_shared_interrupt_unmask(int irq);

/* Disable Interrupt. This function will mask one IRQ at the interrupt
 * controller. This is normally done by _unregister().  Note that this will
 * affect all ISRs on this IRQ.
 *
 * Arguments
 *  irq         System IRQ number
 */
extern void BSP_shared_interrupt_mask(int irq);

/*
 *  Delay for the specified number of microseconds.
 */
void rtems_bsp_delay(int usecs);

/*
 * Prototypes for methods used across file boundaries
 */
void console_outbyte_polled(int  port, unsigned char ch);
int console_inbyte_nonblocking(int port);

#ifdef __cplusplus
}
#endif

#endif
