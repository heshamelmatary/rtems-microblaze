/**
 * @file
 * @ingroup uart
 * @brief APBUART via PCI - driver interface
 */

/*
 *  COPYRIGHT (c) 2007.
 *  Gaisler Research
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 *
 */

#ifndef __APBUART_PCI_H__
#define __APBUART_PCI_H__

#include <apbuart.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Register APBUART driver, if APBUART devices are found.
 * bus = pointer to AMBA bus description used to search for APBUART(s).
 *
 */

int apbuart_pci_register (struct ambapp_bus * bus);

/* This function must be called on APBUART interrupt. Called from the
 * PCI interrupt handler.
 * irq = AMBA IRQ assigned to the APBUART device, is found by reading
 *       pending register on IRQMP connected to the APBUART device.
 *
 */
void apbuartpci_interrupt_handler (int irq, void *arg);

extern void (*apbuart_pci_int_reg) (void *handler, int irq, void *arg);

#ifdef __cplusplus
}
#endif

#endif /* __APBUART_PCI_H__ */
