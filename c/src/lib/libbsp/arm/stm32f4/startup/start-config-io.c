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

#include <bsp/io.h>
#include <bsp.h>

const stm32f4_gpio_config stm32f4_start_config_gpio [] = {
#ifdef STM32F4_FAMILY_F4XXXX
  #ifdef STM32F4_ENABLE_USART_1
    STM32F4_PIN_USART1_TX_PA9,
    STM32F4_PIN_USART1_RX_PA10,
  #endif
  #ifdef STM32F4_ENABLE_USART_2
    STM32F4_PIN_USART2_TX_PA2,
    STM32F4_PIN_USART2_RX_PA3,
  #endif
  #ifdef STM32F4_ENABLE_USART_3
    STM32F4_PIN_USART3_TX_PD8,
    STM32F4_PIN_USART3_RX_PD9,
  #endif
  #ifdef STM32F4_ENABLE_UART_4
    STM32F4_PIN_UART4_TX_PA0,
    STM32F4_PIN_UART4_RX_PA1,
  #endif
  #ifdef STM32F4_ENABLE_UART_5
    STM32F4_PIN_UART5_TX_PC12,
    STM32F4_PIN_UART5_RX_PD2,
  #endif
  #ifdef STM32F4_ENABLE_USART_6
    STM32F4_PIN_USART6_TX_PC6,
    STM32F4_PIN_USART6_RX_PC7,
  #endif
  #ifdef STM32F4_ENABLE_I2C1
    #error Not implemented.
  #endif
  #ifdef STM32F4_ENABLE_I2C2
    #error Not implemented.
  #endif
#endif /* STM32F4_FAMILY_F4XXXX */
#ifdef STM32F4_FAMILY_F10XXX
  #ifdef STM32F4_ENABLE_USART_1
    STM32F4_PIN_USART1_TX_MAP_0,
    STM32F4_PIN_USART1_RX_MAP_0,
  #endif
  #ifdef STM32F4_ENABLE_USART_2
    STM32F4_PIN_USART2_TX_MAP_0,
    STM32F4_PIN_USART2_RX_MAP_0,
  #endif
  #ifdef STM32F4_ENABLE_USART_3
    STM32F4_PIN_USART3_TX_MAP_0,
    STM32F4_PIN_USART3_RX_MAP_0,
  #endif
  #ifdef STM32F4_ENABLE_UART_4
    STM32F4_PIN_UART4_TX,
    STM32F4_PIN_UART4_RX,
  #endif
  #ifdef STM32F4_ENABLE_UART_5
    STM32F4_PIN_UART5_TX,
    STM32F4_PIN_UART5_RX,
  #endif
  #ifdef STM32F4_ENABLE_USART_6
    #error STM32F10XXX has no USART 6
  #endif
  #ifdef STM32F4_ENABLE_I2C1
    STM32F4_PIN_I2C1_SCL_MAP0,
    STM32F4_PIN_I2C1_SDA_MAP0,
  #endif
  #ifdef STM32F4_ENABLE_I2C2
    STM32F4_PIN_I2C2_SCL,
    STM32F4_PIN_I2C2_SDA,
  #endif
#endif /* STM32F4_FAMILY_F10XXX */
  STM32F4_GPIO_CONFIG_TERMINAL
};
