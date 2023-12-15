/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f0xx_it.c
  * @brief   Interrupt Service Routines.
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
#include "stm32f0xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "gpio.h"
#include "sensors.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M0 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
  while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
		LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_13);
		LL_mDelay(500);
		LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_13);
		LL_mDelay(500);
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVC_IRQn 0 */

  /* USER CODE END SVC_IRQn 0 */
  /* USER CODE BEGIN SVC_IRQn 1 */

  /* USER CODE END SVC_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */

  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F0xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f0xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles EXTI line 4 to 15 interrupts.
  */
void EXTI4_15_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI4_15_IRQn 0 */

  /* USER CODE END EXTI4_15_IRQn 0 */
  if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_13) != RESET)
  {
    LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_13);
    /* USER CODE BEGIN LL_EXTI_LINE_13 */
		if (!LL_GPIO_IsInputPinSet(GPIO_BTN_EXTI, PIN_EXTI))
		{
			wetsens_state = WETSENS_STATE_WET;
		}
		else
		{
			wetsens_state = WETSENS_STATE_DRY;
		}
		
    /* USER CODE END LL_EXTI_LINE_13 */
  }
  if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_14) != RESET)
  {
    LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_14);
    /* USER CODE BEGIN LL_EXTI_LINE_14 */
		if (!flags.usart1_tx_busy)
		{
			flags.usart1_tx_busy = 1;
			send_hdr.cmd = CMD_ANS_DATA;
			chunk_cnt = usart_start_data_sending (&send_hdr, data_pack, &pack_crc, sensor_type);
		}
    /* USER CODE END LL_EXTI_LINE_14 */
  }
  /* USER CODE BEGIN EXTI4_15_IRQn 1 */

  /* USER CODE END EXTI4_15_IRQn 1 */
}

/**
  * @brief This function handles USART1 global interrupt / USART1 wake-up interrupt through EXTI line 25.
  */
void USART1_IRQHandler(void)
{	
  /* USER CODE BEGIN USART1_IRQn 0 */
    LL_GPIO_ResetOutputPin(GPIO_LED, PIN_RED_LED);
	/**TRANSMISSION**/
	if(LL_USART_IsActiveFlag_TXE(USART1) && LL_USART_IsEnabledIT_TXE(USART1) && flags.usart1_tx_busy)
	{
		switch(send_hdr.cmd)
		{
			case CMD_ANS_WHOAMI:
				usart_txe_callback(&send_hdr, &whoami_pack, pack_crc, chunk_cnt);
				break;
			case CMD_ANS_DATA:
				usart_txe_callback(&send_hdr, data_pack, pack_crc, chunk_cnt);
				break;
			default:
				break;
		};
	}
	
	if (LL_USART_IsActiveFlag_TC(USART1) && LL_USART_IsEnabledIT_TC(USART1))
	{
		LL_USART_ClearFlag_TC(USART1);
		flags.usart1_tx_busy = 0;
		LL_USART_DisableIT_TC(USART1);
	}

	/**RECEPTION**/
	static usart_header recv_hdr = {0};
	static usart_packet recv_pack = {0};
	
	flags.usart1_rx_end = usart_rxne_callback(&recv_hdr, &recv_pack, &cmd, USART1);

    /**TIMEOUT**/
    if (LL_USART_IsActiveFlag_RTO(USART1)) {
        LL_USART_ClearFlag_RTO(USART1);
        usart_recv_timeout_callback(USART1);
    }
    
    if(LL_USART_IsActiveFlag_ORE(USART1))
    {
        LL_USART_ClearFlag_ORE(USART1);
        LL_GPIO_SetOutputPin(GPIO_LED, PIN_RED_LED);
    }
    
    if(flags.usart1_rx_end&&!flags.usart1_tx_busy)
    {
        flags.usart1_tx_busy = 1;
        send_hdr.cmd = cmd;
        
        switch(send_hdr.cmd)
        {
            case CMD_ANS_WHOAMI:
                LL_mDelay(1);
                chunk_cnt = usart_start_data_sending (&send_hdr, &whoami_pack, &pack_crc, SENSOR_TYPE_NONE);
                break;
            case CMD_ANS_DATA:
                LL_mDelay(1);
                chunk_cnt = usart_start_data_sending (&send_hdr, data_pack, &pack_crc, sensor_type);
                break;
            default:
                break;
        };
    }

}

/* USER CODE BEGIN 1 */

void TIM2_IRQHandler(void)
{
    if(LL_TIM_IsActiveFlag_UPDATE(TIM2))
    {
    /* Clear the update interrupt flag*/
    LL_TIM_ClearFlag_UPDATE(TIM2);
    }
  
    /* TIM2 update interrupt processing */
    sensors_measure(data_pack);
    flags.start_meas = 1;
}
/* USER CODE END 1 */
