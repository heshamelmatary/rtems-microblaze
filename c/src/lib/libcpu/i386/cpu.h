/*
 * @file cpu.h
 *
 *          This file contains definitions for data structure related
 *          to Intel system programming. More information can be found
 *	    on Intel site and more precisely in the following book :
 *
 *		Pentium Processor familly
 *		Developper's Manual
 *
 *		Volume 3 : Architecture and Programming Manual
 *
 * Copyright (C) 1998  Eric Valette (valette@crf.canon.fr)
 *                     Canon Centre Recherche France.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _LIBCPU_i386_CPU_H
#define _LIBCPU_i386_CPU_H

#include <rtems/score/registers.h>

#ifndef ASM

/*
 *  Interrupt Level Macros
 */
#include <rtems/score/interrupts.h>

#include <rtems/score/basedefs.h>

/*
 *  Segment Access Routines
 *
 *  NOTE:  Unfortunately, these are still static inlines even when the
 *         "macro" implementation of the generic code is used.
 */

static __inline__ unsigned short i386_get_cs(void)
{
  register unsigned short segment = 0;

  __asm__ volatile ( "movw %%cs,%0" : "=r" (segment) : "0" (segment) );

  return segment;
}

static __inline__ unsigned short i386_get_ds(void)
{
  register unsigned short segment = 0;

  __asm__ volatile ( "movw %%ds,%0" : "=r" (segment) : "0" (segment) );

  return segment;
}

static __inline__ unsigned short i386_get_es(void)
{
  register unsigned short segment = 0;

  __asm__ volatile ( "movw %%es,%0" : "=r" (segment) : "0" (segment) );

  return segment;
}

static __inline__ unsigned short i386_get_ss(void)
{
  register unsigned short segment = 0;

  __asm__ volatile ( "movw %%ss,%0" : "=r" (segment) : "0" (segment) );

  return segment;
}

static __inline__ unsigned short i386_get_fs(void)
{
  register unsigned short segment = 0;

  __asm__ volatile ( "movw %%fs,%0" : "=r" (segment) : "0" (segment) );

  return segment;
}

static __inline__ unsigned short i386_get_gs(void)
{
  register unsigned short segment = 0;

  __asm__ volatile ( "movw %%gs,%0" : "=r" (segment) : "0" (segment) );

  return segment;
}

/*
 *  IO Port Access Routines
 */

#define i386_outport_byte( _port, _value ) \
do { register unsigned short __port  = _port; \
     register unsigned char  __value = _value; \
     \
     __asm__ volatile ( "outb %0,%1" : : "a" (__value), "d" (__port) ); \
   } while (0)

#define i386_outport_word( _port, _value ) \
do { register unsigned short __port  = _port; \
     register unsigned short __value = _value; \
     \
     __asm__ volatile ( "outw %0,%1" : : "a" (__value), "d" (__port) ); \
   } while (0)

#define i386_outport_long( _port, _value ) \
do { register unsigned short __port  = _port; \
     register unsigned int  __value = _value; \
     \
     __asm__ volatile ( "outl %0,%1" : : "a" (__value), "d" (__port) ); \
   } while (0)

#define i386_inport_byte( _port, _value ) \
do { register unsigned short __port  = _port; \
     register unsigned char  __value = 0; \
     \
     __asm__ volatile ( "inb %1,%0" : "=a" (__value) \
                                : "d"  (__port) \
                  ); \
     _value = __value; \
   } while (0)

#define i386_inport_word( _port, _value ) \
do { register unsigned short __port  = _port; \
     register unsigned short __value = 0; \
     \
     __asm__ volatile ( "inw %1,%0" : "=a" (__value) \
                                : "d"  (__port) \
                  ); \
     _value = __value; \
   } while (0)

#define i386_inport_long( _port, _value ) \
do { register unsigned short __port  = _port; \
     register unsigned int  __value = 0; \
     \
     __asm__ volatile ( "inl %1,%0" : "=a" (__value) \
                                : "d"  (__port) \
                  ); \
     _value = __value; \
   } while (0)

/*
 * Type definition for raw interrupts.
 */

typedef unsigned char  rtems_vector_offset;

typedef struct __rtems_raw_irq_connect_data__{
 /*
  * IDT vector offset (IRQ line + PC386_IRQ_VECTOR_BASE)
  */
  rtems_vector_offset		idtIndex;
  /*
   * IDT raw handler. See comment on handler properties below in function prototype.
   */
  rtems_raw_irq_hdl	   	hdl;
  /*
   * function for enabling raw interrupts. In order to be consistent
   * with the fact that the raw connexion can defined in the
   * libcpu library, this library should have no knowledge of
   * board specific hardware to manage interrupts and thus the
   * "on" routine must enable the irq both at device and PIC level.
   *
   */
    rtems_raw_irq_enable	on;
  /*
   * function for disabling raw interrupts. In order to be consistent
   * with the fact that the raw connexion can defined in the
   * libcpu library, this library should have no knowledge of
   * board specific hardware to manage interrupts and thus the
   * "on" routine must disable the irq both at device and PIC level.
   *
   */
  rtems_raw_irq_disable		off;
  /*
   * function enabling to know what interrupt may currently occur
   */
  rtems_raw_irq_is_enabled	isOn;
}rtems_raw_irq_connect_data;

