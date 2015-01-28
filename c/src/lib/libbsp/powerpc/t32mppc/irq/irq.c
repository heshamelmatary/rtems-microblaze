/*
 * Copyright (c) 2012 embedded brains GmbH.  All rights reserved.
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

#include <bsp/irq-generic.h>

rtems_status_code bsp_interrupt_vector_enable(rtems_vector_number vector)
{
	return RTEMS_INVALID_ID;
}

rtems_status_code bsp_interrupt_vector_disable(rtems_vector_number vector)
{
	return RTEMS_INVALID_ID;
}

rtems_status_code bsp_interrupt_facility_initialize(void)
{
	return RTEMS_SUCCESSFUL;
}
