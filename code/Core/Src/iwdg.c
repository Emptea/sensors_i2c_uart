/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    iwdg.c
 * @brief   This file provides code for the configuration
 *          of the IWDG instances.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "iwdg.h"

/* IWDG init function */
void MX_IWDG_Init(void)
{
    LL_IWDG_Enable(IWDG);
    LL_IWDG_EnableWriteAccess(IWDG);
    LL_IWDG_SetPrescaler(IWDG, LL_IWDG_PRESCALER_128);
    LL_IWDG_SetReloadCounter(IWDG, 1000);
    while (LL_IWDG_IsReady(IWDG) != 1) {
    }

    // LL_IWDG_SetWindow(IWDG, 0);
    LL_IWDG_ReloadCounter(IWDG);
}
