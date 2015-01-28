/**
 * @file
 *
 * @ingroup mpc55xx
 *
 * @brief Source file for MPC55XX interrupt support.
 */

/*
 * Copyright (c) 2008-2012 embedded brains GmbH.  All rights reserved.
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

#include <mpc55xx/regs.h>

#include <libcpu/powerpc-utility.h>

#include <bsp/irq.h>
#include <bsp/vectors.h>
#include <bsp/irq-generic.h>

#define RTEMS_STATUS_CHECKS_USE_PRINTK

#include <rtems/status-checks.h>

/**
 * @brief Returns the priority @a priority of IRQ @a vector from the INTC.
 */
rtems_status_code mpc55xx_intc_get_priority( rtems_vector_number vector, unsigned *priority)
{
	if (MPC55XX_IRQ_IS_VALID( vector)) {
		*priority = INTC.PSR [vector].B.PRI;
		return RTEMS_SUCCESSFUL;
	} else {
		*priority = MPC55XX_INTC_INVALID_PRIORITY;
		return RTEMS_INVALID_NUMBER;
	}
}

/**
 * @brief Sets the priority of IRQ @a vector to @a priority at the INTC.
 */
rtems_status_code mpc55xx_intc_set_priority( rtems_vector_number vector, unsigned priority)
{
	if (MPC55XX_IRQ_IS_VALID( vector) && MPC55XX_INTC_IS_VALID_PRIORITY( priority)) {
		INTC.PSR [vector].B.PRI = priority;
		if (INTC.PSR [vector].B.PRI == priority) {
			return RTEMS_SUCCESSFUL;
		} else {
			return RTEMS_IO_ERROR;
		}
	} else {
		return RTEMS_INVALID_NUMBER;
	}
}

/**
 * @brief Raises the software IRQ with number @a vector.
 */
rtems_status_code mpc55xx_intc_raise_software_irq( rtems_vector_number vector)
{
	if (MPC55XX_IRQ_IS_SOFTWARE( vector)) {
		INTC.SSCIR [vector].B.SET = 1;
		return RTEMS_SUCCESSFUL;
	} else {
		return RTEMS_INVALID_NUMBER;
	}
}

/**
 * @brief Clears the software IRQ with number @a vector.
 */
rtems_status_code mpc55xx_intc_clear_software_irq( rtems_vector_number vector)
{
	if (MPC55XX_IRQ_IS_SOFTWARE( vector)) {
		INTC.SSCIR [vector].B.CLR = 1;
		return RTEMS_SUCCESSFUL;
	} else {
		return RTEMS_INVALID_NUMBER;
	}
}

/**
 * @brief Installs interrupt handler and sets priority.
 */
rtems_status_code mpc55xx_interrupt_handler_install(
	rtems_vector_number vector,
	const char *info,
	rtems_option options,
	unsigned priority,
	rtems_interrupt_handler handler,
	void *arg
)
{
	if (MPC55XX_IRQ_IS_VALID( vector) && MPC55XX_INTC_IS_VALID_PRIORITY( priority)) {
		rtems_status_code sc = RTEMS_SUCCESSFUL;

		sc = rtems_interrupt_handler_install( vector, info, options, handler, arg);
		RTEMS_CHECK_SC( sc, "Install interrupt handler");

		return mpc55xx_intc_set_priority( vector, priority);
	} else {
		return RTEMS_INVALID_NUMBER;
	}
}

static void mpc55xx_interrupt_dispatch(void)
{
	/* Acknowlege interrupt request */
	rtems_vector_number vector = INTC.IACKR.B.INTVEC;

	/* Save machine state and enable external exceptions */
	uint32_t msr = ppc_external_exceptions_enable();

	/* Dispatch interrupt handlers */
	bsp_interrupt_handler_dispatch( vector);

	/* Restore machine state */
	ppc_external_exceptions_disable( msr);

	/* End of interrupt */
	INTC.EOIR.R = 1;
}

#ifndef PPC_EXC_CONFIG_USE_FIXED_HANDLER

/**
 * @brief External exception handler.
 */
static int mpc55xx_external_exception_handler( BSP_Exception_frame *frame, unsigned exception_number)
{
	mpc55xx_interrupt_dispatch();

	return 0;
}

#else /* PPC_EXC_CONFIG_USE_FIXED_HANDLER */

void bsp_interrupt_dispatch(void)
{
	mpc55xx_interrupt_dispatch();
}

#endif /* PPC_EXC_CONFIG_USE_FIXED_HANDLER */

rtems_status_code bsp_interrupt_facility_initialize(void)
{
	rtems_vector_number vector;

#ifndef PPC_EXC_CONFIG_USE_FIXED_HANDLER
	/* Install exception handler */
	if (ppc_exc_set_handler( ASM_EXT_VECTOR, mpc55xx_external_exception_handler)) {
		return RTEMS_IO_ERROR;
	}
#endif

	/* Initialize interrupt controller */

	/* Disable all interrupts */
	for (vector = MPC55XX_IRQ_MIN; vector <= MPC55XX_IRQ_MAX; ++vector) {
		INTC.PSR [vector].B.PRI = MPC55XX_INTC_DISABLED_PRIORITY;
	}

	/* Software vector mode */
	INTC.MCR.B.VTES = 0;
	INTC.MCR.B.HVEN = 0;

	/* Set current priority to 0 */
	INTC.CPR.B.PRI = 0;

	return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_vector_enable( rtems_vector_number vector)
{
	if (MPC55XX_IRQ_IS_VALID( vector)) {
		return mpc55xx_intc_set_priority( vector, MPC55XX_INTC_DEFAULT_PRIORITY);
	} else {
		return RTEMS_SUCCESSFUL;
	}
}

rtems_status_code bsp_interrupt_vector_disable( rtems_vector_number vector)
{
	if (MPC55XX_IRQ_IS_VALID( vector)) {
		return mpc55xx_intc_set_priority( vector, MPC55XX_INTC_DISABLED_PRIORITY);
	} else {
		return RTEMS_SUCCESSFUL;
	}
}
