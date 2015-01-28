/**
 *  @file
 *
 *  @ingroup m68k_tm27
 *
 *  @brief Time Test 27
 */

/*
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_TMTEST27
#error "This is an RTEMS internal file you must not include directly."
#endif

#ifndef __tm27_h
#define __tm27_h

/**
 *  @defgroup m68k_tm27 Stuff for Time Test 27
 *
 *  @ingroup m68k_gen68360
 *
 *  @brief Don't bother with hardware -- just use a software-interrupt
 */

#define MUST_WAIT_FOR_INTERRUPT 0

#define Install_tm27_vector( handler ) set_vector( (handler), 34, 1 )

#define Cause_tm27_intr()	asm volatile ("trap #2");

#define Clear_tm27_intr() /* empty */

#define Lower_tm27_intr() /* empty */

#endif
