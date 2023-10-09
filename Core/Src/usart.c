/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
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
#include "usart.h"
#include "i2c.h"
#include "lm75bd.h"
#include "tmp112.h"
#include "sht3x_dis.h"
#include "zs05.h"
#include "bmp180.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  LL_USART_InitTypeDef USART_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_USART1);

  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
  /**USART1 GPIO Configuration
  PA1   ------> USART1_DE
  PA2   ------> USART1_TX
  PA3   ------> USART1_RX
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_1;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_1;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LL_GPIO_PIN_2;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_1;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LL_GPIO_PIN_3;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_1;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  USART_InitStruct.BaudRate = 115200;
  USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
  USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
  USART_InitStruct.Parity = LL_USART_PARITY_NONE;
  USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
  USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
  USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
  LL_USART_Init(USART1, &USART_InitStruct);
  LL_USART_DisableIT_CTS(USART1);
  LL_USART_ConfigAsyncMode(USART1);
  LL_USART_Enable(USART1);
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/* USER CODE BEGIN 1 */

static void usart_send (const void *s, uint32_t len)
{
	do
	{
		LL_USART_TransmitData8(USART1, (*(uint8_t *)s++));
	} while(len--);
}

//static void usart_receive (uint8_t *data, uint32_t len)
//{
//	do
//	{
//		*data++ = LL_USART_ReceiveData8(USART1);
//	} while (len--);
//}

//static void usart_work (enum mode mode, usart_header *header, uint32_t uid)
//{
//	// should be usart_receive((uint8_t *)header, 32) before this func
//	switch(mode)
//	{
//		case MODE_IDLE:
//			if (header->protocol == PROTOCOL_AURA && header->dest == uid)
//				mode = MODE_RECEIVE;
//			break;
//		case MODE_RECEIVE:
//		{
//			
//			break;
//		}
//		default:
//			break;
//	}
//}

void usart_create_data(usart_data_header *data_header, uint32_t uid)
{
	static uint32_t cnt = 0;

	data_header->header.protocol = PROTOCOL_AURA;
	data_header->header.cnt = ++cnt;
	data_header->header.dist = 0;
	data_header->header.flags = 0x2; //0b010
	data_header->header.src = uid;
	data_header->header.dest = PC_ID;
	
}



void usart_whoami(usart_data_header *data_header)
{
	data_header->chunk_header.type = DATA_TYPE_CHAR;
	data_header->chunk_header.payload_sz = 0;
	
	uint16_t crc = crc16(0, &(data_header->header), HEADER_SIZE);	
	crc = crc16(crc, &(data_header->chunk_header), CHUNK_HEADER_SIZE);
	SET_BIT(data_header->header.flags, WHOAMI_BIT);
	
	usart_send(&(data_header->header), HEADER_SIZE);
	usart_send(&(data_header->chunk_header), CHUNK_HEADER_SIZE);
	usart_send(&crc, 2);
	
	CLEAR_BIT(data_header->header.flags, WHOAMI_BIT);
}


uint32_t usart_init(usart_data_header *data_header)
{
	MX_USART1_UART_Init();
	
	data_header->header.src = uid_hash();
	
	usart_whoami(data_header);
	
	return data_header->header.src;
}

void usart_get_src()
{
	
}
/* USER CODE END 1 */
