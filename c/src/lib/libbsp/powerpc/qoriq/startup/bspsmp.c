/*
 * Copyright (c) 2013-2015 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <rtems/score/smpimpl.h>

#include <libcpu/powerpc-utility.h>

#include <bsp.h>
#include <bsp/mmu.h>
#include <bsp/fatal.h>
#include <bsp/qoriq.h>
#include <bsp/vectors.h>
#include <bsp/bootcard.h>
#include <bsp/irq-generic.h>
#include <bsp/linker-symbols.h>

LINKER_SYMBOL(bsp_exc_vector_base);

#if QORIQ_THREAD_COUNT > 1
void _start_thread(void);
#endif

void _start_secondary_processor(void);

#define IPI_INDEX 0

#define TLB_BEGIN (3 * QORIQ_TLB1_ENTRY_COUNT / 4)

#define TLB_END QORIQ_TLB1_ENTRY_COUNT

#define TLB_COUNT (TLB_END - TLB_BEGIN)

/*
 * These values can be obtained with the debugger or a look into the
 * U-Boot sources (arch/powerpc/cpu/mpc85xx/release.S).
 */
#define BOOT_BEGIN U_BOOT_BOOT_PAGE_BEGIN
#define BOOT_LAST U_BOOT_BOOT_PAGE_LAST
#define SPIN_TABLE (BOOT_BEGIN + U_BOOT_BOOT_PAGE_SPIN_OFFSET)

typedef struct {
  uint32_t addr_upper;
  uint32_t addr_lower;
  uint32_t r3_upper;
  uint32_t r3_lower;
  uint32_t reserved_0;
  uint32_t pir;
  uint32_t r6_upper;
  uint32_t r6_lower;
  uint32_t reserved_1[8];
} uboot_spin_table;

#if QORIQ_THREAD_COUNT > 1
static bool is_started_by_u_boot(uint32_t cpu_index)
{
  return cpu_index % QORIQ_THREAD_COUNT == 0;
}

void qoriq_start_thread(void)
{
  const Per_CPU_Control *cpu_self = _Per_CPU_Get();

  ppc_exc_initialize_interrupt_stack(
    (uintptr_t) cpu_self->interrupt_stack_low,
    rtems_configuration_get_interrupt_stack_size()
  );

  bsp_interrupt_facility_initialize();

  _SMP_Start_multitasking_on_secondary_processor();
}
#endif

static void start_thread_if_necessary(uint32_t cpu_index_self)
{
#if QORIQ_THREAD_COUNT > 1
  uint32_t i;

  for (i = 1; i < QORIQ_THREAD_COUNT; ++i) {
    uint32_t cpu_index_next = cpu_index_self + i;

    if (
      is_started_by_u_boot(cpu_index_self)
        && cpu_index_next < rtems_configuration_get_maximum_processors()
        && _SMP_Should_start_processor(cpu_index_next)
    ) {
      /* Thread Initial Next Instruction Address (INIA) */
      PPC_SET_THREAD_MGMT_REGISTER(321, (uint32_t) _start_thread);

      /* Thread Initial Machine State (IMSR) */
      PPC_SET_THREAD_MGMT_REGISTER(289, QORIQ_INITIAL_MSR);

      /* Thread Enable Set (TENS) */
      PPC_SET_SPECIAL_PURPOSE_REGISTER(438, 1U << i);
    }
  }
#endif
}

void bsp_start_on_secondary_processor(void)
{
  uint32_t cpu_index_self = _SMP_Get_current_processor();
  const Per_CPU_Control *cpu_self = _Per_CPU_Get_by_index(cpu_index_self);

  ppc_exc_initialize_with_vector_base(
    (uintptr_t) cpu_self->interrupt_stack_low,
    rtems_configuration_get_interrupt_stack_size(),
    bsp_exc_vector_base
  );

  /* Now it is possible to make the code execute only */
  qoriq_mmu_change_perm(
    FSL_EIS_MAS3_SR | FSL_EIS_MAS3_SX,
    FSL_EIS_MAS3_SX,
    FSL_EIS_MAS3_SR
  );

  bsp_interrupt_facility_initialize();

  start_thread_if_necessary(cpu_index_self);

  _SMP_Start_multitasking_on_secondary_processor();
}

static void bsp_inter_processor_interrupt(void *arg)
{
  _SMP_Inter_processor_interrupt_handler();
}

uint32_t _CPU_SMP_Initialize(void)
{
  if (rtems_configuration_get_maximum_processors() > 0) {
    qoriq_mmu_context mmu_context;

    qoriq_mmu_context_init(&mmu_context);
    qoriq_mmu_add(
      &mmu_context,
      BOOT_BEGIN,
      BOOT_LAST,
      0,
      0,
      FSL_EIS_MAS3_SR | FSL_EIS_MAS3_SW,
      0
    );
    qoriq_mmu_partition(&mmu_context, TLB_COUNT);
    qoriq_mmu_write_to_tlb1(&mmu_context, TLB_BEGIN);
  }

  start_thread_if_necessary(0);

  return QORIQ_CPU_COUNT;
}

static void release_processor(uboot_spin_table *spin_table, uint32_t cpu_index)
{
  const Per_CPU_Control *cpu = _Per_CPU_Get_by_index(cpu_index);

  spin_table->pir = cpu_index;
  spin_table->r3_lower = (uint32_t) cpu->interrupt_stack_high;
  spin_table->addr_upper = 0;
  rtems_cache_flush_multiple_data_lines(spin_table, sizeof(*spin_table));
  ppc_synchronize_data();
  spin_table->addr_lower = (uint32_t) _start_secondary_processor;
  rtems_cache_flush_multiple_data_lines(spin_table, sizeof(*spin_table));
}

bool _CPU_SMP_Start_processor(uint32_t cpu_index)
{
#if QORIQ_THREAD_COUNT > 1
  if (is_started_by_u_boot(cpu_index)) {
    uboot_spin_table *spin_table =
      &((uboot_spin_table *) SPIN_TABLE)[cpu_index / 2 - 1];

    release_processor(spin_table, cpu_index);

    return true;
  } else {
    return _SMP_Should_start_processor(cpu_index - 1);
  }
#else
  uboot_spin_table *spin_table = (uboot_spin_table *) SPIN_TABLE;

  release_processor(spin_table, cpu_index);

  return true;
#endif
}

static void mmu_config_undo(void)
{
  int i;

  for (i = TLB_BEGIN; i < TLB_END; ++i) {
    qoriq_tlb1_invalidate(i);
  }
}

void _CPU_SMP_Finalize_initialization(uint32_t cpu_count)
{
  if (rtems_configuration_get_maximum_processors() > 0) {
    mmu_config_undo();
  }

  if (cpu_count > 1) {
    rtems_status_code sc;

    sc = rtems_interrupt_handler_install(
      QORIQ_IRQ_IPI_0,
      "IPI",
      RTEMS_INTERRUPT_UNIQUE,
      bsp_inter_processor_interrupt,
      NULL
    );
    if (sc != RTEMS_SUCCESSFUL) {
      bsp_fatal(QORIQ_FATAL_SMP_IPI_HANDLER_INSTALL);
    }
  }
}

void _CPU_SMP_Send_interrupt(uint32_t target_processor_index)
{
  qoriq.pic.ipidr [IPI_INDEX].reg = 1U << target_processor_index;
}
