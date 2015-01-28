/*
 * ATTENTION: As MC68349 has no built-in Timer, the following code doesn't work
 *            in a MC68349. You can't use FIFO full mode for the moment, but
 *            it should be easy to fix this by using an external timer.
 *
 * Use TIMER 1 for TIMEOUT when using FIFO FULL mode in UART driver
 * Use TIMER 2 for timing test suites
 *
 *  NOTE: It is important that the timer start/stop overhead be
 *        determined when porting or modifying this code.
 */

/*
 * Geoffroy Montel
 * France Telecom - CNET/DSM/TAM/CAT
 * 4, rue du Clos Courtel
 * 35512 CESSON-SEVIGNE
 * FRANCE
 *
 * e-mail: g_montel@yahoo.com
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <rtems.h>
#include <bsp.h>
#include <rtems/btimer.h>
#include <m340uart.h>
#include <m340timer.h>
#include <m68340.h>

#define TIMER1_VECTOR 122
#define TIMER1_IRQ_LEVEL 5
#define TIMER1_INTERRUPT_ARBITRATION 5

#define TIMER2_VECTOR 123
#define TIMER2_IRQ_LEVEL 4
#define TIMER2_INTERRUPT_ARBITRATION 4

#define CLOCK_SPEED 25    /* in Mhz */

#define max(a,b) (((a)>(b)) ? (a) : (b))

void (*Restart_Fifo_Full_A_Timer)(void);
void (*Restart_Check_A_Timer)(void);
void (*Restart_Fifo_Full_B_Timer)(void);
void (*Restart_Check_B_Timer)(void);

int preload = 0;

/*
 * __Restart_Fifo_Full_Timer
 *
 * When a character is received, sets the TIMER to raise an interrupt at
 * TIMEOUT.  It's necessary to prevent from not getting n-1 characters
 * (with n the Uart Fifo size).
 */
static void __Restart_Fifo_Full_Timer (void)
{
  TSR1 |= m340_TO;
  TCR1 &= ~m340_CPE;
  WPREL11 = preload;
  TCR1 |= m340_CPE;
}

/*
 * __Restart_Check_Timer
 *
 * When no character has been received recently, check now and then if whether
 * a there's a character in the FIFO
 */
static void __Restart_Check_Timer (void)
{
 TSR1 |= m340_TO;
 TCR1 &= ~m340_CPE;
 WPREL11 = 0xFFFF;
 TCR1 |= m340_CPE;
}

/*
 * __do_nothing
 *
 * We always restart the fifo full timer with a call to Restart_*_Timer
 * if we do not use FIFO full, Restart_X_Timer are set to do __do_nothing
 */
static void __do_nothing (void)
{
}

#define Fifo_Full_on_A \
   (m340_uart_config[UART_CHANNEL_A].rx_mode==UART_FIFO_FULL && \
    m340_uart_config[UART_CHANNEL_A].enable && \
    m340_uart_config[UART_CHANNEL_A].mode==UART_INTERRUPTS)
#define Fifo_Full_on_B \
    (m340_uart_config[UART_CHANNEL_B].rx_mode==UART_FIFO_FULL && \
    m340_uart_config[UART_CHANNEL_B].enable && \
    m340_uart_config[UART_CHANNEL_B].mode==UART_INTERRUPTS)

/*
 * Fifo_Full_benchmark_timer_initialize
 *
 * initialize Timer 1 for FIFO full mode
 */
