/**
 * @file
 *
 * @brief ARMV7M ISR Dispatch
 */

/*
 * Copyright (c) 2011-2014 Sebastian Huber.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/score/armv7m.h>
#include <rtems/score/percpu.h>

#ifdef ARM_MULTILIB_ARCH_V7M

static void __attribute__((naked)) _ARMV7M_Thread_dispatch( void )
{
  __asm__ volatile (
    "bl _Thread_Dispatch\n"
    /* FIXME: SVC, binutils bug */
    ".short 0xdf00\n"
    "nop\n"
  );
}

static void _ARMV7M_Trigger_lazy_floating_point_context_save( void )
{
#ifdef ARM_MULTILIB_VFP
  __asm__ volatile (
    "vmov.f32 s0, s0\n"
  );
#endif
}

void _ARMV7M_Pendable_service_call( void )
{
  ARMV7M_Exception_frame *ef;

  _ISR_Nest_level = 1;

  _ARMV7M_SCB->icsr = ARMV7M_SCB_ICSR_PENDSVCLR;
  _ARMV7M_Trigger_lazy_floating_point_context_save();

  ef = (ARMV7M_Exception_frame *) _ARMV7M_Get_PSP();
  --ef;
  _ARMV7M_Set_PSP( (uint32_t) ef );

  /*
   * According to "ARMv7-M Architecture Reference Manual" section B1.5.6
   * "Exception entry behavior" the return address is half-word aligned.
   */
  ef->register_pc = (void *)
    ((uintptr_t) _ARMV7M_Thread_dispatch & ~((uintptr_t) 1));

  ef->register_xpsr = 0x01000000U;
}

void _ARMV7M_Supervisor_call( void )
{
  ARMV7M_Exception_frame *ef;

  _ARMV7M_Trigger_lazy_floating_point_context_save();

  ef = (ARMV7M_Exception_frame *) _ARMV7M_Get_PSP();
  ++ef;
  _ARMV7M_Set_PSP( (uint32_t) ef );

  _ISR_Nest_level = 0;
  RTEMS_COMPILER_MEMORY_BARRIER();

  if ( _Thread_Dispatch_necessary ) {
    _ARMV7M_Pendable_service_call();
  }
}

#endif /* ARM_MULTILIB_ARCH_V7M */
