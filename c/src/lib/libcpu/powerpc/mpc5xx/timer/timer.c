/**
 *  @file
 *  @brief Timer Driver for the PowerPC MPC5xx.
 *
 *  This file manages the interval timer on the PowerPC MPC5xx.
 *  @noe This is not the PIT, but rather the RTEMS interval timer.
 *  We shall use the bottom 32 bits of the timebase register,
 */

/*
 *  MPC5xx port sponsored by Defence Research and Development Canada - Suffield
 *  Copyright (C) 2004, Real-Time Systems Inc. (querbach@realtime.bc.ca)
 *
 *  Derived from c/src/lib/libcpu/powerpc/mpc8xx/timer/timer.c:
 *
 *  Author: Jay Monkman (jmonkman@frasca.com)
 *  Copywright (C) 1998 by Frasca International, Inc.
 *
 *  Derived from c/src/lib/libcpu/ppc/ppc403/timer/timer.c:
 *
 *  Author:     Andrew Bray <andy@i-cubed.co.uk>
 *
 *  COPYRIGHT (c) 1995 by i-cubed ltd.
 *
 *  To anyone who acknowledges that this file is provided "AS IS"
 *  without any express or implied warranty:
 *      permission to use, copy, modify, and distribute this file
 *      for any purpose is hereby granted without fee, provided that
 *      the above copyright notice and this notice appears in all
 *      copies, and that the name of i-cubed limited not be used in
 *      advertising or publicity pertaining to distribution of the
 *      software without specific, written prior permission.
 *      i-cubed limited makes no representations about the suitability
 *      of this software for any purpose.
 *
 *  Derived from c/src/lib/libcpu/hppa1_1/timer/timer.c:
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <rtems.h>
#include <rtems/btimer.h>
#include <mpc5xx.h>

static volatile uint32_t Timer_starting;
static bool benchmark_timer_find_average_overhead;

extern uint32_t bsp_timer_least_valid;
extern uint32_t bsp_timer_average_overhead;

/*
 *  This is so small that this code will be reproduced where needed.
 */
static inline uint32_t get_itimer(void)
{
   uint32_t ret;

   __asm__ volatile ("mftb %0" : "=r" ((ret))); /* TBLO */

   return ret;
}

void benchmark_timer_initialize(void)
{
  /* set interrupt level and enable timebase. This should never */
  /*  generate an interrupt however. */
  usiu.tbscrk = USIU_UNLOCK_KEY;
  usiu.tbscr |= USIU_TBSCR_TBIRQ(4) 	/* interrupt priority level */
              | USIU_TBSCR_TBF 		/* freeze timebase during debug */
              | USIU_TBSCR_TBE;		/* enable timebase */
  usiu.tbscrk = 0;

  Timer_starting = get_itimer();
}

benchmark_timer_t benchmark_timer_read(void)
{
  uint32_t clicks;
  uint32_t total;

  clicks = get_itimer();

  total = clicks - Timer_starting;

  if ( benchmark_timer_find_average_overhead == 1 )
    return total;          /* in XXX microsecond units */

  else {
    if ( total < bsp_timer_least_valid ) {
      return 0;            /* below timer resolution */
    }
    return (total - bsp_timer_average_overhead);
  }
}

void benchmark_timer_disable_subtracting_average_overhead(bool find_flag)
{
  benchmark_timer_find_average_overhead = find_flag;
}
