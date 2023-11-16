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
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lm75bd.h"
#include "tmp112.h"
#include "sht3x_dis.h"
#include "zs05.h"
#include "bmp180.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//#define LM75BD
#define ZS05
//#define BMP180

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

struct zs05_data zs05_data = {0};
struct p_bmp180 p_bmp180 = {0};


uint32_t adresses[] = {LM75BD_ADDR, TMP112_ADDR, SHT3X_DIS_ADDR, ZS05_ADDR, BMP180_ADDR};
usart_packet pack = {0};
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
  MX_I2C1_Init();
  //MX_USART1_UART_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
	usart_init(&pack);
	
	#ifdef LM75BD
		pack.chunk_header.id = SENSOR_TYPE_LM75BD;
		lm75bd_read_temp();
		pack.chunk_header.type = DATA_TYPE_FLOAT;
		pack.data.temp = lm75bd_read_temp();
		pack.data.hum_or_press = 0;
	#endif
		
	#ifdef ZS05
		pack.chunk_header.id = SENSOR_TYPE_ZS05;
		pack.chunk_header.type = DATA_TYPE_FLOAT;
		pack.chunk_header.payload_sz = DATA_SIZE;
		LL_mDelay(4000);
		while(!zs05_read(&pack.data))
			LL_mDelay(500);
	#endif
		
	#ifdef BMP180
		pack.chunk_header.id = SENSOR_TYPE_BMP180;
		uint32_t oss = 0;
		bmp180_get_cal_param(&p_bmp180);
		bmp180_get_temp(&p_bmp180);
		bmp180_get_press(&p_bmp180, oss);
		pack.chunk_header.id = SENSOR_TYPE_BMP180;
		pack.chunk_header.type = DATA_TYPE_FLOAT;
		pack.chunk_header.payload_sz = DATA_SIZE;
		pack.data.temp = p_bmp180.temp;
		pack.data.hum_or_press = p_bmp180.press;
	#endif

//	LL_TIM_EnableIT_UPDATE(TIM2);
//	LL_TIM_EnableCounter(TIM2);

//	LL_TIM_EnableCounter(TIM3);
//	LL_TIM_ClearFlag_UPDATE(TIM3);
//	LL_TIM_EnableIT_UPDATE(TIM3);
	
	LL_USART_EnableIT_RXNE(USART1);
	LL_USART_EnableIT_TXE(USART1);
	LL_USART_EnableIT_TC(USART1);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		#ifdef LM75BD
			pack.chunk_header.type = DATA_TYPE_FLOAT;
			pack.data.temp = lm75bd_read_temp();
			pack.data.hum_or_press = 0;
		#endif
		
		#ifdef ZS05
			zs05_read(&pack.data);
			LL_mDelay(500);
		#endif
		
		#ifdef BMP180
			bmp180_get_temp(&p_bmp180);
			bmp180_get_press(&p_bmp180, oss);
			pack.chunk_header.id = SENSOR_TYPE_BMP180;
			pack.chunk_header.type = DATA_TYPE_FLOAT;
			pack.chunk_header.payload_sz = DATA_SIZE;
			pack.data.temp = p_bmp180.temp;
			pack.data.hum_or_press = p_bmp180.press;
		#endif
		
		if(flags.usart1_rx_end&&!flags.usart1_tx_busy)
		{
			flags.usart1_tx_busy = 1;
			usart_txe_callback(&pack);
			LL_USART_EnableIT_TXE(USART1);
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
		LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_13);
		LL_mDelay(500);
		LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_13);
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
