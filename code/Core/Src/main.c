/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "i2c_ex.h"
#include "usart.h"
#include "usart_ex.h"
#include "gpio.h"
#include "gpio_ex.h"
#include "tim.h"
#include "sensors.h"
#include "iwdg.h"
#include "calibration.h"

// #define DEBUG
#define WET_SENSOR

struct zs05_data zs05_data = {0};
struct p_bmp180 p_bmp180 = {0};
uint32_t chunk_cnt = 0;

usart_header send_hdr = {0};
usart_packet whoami_pack = {
    .chunk_hdr.id = CHUNK_ID_TYPE,
    .chunk_hdr.type = DATA_TYPE_UINT32,
    .chunk_hdr.payload_sz = 4,
};

usart_packet data_pack[2];

usart_packet error_pack = {
    .chunk_hdr.id = CHUNK_ID_ERROR,
    .chunk_hdr.type = DATA_TYPE_UINT16,
    .chunk_hdr.payload_sz = 2,
    .data = {0},
};

uint32_t sensor_type = 0;
enum wetsens_state wetsens_state = 0;
enum cmd cmd = CMD_NONE;
uint16_t pack_crc = 0;

union offset offset = {0};

struct flags flags = {0};

void SystemClock_Config(void);

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_SYSCFG);
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

    /* SysTick_IRQn interrupt configuration */
    NVIC_SetPriority(SysTick_IRQn, 3);

    /* Configure the system clock */
    SystemClock_Config();

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
#ifndef WET_SENSOR
    MX_I2C1_Init();
#endif
    MX_USART1_UART_Init();
#ifndef DEBUG
    MX_IWDG_Init();
#endif

#ifndef WET_SENSOR
    sensor_type = sensors_init();
 #else
    sensor_type = SENSOR_TYPE_WETSENS;
 #endif
    MX_TIM2_Init();

    turn_red_on(); // signal that power on

    send_hdr.protocol = PROTOCOL_AURA;
    send_hdr.src = uid_hash();
    send_hdr.dest = PC_ID;
    send_hdr.cnt = 0;

    memcpy_u8(&sensor_type, whoami_pack.data, 4);

    LL_USART_EnableIT_RXNE(USART1);

    GPIO_EXTI_Enable();

    offset = calibration_init();

    while (1) {
        sensors_measure(data_pack, sensor_type);
#ifndef DEBUG
        if (is_green_on()) {
            LL_IWDG_ReloadCounter(IWDG);
        }
#endif
        LL_mDelay(50);
    }
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
    LL_FLASH_SetLatency(LL_FLASH_LATENCY_0);
    while (LL_FLASH_GetLatency() != LL_FLASH_LATENCY_0) {
    }
    LL_RCC_HSE_Enable();

    /* Wait till HSE is ready */
    while (LL_RCC_HSE_IsReady() != 1) {
    }
    LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE_DIV_1, LL_RCC_PLL_MUL_2);
    LL_RCC_PLL_Enable();

    /* Wait till PLL is ready */
    while (LL_RCC_PLL_IsReady() != 1) {
    }
    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

    /* Wait till System clock is ready */
    while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL) {
    }
    LL_Init1msTick(16000000);
    LL_SetSystemCoreClock(16000000);
    LL_RCC_SetUSARTClockSource(LL_RCC_USART1_CLKSOURCE_PCLK1);
    LL_RCC_SetI2CClockSource(LL_RCC_I2C1_CLKSOURCE_SYSCLK);
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
    __disable_irq();
    while (1) {
        blink_red();
    }
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
