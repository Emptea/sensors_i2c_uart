/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    usart.h
 * @brief   This file contains all the function prototypes for
 *          the usart.c file
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif


#ifdef STM32F031x6
    #define USART_GPIO  GPIOA
    #define USART_DE_PIN LL_GPIO_PIN_1
    #define USART_TX_PIN  LL_GPIO_PIN_2
    #define USART_RX_PIN  LL_GPIO_PIN_3
#elif STM32F051x8
    #define USART_GPIO  GPIOA
    #define USART_DE_PIN LL_GPIO_PIN_12
    #define USART_TX_PIN  LL_GPIO_PIN_9
    #define USART_RX_PIN  LL_GPIO_PIN_10
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_ll_usart.h"

void MX_USART1_UART_Init(void);

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */
