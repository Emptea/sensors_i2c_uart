/* USER CODE BEGIN Header */
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
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lm75bd.h"
#include "tmp112.h"
#include "sht3x_dis.h"
#include "zs05.h"
#include "bmp180.h"
#include "tools.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//#define LM75BD
#define ZS05
//#define BMP180
//#define WET_SENSOR

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

struct zs05_data zs05_data = {0};
struct p_bmp180 p_bmp180 = {0};
uint32_t chunk_cnt = 0;

usart_header hdr = {0};
usart_packet whoami_pack;
usart_packet data_pack[2];

uint32_t sensor_type = 0;
enum wetsens_state wetsens_state = 0;
enum cmd cmd = CMD_NONE;
uint16_t pack_crc = 0;

struct flags flags = {0};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_SYSCFG);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

  /* SysTick_IRQn interrupt configuration */
  NVIC_SetPriority(SysTick_IRQn, 3);

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
	#ifndef WET_SENSOR
		MX_I2C1_Init();
	#endif
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
	
	#ifdef LM75BD
		float lm75bd_temp = 0;
		
		data_pack[0].chunk_hdr.id = CHUNK_ID_TEMP;
		data_pack[0].chunk_hdr.type = DATA_TYPE_FLOAT32;
		data_pack[0].chunk_hdr.payload_sz = 4;
		
		chunk_cnt = 1;
		
		sensor_type = SENSOR_TYPE_LM75BD;
		lm75bd_read_temp();
	#endif
		
	#ifdef ZS05
		float zs05_data[2] = {0};
		data_pack[0].chunk_hdr.id = CHUNK_ID_TEMP;
		data_pack[0].chunk_hdr.type = DATA_TYPE_FLOAT32;
		data_pack[0].chunk_hdr.payload_sz = 4;
		
		data_pack[1].chunk_hdr.id = CHUNK_ID_HUM;
		data_pack[1].chunk_hdr.type = DATA_TYPE_FLOAT32;
		data_pack[1].chunk_hdr.payload_sz = 4;
		
		chunk_cnt = 2;
	
		sensor_type = SENSOR_TYPE_ZS05;
		LL_mDelay(4000);
		while(!zs05_read(zs05_data))
			LL_mDelay(500);
	#endif
		
	#ifdef BMP180
		data_pack[0].chunk_hdr.id = CHUNK_ID_TEMP;
		data_pack[0].chunk_hdr.type = DATA_TYPE_FLOAT32;
		data_pack[0].chunk_hdr.payload_sz = 4;
		
		data_pack[1].chunk_hdr.id = CHUNK_ID_PRESS;
		data_pack[1].chunk_hdr.type = DATA_TYPE_FLOAT32;
		data_pack[1].chunk_hdr.payload_sz = 4;
		
		chunk_cnt = 2;
	
		sensor_type = SENSOR_TYPE_BMP180;
		uint32_t oss = 0;
		bmp180_get_cal_param(&p_bmp180);
	#endif
	
	LL_GPIO_SetOutputPin(GPIO_LED, PIN_GREEN_LED);
	LL_USART_EnableIT_RXNE(USART1);

	#ifdef WET_SENSOR
		data_pack[0].chunk_hdr.id = CHUNK_ID_WETSENS;
		data_pack[0].chunk_hdr.type = DATA_TYPE_UINT16;
		data_pack[0].chunk_hdr.payload_sz = 2;
		
		chunk_cnt = 1;
	
		uint16_t null = 0;
		memcpy_u8(&null, data_pack[0].data, 2);
		sensor_type = SENSOR_TYPE_WETSENS;
		
		LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_13);
	#endif
	
	hdr.protocol = PROTOCOL_AURA;
	hdr.src = uid_hash();
	hdr.dest = PC_ID;
	hdr.cnt = 0;
	
	whoami_pack.chunk_hdr.id = CHUNK_ID_TYPE;
	whoami_pack.chunk_hdr.type = DATA_TYPE_UINT32;
	whoami_pack.chunk_hdr.payload_sz = 4;
	memcpy_u8(&sensor_type, whoami_pack.data, 4);
	
	GPIO_EXTI_Enable();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		#ifdef LM75BD
			lm75bd_temp = lm75bd_read_temp();
			memcpy_u8(&lm75bd_temp, data_pack[0].data, 4);
		#endif
		
		#ifdef ZS05
			zs05_read(zs05_data);
			memcpy_u8(&zs05_data[0], data_pack[0].data, 4);
			memcpy_u8(&zs05_data[1], data_pack[1].data, 4);
			LL_mDelay(500);
		#endif
		
		#ifdef BMP180
			bmp180_get_temp(&p_bmp180);
			bmp180_get_press(&p_bmp180, oss);
			memcpy_u8(&p_bmp180.temp, data_pack[0].data, 4);
			memcpy_u8(& p_bmp180.press, data_pack[1].data, 4);
		#endif
		
		if(flags.usart1_rx_end&&!flags.usart1_tx_busy)
		{
			flags.usart1_tx_busy = 1;
			hdr.cmd = cmd;
			
			switch(hdr.cmd)
			{
				case CMD_ANS_WHOAMI:
					chunk_cnt = usart_start_data_sending (&hdr, &whoami_pack, &pack_crc, SENSOR_TYPE_NONE);
					break;
				case CMD_ANS_DATA:
					chunk_cnt = usart_start_data_sending (&hdr, data_pack, &pack_crc, sensor_type);
					break;
				default:
					break;
			};
		}


    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_0);
  while(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_0)
  {
  }
  LL_RCC_HSE_Enable();

   /* Wait till HSE is ready */
  while(LL_RCC_HSE_IsReady() != 1)
  {

  }
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE_DIV_1, LL_RCC_PLL_MUL_2);
  LL_RCC_PLL_Enable();

   /* Wait till PLL is ready */
  while(LL_RCC_PLL_IsReady() != 1)
  {

  }
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {

  }
  LL_Init1msTick(16000000);
  LL_SetSystemCoreClock(16000000);
  LL_RCC_SetUSARTClockSource(LL_RCC_USART1_CLKSOURCE_PCLK1);
  LL_RCC_SetI2CClockSource(LL_RCC_I2C1_CLKSOURCE_SYSCLK);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
		LL_GPIO_SetOutputPin(GPIO_LED, PIN_RED_LED);
		LL_mDelay(500);
		LL_GPIO_ResetOutputPin(GPIO_LED, PIN_RED_LED);
		LL_mDelay(500);
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
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
