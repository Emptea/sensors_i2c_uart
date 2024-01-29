/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    gpio.h
 * @brief   This file contains all the function prototypes for
 *          the gpio.c file
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
#ifndef __GPIO_H__
#define __GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

#define GPIO_LED      GPIOB
#define PIN_GREEN_LED LL_GPIO_PIN_12
#define PIN_RED_LED   LL_GPIO_PIN_13
#define GPIO_BTN_EXTI GPIOC
#define PIN_EXTI      LL_GPIO_PIN_13
#define PIN_BTN       LL_GPIO_PIN_14

extern enum wetsens_state {
    WETSENS_STATE_DRY = 0,
    WETSENS_STATE_WET = 0xFFFF
} wetsens_state;

void MX_GPIO_Init(void);
void GPIO_EXTI_Enable();

#ifdef __cplusplus
}
#endif
#endif /*__ GPIO_H__ */