typedef struct {
  /*
   * size of all the table fields (*Tbl) described below.
   */
  unsigned int	 		idtSize;
  /*
   * Default handler used when disconnecting interrupts.
   */
  rtems_raw_irq_connect_data	defaultRawEntry;
  /*
   * Table containing initials/current value.
   */
  rtems_raw_irq_connect_data*	rawIrqHdlTbl;
}rtems_raw_irq_global_settings;

#include <rtems/score/idtr.h>

/*
 * C callable function enabling to get handler currently connected to a vector
 *
 */
rtems_raw_irq_hdl get_hdl_from_vector(rtems_vector_offset);

/*
 * C callable function enabling to set up one raw idt entry
 */
extern int i386_set_idt_entry (const rtems_raw_irq_connect_data*);

/*
 * C callable function enabling to get one current raw idt entry
 */
extern int i386_get_current_idt_entry (rtems_raw_irq_connect_data*);

/*
 * C callable function enabling to remove one current raw idt entry
 */
extern int i386_delete_idt_entry (const rtems_raw_irq_connect_data*);

/*
 * C callable function enabling to init idt.
 *
 * CAUTION : this function assumes that the IDTR register
 * has been already set.
 */
extern int i386_init_idt (rtems_raw_irq_global_settings* config);

/*
 * C callable function enabling to get actual idt configuration
 */
extern int i386_get_idt_config (rtems_raw_irq_global_settings** config);


/*
 * See page 11.12 Figure 11-8.
 *
 */
/**
 * @brief describes one entry of Global/Local Descriptor Table
 */
typedef struct {
  unsigned int limit_15_0 		: 16;
  unsigned int base_address_15_0	: 16;
  unsigned int base_address_23_16	: 8;
  unsigned int type			: 4;
  unsigned int descriptor_type		: 1;
  unsigned int privilege		: 2;
  unsigned int present			: 1;
  unsigned int limit_19_16		: 4;
  unsigned int available		: 1;
  unsigned int fixed_value_bits		: 1;
  unsigned int operation_size		: 1;
  unsigned int granularity		: 1;
  unsigned int base_address_31_24	: 8;
} RTEMS_COMPILER_PACKED_ATTRIBUTE segment_descriptors;

/*
 * C callable function enabling to get easilly usable info from
 * the actual value of GDT register.
 */
extern void i386_get_info_from_GDTR (segment_descriptors** table,
                                     uint16_t* limit);
/*
 * C callable function enabling to change the value of GDT register. Must be called
 * with interrupts masked at processor level!!!.
 */
extern void i386_set_GDTR (segment_descriptors*,
                           uint16_t limit);

/**
 * @brief Allows to set a GDT entry.
 *
 * Puts global descriptor \p sd to the global descriptor table on index
 * \p segment_selector_index
 *
 * @param[in] segment_selector_index index to GDT entry
 * @param[in] sd structure to be coppied to given \p segment_selector in GDT
 * @retval  0 FAILED out of GDT range or index is 0, which is not valid
 *                   index in GDT
 * @retval  1 SUCCESS
 */
extern uint32_t i386_raw_gdt_entry (uint16_t segment_selector_index,
                               segment_descriptors* sd);

/**
 * @brief fills \p sd with provided \p base in appropriate fields of \p sd
 *
 * @param[in] base 32-bit address to be set as descriptor's base
 * @param[out] sd descriptor being filled with \p base
 */
extern void i386_fill_segment_desc_base (uint32_t base,
                                         segment_descriptors* sd);

/**
 * @brief fills \p sd with provided \p limit in appropriate fields of \p sd
 *
 * sets granularity bit if necessary
 *
 * @param[in] limit 32-bit value representing number of limit bytes
 * @param[out] sd descriptor being filled with \p limit
 */
extern void i386_fill_segment_desc_limit (uint32_t limit,
                                          segment_descriptors* sd);

/*
 * C callable function enabling to set up one raw interrupt handler
 */
extern uint32_t i386_set_gdt_entry (uint16_t segment_selector,
                                    uint32_t base,
                                    uint32_t limit);

/**
 * @brief Returns next empty descriptor in GDT.
 *
 * Number of descriptors that can be returned depends on \a GDT_SIZE
 *
 * @retval  0 FAILED GDT is full
 * @retval  <1;65535> segment_selector number as index to GDT
 */