void Fifo_Full_benchmark_timer_initialize (void)
{
  float max_baud_rate;
  int prescaler_output_tap = -1;
  int nb_of_clock_ticks = 0;
  rtems_isr_entry old_handler;

  /*
   *  USE TIMER 1 for UART FIFO FULL mode
   */
  if ( Fifo_Full_on_A || Fifo_Full_on_B ) {
    /* Disable the timer */
    TCR1 &= ~m340_SWR;

    /* Reset the interrupts */
    TSR1 &= ~(m340_TO | m340_TG | m340_TC);

    /* Init the stop bit for normal operation, ignore FREEZE, user privileges,
     * set interrupt arbitration.
     */
    TMCR1 = TIMER1_INTERRUPT_ARBITRATION;

    /* interrupt priority level and interrupt vector */
    TIR1 = TIMER1_VECTOR | (TIMER1_IRQ_LEVEL << 8);

    /* compute prescaler */
    if ( Fifo_Full_on_A && Fifo_Full_on_B) {
      max_baud_rate = max(
        m340_uart_config[UART_CHANNEL_A].rx_baudrate,
        m340_uart_config[UART_CHANNEL_B].rx_baudrate
      );
    } else if ( Fifo_Full_on_A ) {
      max_baud_rate = m340_uart_config[UART_CHANNEL_A].rx_baudrate;
    } else
      max_baud_rate = m340_uart_config[UART_CHANNEL_B].rx_baudrate;

    /* find out config */
    nb_of_clock_ticks = (10/max_baud_rate)*(CLOCK_SPEED*1000000)*1.2;
    if (nb_of_clock_ticks < 0xFFFF) {
      preload = nb_of_clock_ticks;
      prescaler_output_tap = -1;
    } else if (nb_of_clock_ticks/2 < 0xFFFF) {
      preload = nb_of_clock_ticks/2;
      prescaler_output_tap = m340_Divide_by_2;
    } else if (nb_of_clock_ticks/4 < 0xFFFF) {
      preload = nb_of_clock_ticks/4;
      prescaler_output_tap = m340_Divide_by_4;
    } else if (nb_of_clock_ticks/8 < 0xFFFF) {
      preload = nb_of_clock_ticks/8;
      prescaler_output_tap = m340_Divide_by_16;
    } else if (nb_of_clock_ticks/16 < 0xFFFF) {
      preload = nb_of_clock_ticks/16;
      prescaler_output_tap = m340_Divide_by_16;
    } else if (nb_of_clock_ticks/32 < 0xFFFF) {
      preload = nb_of_clock_ticks/32;
      prescaler_output_tap = m340_Divide_by_32;
    } else if (nb_of_clock_ticks/64 < 0xFFFF) {
      preload = nb_of_clock_ticks/64;
      prescaler_output_tap = m340_Divide_by_64;
    } else if (nb_of_clock_ticks/128 < 0xFFFF) {
      preload = nb_of_clock_ticks/128;
      prescaler_output_tap = m340_Divide_by_128;
    } else if (nb_of_clock_ticks/256 < 0xFFFF) {
      preload = nb_of_clock_ticks/256;
      prescaler_output_tap = m340_Divide_by_256;
    }

    /* Input Capture/Output Compare (ICOC) */
    TCR1 = m340_SWR | m340_TO_Enabled | m340_ICOC;
    if (prescaler_output_tap!=-1) TCR1 |= prescaler_output_tap | m340_PSE;

    /* install interrupt vector */
    rtems_interrupt_catch(InterruptHandler, TIMER1_VECTOR, &old_handler);

  } /* fifo full mode on a uart */

  /* install routines */
  if ( Fifo_Full_on_A ) {
    Restart_Check_A_Timer     = __Restart_Check_Timer;
    Restart_Fifo_Full_A_Timer = __Restart_Fifo_Full_Timer;
  } else {
    Restart_Check_A_Timer     = __do_nothing;
    Restart_Fifo_Full_A_Timer = __do_nothing;
  }

  if ( Fifo_Full_on_B ) {
    Restart_Check_B_Timer     = __Restart_Check_Timer;
    Restart_Fifo_Full_B_Timer = __Restart_Fifo_Full_Timer;
  } else {
    Restart_Check_B_Timer     = __do_nothing;
    Restart_Fifo_Full_B_Timer = __do_nothing;
  }

  /* start checking timer */
  Restart_Check_A_Timer();
  Restart_Check_B_Timer();
}

/*
 * benchmark_timer_initialize
 *
 * init Timer for timing test suites
 */
void benchmark_timer_initialize (void)
{
  /* Disable the timer */
  TCR2 &= ~m340_SWR;

  /* Reset the interrupts */
  TSR2 &= ~(m340_TO | m340_TG | m340_TC);

  /* Init the stop bit for normal operation, ignore FREEZE, user privileges,
     set interrupt arbitration */
  TMCR1 = TIMER2_INTERRUPT_ARBITRATION;

  /* interrupt priority level and interrupt vector */
  TIR1 = TIMER2_VECTOR | (TIMER2_IRQ_LEVEL << 8);

  /* Init the stop bit for normal operation, ignore FREEZE, user privileges,
     set interrupt arbitration */
  TMCR2 = TIMER2_INTERRUPT_ARBITRATION;

  /* Preload register 1 */
  WPREL21 = 0xFFFF;

  /* Input Capture/Output Compare (ICOC) */
  TCR2 = m340_SWR | m340_ICOC | m340_PSE | m340_Divide_by_16 | m340_CPE;
}

/*
 * benchmark_timer_read
 *
 * Return timer value in microsecond units
 */
uint32_t benchmark_timer_read (void)
{
 /* there's CLOCK_SPEED / 16 micro seconds between two timer
  * register decrements.
  */
 return (((0xFFFF - TCNTR2) * CLOCK_SPEED) / 16);
}

/*
 * benchmark_timer_disable_subtracting_average_overhead
 */
void benchmark_timer_disable_subtracting_average_overhead(bool find_flag)
{
}
