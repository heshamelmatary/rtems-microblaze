/*
 * @file
 * @ingroup sparc64_niagara
 * @brief Implementations for interrupt mechanisms for Time Test 27
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

/*
 *  Define the interrupt mechanism for Time Test 27
 */

#define MUST_WAIT_FOR_INTERRUPT 0

#define Install_tm27_vector( handler ) /* set_vector( (handler), 6, 1 ) */

#define Cause_tm27_intr()  /* XXX */

#define Clear_tm27_intr()  /* XXX */

#define Lower_tm27_intr() /* empty */

#endif