extern uint16_t i386_next_empty_gdt_entry (void);

/**
 * @brief Copies GDT entry at index \p segment_selector to structure
 * pointed to by \p struct_to_fill
 *
 * @param[in] segment_selector index to GDT table specifying descriptor to copy
 * @param[out] struct_to_fill pointer to memory where will be descriptor coppied
 * @retval  0 FAILED segment_selector out of GDT range
 * @retval  <1;65535> retrieved segment_selector
 */
extern uint16_t i386_cpy_gdt_entry (uint16_t segment_selector,
                                    segment_descriptors* struct_to_fill);

/**
 * @brief Returns pointer to GDT table at index given by \p segment_selector
 *
 * @param[in] sgmnt_selector index to GDT table for specifying descriptor to get
 * @retval  NULL FAILED segment_selector out of GDT range
 * @retval  pointer to GDT table at \p segment_selector
 */
extern segment_descriptors* i386_get_gdt_entry (uint16_t sgmnt_selector);

/**
 * @brief Extracts base address from GDT entry pointed to by \p gdt_entry
 *
 * @param[in]  gdt_entry pointer to entry from which base should be retrieved
 * @retval base address from GDT entry
*/
RTEMS_INLINE_ROUTINE void* i386_base_gdt_entry (segment_descriptors* gdt_entry)
{
    return (void*)(gdt_entry->base_address_15_0 |
            (gdt_entry->base_address_23_16<<16) |
            (gdt_entry->base_address_31_24<<24));
}

/**
 * @brief Extracts limit in bytes from GDT entry pointed to by \p gdt_entry
 *
 * @param[in]  gdt_entry pointer to entry from which limit should be retrieved
 * @retval limit value in bytes from GDT entry
 */
extern uint32_t i386_limit_gdt_entry (segment_descriptors* gdt_entry);

/*
 * See page 11.18 Figure 11-12.
 *
 */

typedef struct {
  unsigned int offset			: 12;
  unsigned int page			: 10;
  unsigned int directory 		: 10;
}la_bits;

typedef union {
  la_bits	bits;
  unsigned int	address;
}linear_address;


/*
 * See page 11.20 Figure 11-14.
 *
 */

typedef struct {
  unsigned int present	 		: 1;
  unsigned int writable			: 1;
  unsigned int user			: 1;
  unsigned int write_through		: 1;
  unsigned int cache_disable		: 1;
  unsigned int accessed			: 1;
  unsigned int reserved1		: 1;
  unsigned int page_size		: 1;
  unsigned int reserved2		: 1;
  unsigned int available		: 3;
  unsigned int page_frame_address	: 20;
}page_dir_bits;

typedef union {
  page_dir_bits	bits;
  unsigned int	dir_entry;
}page_dir_entry;

typedef struct {
  unsigned int present	 		: 1;
  unsigned int writable			: 1;
  unsigned int user			: 1;
  unsigned int write_through		: 1;
  unsigned int cache_disable		: 1;
  unsigned int accessed			: 1;
  unsigned int dirty			: 1;
  unsigned int reserved2		: 2;
  unsigned int available		: 3;
  unsigned int page_frame_address	: 20;
}page_table_bits;

typedef union {
  page_table_bits	bits;
  unsigned int		table_entry;
} page_table_entry;

/*
 * definitions related to page table entry
 */
#define PG_SIZE 0x1000
#define MASK_OFFSET 0xFFF
#define MAX_ENTRY (PG_SIZE/sizeof(page_dir_entry))
#define FOUR_MB       0x400000
#define MASK_FLAGS 0x1A

#define PTE_PRESENT  		0x01
#define PTE_WRITABLE 		0x02
#define PTE_USER		0x04
#define PTE_WRITE_THROUGH	0x08
#define PTE_CACHE_DISABLE	0x10

typedef struct {
  page_dir_entry pageDirEntry[MAX_ENTRY];
} page_directory;

typedef struct {
  page_table_entry pageTableEntry[MAX_ENTRY];
} page_table;


/* C declaration for paging management */

extern int  	_CPU_is_cache_enabled(void);
extern int  	_CPU_is_paging_enabled(void);
extern int 	init_paging(void);
extern void 	_CPU_enable_paging(void);
extern void 	_CPU_disable_paging(void);
extern void 	_CPU_disable_cache(void);
extern void 	_CPU_enable_cache(void);
extern int 	_CPU_map_phys_address
                      (void **mappedAddress, void *physAddress,
		       int size, int flag);
extern int 	_CPU_unmap_virt_address (void *mappedAddress, int size);
extern int 	_CPU_change_memory_mapping_attribute
                         (void **newAddress, void *mappedAddress,
			  unsigned int size, unsigned int flag);
extern int  	_CPU_display_memory_attribute(void);

# endif /* ASM */

#endif
