/*  bspstart.c for TLL6527M
 *
 *  This routine does the bulk of the system initialization.
 */

/*
 * COPYRIGHT (c) 2010 by ECE Northeastern University.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license
 */

#include <bsp.h>
#include <bsp/bootcard.h>
#include <cplb.h>
#include <bsp/interrupt.h>
#include <libcpu/ebiuRegs.h>

const unsigned int dcplbs_table[16][2] = {  
  { 0xFFA00000, (PAGE_SIZE_1MB | CPLB_D_PAGE_MGMT | CPLB_WT) },
  { 0xFF900000, (PAGE_SIZE_1MB | CPLB_D_PAGE_MGMT | CPLB_WT) },/* L1 Data B */
  { 0xFF800000, (PAGE_SIZE_1MB | CPLB_D_PAGE_MGMT | CPLB_WT) },/* L1 Data A */
  { 0xFFB00000, (PAGE_SIZE_1MB | CPLB_DNOCACHE) },

  { 0x20300000, (PAGE_SIZE_1MB | CPLB_DNOCACHE) },/* Async Memory Bank 3 */
  { 0x20200000, (PAGE_SIZE_1MB | CPLB_DNOCACHE) },/* Async Memory Bank 2  */
  { 0x20100000, (PAGE_SIZE_1MB | CPLB_DNOCACHE) },/* Async Memory Bank 1 */
  { 0x20000000, (PAGE_SIZE_1MB | CPLB_DNOCACHE) }, /* Async Memory Bank 0 */

  { 0x02400000, (PAGE_SIZE_4MB | CPLB_DNOCACHE) },
  { 0x02000000, (PAGE_SIZE_4MB | CPLB_DNOCACHE) },
  { 0x00C00000, (PAGE_SIZE_4MB | CPLB_DNOCACHE) },
  { 0x00800000, (PAGE_SIZE_4MB | CPLB_DNOCACHE) },
  { 0x00400000, (PAGE_SIZE_4MB | CPLB_DNOCACHE) },
  { 0x00000000, (PAGE_SIZE_4MB | CPLB_DNOCACHE) },

  { 0xffffffff, 0xffffffff }/* end of section - termination */
};


const unsigned int _icplbs_table[16][2] = { 
  { 0xFFA00000, (PAGE_SIZE_1MB | CPLB_I_PAGE_MGMT | CPLB_I_PAGE_MGMT | 0x4) },
  /* L1 Code */
  { 0xEF000000, (PAGE_SIZE_1MB | CPLB_INOCACHE) }, /* AREA DE BOOT */
  { 0xFFB00000, (PAGE_SIZE_1MB | CPLB_INOCACHE) },

  { 0x20300000, (PAGE_SIZE_1MB | CPLB_INOCACHE) },/* Async Memory Bank 3 */
  { 0x20200000, (PAGE_SIZE_1MB | CPLB_INOCACHE) },/* Async Bank 2 (Secnd) */
  { 0x20100000, (PAGE_SIZE_1MB | CPLB_INOCACHE) },/* Async Bank 1 (Prim B) */
  { 0x20000000, (PAGE_SIZE_1MB | CPLB_INOCACHE) },/* Async Bank 0 (Prim A) */

  { 0x02400000, (PAGE_SIZE_4MB | CPLB_INOCACHE) },
  { 0x02000000, (PAGE_SIZE_4MB | CPLB_INOCACHE) },
  { 0x00C00000, (PAGE_SIZE_4MB | CPLB_INOCACHE) },
  { 0x00800000, (PAGE_SIZE_4MB | CPLB_INOCACHE) },
  { 0x00400000, (PAGE_SIZE_4MB | CPLB_INOCACHE) },
  { 0x00000000, (PAGE_SIZE_4MB | CPLB_INOCACHE) },

  { 0xffffffff, 0xffffffff }/* end of section - termination */
};

/*
 * Init_PLL
 *
 * Routine to initialize the PLL. The TLL6527M uses a 25 Mhz XTAL.
 */
static void Init_PLL (void)
{
  unsigned short msel = 0;
  unsigned short ssel = 0;

  msel = (unsigned short)( (float)CCLK/(float)CLKIN );
  ssel = (unsigned short)( (float)(CLKIN*msel)/(float)SCLK);
  
  asm("cli r0;");

  *((uint32_t*)SIC_IWR) = 0x1;

  /* Configure PLL registers */
  *((uint16_t*)PLL_DIV) = ssel;
  msel = msel<<9;
  *((uint16_t*)PLL_CTL) = msel;

  /* Commands to set PLL values */
  asm("idle;");
  asm("sti r0;");
}

/*
 * Init_EBIU
 *
 * Configure extern memory
 */
static void Init_EBIU (void)
{
  /* Check if SDRAM is already enabled */
  if ( 0 != (*(uint16_t *)EBIU_SDSTAT & EBIU_SDSTAT_SDRS) ){
    asm("ssync;");
    /* RDIV = (100MHz*64ms)/8192-(6+3)=0x406 cycles */
    *(uint16_t *)EBIU_SDRRC  = 0x3F6; /* SHould have been 0x306*/
    *(uint16_t *)EBIU_SDBCTL = EBIU_SDBCTL_EBCAW_10 | EBIU_SDBCTL_EBSZ_64M |
        EBIU_SDBCTL_EBE;
    *(uint32_t *)EBIU_SDGCTL = 0x8491998d;
    asm("ssync;");
  } else {
    /* SDRAm is already programmed */
  }
}

/*
 * Init_Flags
 *
 * Enable LEDs port
 */
static void Init_Flags(void)
{
  *((uint16_t*)PORTH_FER)    = 0x0;
  *((uint16_t*)PORTH_MUX)    = 0x0;
  *((uint16_t*)PORTHIO_DIR)  = 0x1<<15;
  *((uint16_t*)PORTHIO_SET)  = 0x1<<15;
}

/*
 *  bsp_pretasking_hook
 */
void bsp_pretasking_hook(void)
{
  bfin_interrupt_init();
}

void bsp_start( void )
{
  int i;

  /* BSP Hardware Initialization*/
  Init_RTC();   /* Blackfin Real Time Clock initialization */
  Init_PLL();   /* PLL initialization */
  Init_EBIU();  /* EBIU initialization */
  Init_Flags(); /* GPIO initialization */

  /*
   *  Allocate the memory for the RTEMS Work Space.  This can come from
   *  a variety of places: hard coded address, malloc'ed from outside
   *  RTEMS world (e.g. simulator or primitive memory manager), or (as
   *  typically done by stock BSPs) by subtracting the required amount
   *  of work space from the last physical address on the CPU board.
   */
  for (i=5;i<16;i++) {
    set_vector((rtems_isr_entry)bfin_null_isr, i, 1);
  }

}
