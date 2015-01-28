/*
 *  Motorola MC68xxx Dependent Idle Body Source
 *
 *  This kernel routine is the idle thread.  The idle thread runs any time
 *  no other thread is ready to run.  This thread loops forever with
 *  interrupts enabled.
 */

/*
 *  COPYRIGHT (c) 1989-2002.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <rtems/system.h>
#include <rtems/score/thread.h>

void *_CPU_Thread_Idle_body( uintptr_t ignored )
{
#if defined(mcf5272)
  for( ; ; ) {
    __asm__ volatile( "nop" );
    __asm__ volatile( "nop" );
  }
#else
  for( ; ; ) {
    /* supervisor mode, all interrupts on */
    __asm__ volatile( "stop #0x3000":::"cc" );
  }
#endif
}
