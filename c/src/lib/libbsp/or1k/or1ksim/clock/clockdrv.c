/**
 * @file
 *
 * @ingroup bsp_clock
 *
 * @brief or1ksim clock support.
 */

/*
 * or1ksim Clock driver
 *
 * COPYRIGHT (c) 2014 Hesham ALMatary <heshamelmatary@gmail.com>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE
 */

#include <rtems.h>
#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/or1ksim.h>
#include <rtems/score/cpu.h>
#include <rtems/score/or1k-utility.h>

/* The number of clock cycles before generating a tick timer interrupt. */
#define TTMR_NUM_OF_CLOCK_TICKS_INTERRUPT     0x09ED9
#define OR1KSIM_CLOCK_CYCLE_TIME_NANOSECONDS  10

/* CPU counter */
static CPU_Counter_ticks cpu_counter_ticks;

/* This prototype is added here to Avoid warnings */
void Clock_isr(void *arg);

static void or1ksim_clock_at_tick(void)
{
  uint32_t TTMR;

 /* For TTMR register,
  * The least significant 28 bits are the number of clock cycles
  * before generating a tick timer interrupt. While the most
  * significant 4 bits are used for mode configuration, tick timer
  * interrupt enable and pending interrupts status.
  */
  TTMR = (CPU_OR1K_SPR_TTMR_MODE_RESTART | CPU_OR1K_SPR_TTMR_IE |
           (TTMR_NUM_OF_CLOCK_TICKS_INTERRUPT & CPU_OR1K_SPR_TTMR_TP_MASK)
         ) & ~(CPU_OR1K_SPR_TTMR_IP);

  _OR1K_mtspr(CPU_OR1K_SPR_TTMR, TTMR);
  _OR1K_mtspr(CPU_OR1K_SPR_TTCR, 0);

  cpu_counter_ticks += TTMR_NUM_OF_CLOCK_TICKS_INTERRUPT;
}

static void or1ksim_clock_handler_install(proc_ptr new_isr, proc_ptr old_isr)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  old_isr = NULL;
  _CPU_ISR_install_vector(OR1K_EXCEPTION_TICK_TIMER,
                          new_isr,
                          old_isr);

  if (sc != RTEMS_SUCCESSFUL) {
    rtems_fatal_error_occurred(0xdeadbeef);
  }
}

static void or1ksim_clock_initialize(void)
{
  uint32_t TTMR;

 /* For TTMR register,
  * The least significant 28 bits are the number of clock cycles
  * before generating a tick timer interrupt. While the most
  * significant 4 bits are used for mode configuration, tick timer
  * interrupt enable and pending interrupts status.
  */

  /* FIXME: Long interval should pass since initializing the tick timer
   * registers fires exceptions dispite interrupts has not been enabled yet.
   */
  TTMR = (CPU_OR1K_SPR_TTMR_MODE_RESTART | CPU_OR1K_SPR_TTMR_IE |
           (0xFFED9 & CPU_OR1K_SPR_TTMR_TP_MASK)
         ) & ~(CPU_OR1K_SPR_TTMR_IP);

  _OR1K_mtspr(CPU_OR1K_SPR_TTMR, TTMR);
  _OR1K_mtspr(CPU_OR1K_SPR_TTCR, 0);

  /* Initialize CPU Counter */
  cpu_counter_ticks = 0;
}

 static void or1ksim_clock_cleanup(void)
{
 uint32_t sr;

  sr = _OR1K_mfspr(CPU_OR1K_SPR_SR);

  /* Disable tick timer exceptions */
  _OR1K_mtspr(CPU_OR1K_SPR_SR, (sr & ~CPU_OR1K_SPR_SR_IEE)
  & ~CPU_OR1K_SPR_SR_TEE);

  /* Invalidate tick timer config registers */
  _OR1K_mtspr(CPU_OR1K_SPR_TTCR, 0);
  _OR1K_mtspr(CPU_OR1K_SPR_TTMR, 0);
}

/*
 *  Return the nanoseconds since last tick
 */
static uint32_t or1ksim_clock_nanoseconds_since_last_tick(void)
{
  return
  TTMR_NUM_OF_CLOCK_TICKS_INTERRUPT * OR1KSIM_CLOCK_CYCLE_TIME_NANOSECONDS;
}

CPU_Counter_ticks _CPU_Counter_read(void)
{
  uint32_t ticks_since_last_timer_interrupt;

  ticks_since_last_timer_interrupt = _OR1K_mfspr(CPU_OR1K_SPR_TTCR);

  return cpu_counter_ticks + ticks_since_last_timer_interrupt;
}

CPU_Counter_ticks _CPU_Counter_difference(
  CPU_Counter_ticks second,
  CPU_Counter_ticks first
)
{
  return second - first;
}
#define Clock_driver_support_at_tick() or1ksim_clock_at_tick()

#define Clock_driver_support_initialize_hardware() or1ksim_clock_initialize()

#define Clock_driver_support_install_isr(isr, old_isr) \
  do {                                                 \
    old_isr = NULL;                                    \
    or1ksim_clock_handler_install(isr, old_isr);       \
  } while (0)

#define Clock_driver_support_shutdown_hardware() or1ksim_clock_cleanup()

#define Clock_driver_nanoseconds_since_last_tick \
  or1ksim_clock_nanoseconds_since_last_tick

#include "../../../shared/clockdrv_shell.h"
