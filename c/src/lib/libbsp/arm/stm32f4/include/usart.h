/**
 * @file
 * @ingroup stm32f4_usart
 * @brief USART (universal synchronous/asynchronous receiver/transmitter) support.
 */

/*
 * Copyright (c) 2012 Sebastian Huber.  All rights reserved.
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

#ifndef LIBBSP_ARM_STM32F4_USART_H
#define LIBBSP_ARM_STM32F4_USART_H

#include <libchip/serial.h>

/**
 * @defgroup stm32f4_usart USART Support
 * @ingroup arm_stm32f4
 * @brief USART Support
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern const console_fns stm32f4_usart_fns;

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_STM32F4_USART_H */
