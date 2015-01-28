/*
 *  This include file contains all board IO definitions.
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _BSP_H
#define _BSP_H

#define BSP_ZERO_WORKSPACE_AUTOMATICALLY TRUE

#include <bspopts.h>
#include <bsp/default-initial-extension.h>
#include <rtems.h>
#include <rtems/console.h>
#include <libcpu/io.h>
#include <rtems/clockdrv.h>
#include <bsp/vectors.h>

#ifdef ASM
/* Definition of where to store registers in alignment handler */
#define ALIGN_REGS 0x0140

#else
#include <rtems.h>
#include <rtems/console.h>
#include <rtems/clockdrv.h>
#include <rtems/iosupp.h>

/*
 *  We no longer support the first generation board.
 */

#include <gen2.h>
#include <bsp/irq.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * The following macro calculates the Baud constant. For the Z8530 chip.
 *
 * Note: baud constant = ((clock frequency / Clock_X) / (2 * Baud Rate)) - 2
 *       for the Score603e ((10,000,000 / 16) / (2 * Baud Rate)) - 2
 */
#define _Score603e_Z8530_Baud( _frequency, _clock_by, _baud_rate  )   \
  ( (_frequency /( _clock_by * 2 * _baud_rate))  - 2)

#define Score603e_Z8530_Chip1_Baud( _value ) \
  _Score603e_Z8530_Baud( SCORE603E_85C30_1_CLOCK, \
     SCORE603E_85C30_1_CLOCK_X, _value )

#define Score603e_Z8530_Chip0_Baud( _value ) \
  _Score603e_Z8530_Baud( SCORE603E_85C30_0_CLOCK, \
     SCORE603E_85C30_0_CLOCK_X, _value )

#define Initialize_Board_ctrl_register()                         \
  *SCORE603E_BOARD_CTRL_REG = (*SCORE603E_BOARD_CTRL_REG |       \
                               SCORE603E_BRD_FLASH_DISABLE_MASK)

#define Processor_Synchronize() \
  __asm__ volatile(" eieio ")


/* Constants */

/*
 *  Device Driver Table Entries
 */

/*
 * NOTE: Use the standard Console driver entry
 */

/*
 * NOTE: Use the standard Clock driver entry
 */

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
extern int   CPU_PPC_CLICKS_PER_MS;

extern int   end;        /* last address in the program */

/*
 * Total RAM available
 */
extern int        end;        /* last address in the program */
extern int        RAM_END;
extern uint32_t   BSP_mem_size;


/*
 * How many libio files we want
 */

#define BSP_LIBIO_MAX_FDS       20

/* functions */

/*
 *
 */
rtems_isr_entry  set_EE_vector(
  rtems_isr_entry     handler,                  /* isr routine        */
  rtems_vector_number vector                    /* vector number      */
);
void initialize_external_exception_vector(void);

/*
 * Hwr_init.c
 */
void init_PCI(void);
void init_RTC(void);
void instruction_cache_enable(void);
void data_cache_enable(void);

void     initialize_PCI_bridge(void);
uint16_t read_and_clear_irq(void);
void     set_irq_mask(uint16_t value);
uint16_t get_irq_mask(void);

/*
 * universe.c
 */
void     initialize_universe(void);
void     set_irq_mask(uint16_t value);
uint16_t get_irq_mask(void);
void     unmask_irq(uint16_t irq_idx);
void     mask_irq(uint16_t irq_idx);
void     init_irq_data_register(void);
uint16_t read_and_clear_PMC_irq(uint16_t irq);
bool     Is_PMC_IRQ( uint32_t pmc_irq, uint16_t status_word);
uint16_t read_and_clear_irq(void);
void set_vme_base_address(uint32_t base_address);
uint32_t get_vme_slave_size(void);
void set_vme_slave_size (uint32_t size);

/*
 * FPGA.c
 */
void initialize_PCI_bridge(void);
void init_irq_data_register(void);
uint32_t Read_pci_device_register(uint32_t address);
void  Write_pci_device_register(uint32_t address, uint32_t data);

/* flash.c */
unsigned int SCORE603e_FLASH_Disable(uint32_t unused);
unsigned int SCORE603e_FLASH_verify_enable(void);
unsigned int SCORE603e_FLASH_Enable_writes(uint32_t area);

/*
 * PCI.c
 */
uint32_t PCI_bus_read(volatile uint32_t *_addr);
void PCI_bus_write(volatile uint32_t *_addr, uint32_t _data);

#define BSP_FLASH_ENABLE_WRITES( _area) SCORE603e_FLASH_Enable_writes( _area )
#define BSP_FLASH_DISABLE_WRITES(_area) SCORE603e_FLASH_Disable( _area )

#define Convert_Endian_32( _data ) \
  ( ((_data&0x000000ff)<<24) | ((_data&0x0000ff00)<<8) |  \
    ((_data&0x00ff0000)>>8)  | ((_data&0xff000000)>>24) )

#define Convert_Endian_16( _data ) \
  ( ((_data&0x00ff)<<8) | ((_data&0xff00)>>8) )

/*
 *  Interfaces to required Clock Driver support methods
 */
int BSP_disconnect_clock_handler(void);
int BSP_connect_clock_handler(void);

#ifdef __cplusplus
}
#endif

#endif /* ASM */

#endif
