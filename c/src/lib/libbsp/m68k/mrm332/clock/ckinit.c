/*
 *  This routine initailizes the periodic interrupt timer on
 *  the Motorola 68332.
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <stdlib.h>
#include <bsp.h>
#include <mrm332.h>
#include <rtems/m68k/sim.h>

#define CLOCK_VECTOR   MRM_PIV

uint32_t                Clock_isrs;         /* ISRs until next tick */
volatile uint32_t       Clock_driver_ticks; /* ticks since initialization */
static rtems_isr_entry  Old_ticker;

void Clock_exit( void );

static rtems_isr Clock_isr(rtems_vector_number vector)
{
  Clock_driver_ticks += 1;

  if ( Clock_isrs == 1 ) {
    rtems_clock_tick();
    Clock_isrs = rtems_configuration_get_microseconds_per_tick() / 1000;
  }
  else
    Clock_isrs -= 1;
}

static void Install_clock(rtems_isr_entry clock_isr)
{
  Clock_driver_ticks = 0;
  Clock_isrs = rtems_configuration_get_microseconds_per_tick() / 1000;

  Old_ticker = (rtems_isr_entry) set_vector( clock_isr, CLOCK_VECTOR, 1 );

  /* enable 1mS interrupts */
  *PITR = (unsigned short int)( SAM(0x09,0,PITM) );/* load counter */
  *PICR = (unsigned short int)                     /* enable interrupt */
    ( SAM(ISRL_PIT,8,PIRQL) | SAM(CLOCK_VECTOR,0,PIV) );

  atexit( Clock_exit );
}

void Clock_exit( void )
{
  /* shutdown the periodic interrupt */
  *PICR = (unsigned short int)
    ( SAM(0,8,PIRQL) | SAM(CLOCK_VECTOR,0,PIV) );
  /*     ^^ zero disables interrupt */

  /* do not restore old vector */
}

rtems_device_driver Clock_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *pargp
)
{
  Install_clock( Clock_isr );

  return RTEMS_SUCCESSFUL;
}
