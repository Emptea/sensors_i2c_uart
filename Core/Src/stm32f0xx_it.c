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

#include "main.h"
#include "stm32f0xx_it.h"

#include "gpio.h"
#include "gpio_ex.h"
#include "usart_ex.h"
#include "sensors.h"


/******************************************************************************/
/*           Cortex-M0 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  while (1)
  {
        blink_red();
  }

}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  while (1)
  {
        blink_red();
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  while (1)
  {
        blink_red();
  }
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  while (1)
  {
        blink_red();
  }
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  while (1)
  {
        blink_red();
  }
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
    if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_13) != RESET)
    {
        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_13);
        if (!LL_GPIO_IsInputPinSet(GPIO_BTN_EXTI, PIN_EXTI))
        {
            wetsens_state = WETSENS_STATE_WET;
        }
        else
        {
            wetsens_state = WETSENS_STATE_DRY;
        }
    }
    
    if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_14) != RESET)
    {
        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_14);
        if (!flags.usart1_tx_busy)
        {
            flags.usart1_tx_busy = 1;
            send_hdr.cmd = CMD_ANS_DATA;
            chunk_cnt = usart_start_data_sending (&send_hdr, data_pack, &pack_crc, sensor_type);
        }
    }
}

/**
  * @brief This function handles USART1 global interrupt / USART1 wake-up interrupt through EXTI line 25.
  */
void USART1_IRQHandler(void)
{	
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
            case CMD_ANS_WRITE:
                usart_txe_callback(&send_hdr, &error_pack, pack_crc, chunk_cnt);
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
    #ifdef DELAY
        if(flags.usart1_rx_end) LL_TIM_EnableCounter(TIM2);
    #else
        usart_start_sending_routine(&send_hdr, &pack_crc, sensor_type);
    #endif

    
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
}

void TIM2_IRQHandler(void)
{
    if(LL_TIM_IsActiveFlag_UPDATE(TIM2))
    {
        LL_TIM_ClearFlag_UPDATE(TIM2);
        LL_TIM_DisableCounter(TIM2);
        LL_TIM_SetCounter(TIM2,0);
        usart_start_sending_routine(&send_hdr, &pack_crc, sensor_type);
    }
}
