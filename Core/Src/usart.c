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

  /* USART1 interrupt Init */
  NVIC_SetPriority(USART1_IRQn, 0);
  NVIC_EnableIRQ(USART1_IRQn);

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
  LL_USART_EnableDEMode(USART1);
  LL_USART_SetDESignalPolarity(USART1, LL_USART_DE_POLARITY_HIGH);
  LL_USART_SetDEAssertionTime(USART1, 0);
  LL_USART_SetDEDeassertionTime(USART1, 0);
  LL_USART_ConfigAsyncMode(USART1);
  LL_USART_Enable(USART1);
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/* USER CODE BEGIN 1 */

static void usart_create_data(usart_data_header *hdr)
{
	static uint32_t cnt = 0;

	hdr->header.protocol = PROTOCOL_AURA;
	hdr->header.cnt = cnt;
	cnt = (cnt+1)&0xFFFF; //0...65535
	hdr->header.dest = PC_ID;
}


static uint32_t usart_sending (const void *data, uint32_t sz)
{
	static uint32_t cnt = 0;
	
	if (sz == 0)
		return 1;
	
	LL_USART_TransmitData8(USART1,*(uint8_t *)(data+cnt));
	cnt++;
	
	if (cnt > sz - 1) {
		cnt = 0;
		return 1;
	}
	
	return 0;
}

void usart_txe_callback(usart_packet *pack)
{
	static enum usart_send_state usart_send_state = STATE_SEND_HEADER;
	
	switch(usart_send_state)
	{
		case STATE_SEND_HEADER:
			usart_send_state += usart_sending(pack, HEADER_SIZE);
			break;
		case STATE_SEND_DATA:
			if(flags.whoami) usart_send_state += usart_sending(&pack->whoami, pack->hdr.chunk_header.payload_sz);
			else usart_send_state += usart_sending(&pack->data, pack->hdr.chunk_header.payload_sz);
			break;
		case STATE_SEND_CRC:
			usart_send_state += usart_sending(&pack->crc, 2);
			break;
		case STATE_END:
			LL_USART_DisableIT_TXE(USART1);
			LL_USART_EnableIT_TC(USART1);
			usart_send_state = 	STATE_SEND_HEADER;
			break;
	}
}

/** RXNE FUNCTIONS **/
static uint32_t usart_receiving(uint8_t *data, USART_TypeDef *USARTx, uint32_t sz)
{
	static uint32_t counter = {0};
	
	if (sz == 0) 
		return 1;
	
	if(LL_USART_IsActiveFlag_RXNE(USARTx) && LL_USART_IsEnabledIT_RXNE(USARTx))
  {
		*(data + counter) = LL_USART_ReceiveData8(USARTx);
		counter++;
		
		if (counter > sz - 1)
		{
			counter = 0;
			return 1;
		}
	}
	return 0;
}
static uint32_t check_req(usart_packet *pack)
{
	uint16_t checking_crc =  crc16(0xFFFF, pack ,HEADER_SIZE+pack->hdr.chunk_header.payload_sz);
	return (pack->crc == checking_crc) && (pack->hdr.chunk_header.type == 0)&&(pack->hdr.chunk_header.payload_sz == 0);
}

static void rxne_check_whoami (usart_packet *pack, struct flags *flags)
{
	flags->whoami = (pack->hdr.chunk_header.id == 0);
}

uint32_t usart_rxne_callback(usart_packet *pack, struct flags *flags, USART_TypeDef *USARTx)
{
	static enum usart_rcv_state usart_rcv_state = {STATE_RCV_HEADER};
	
	switch (usart_rcv_state){
		case STATE_RCV_HEADER:
			usart_rcv_state += usart_receiving((uint8_t *)pack, USARTx, HEADER_SIZE);
			break;
		case STATE_RCV_DATA:
			usart_rcv_state += usart_receiving((uint8_t *)&pack->data, USARTx, pack->hdr.chunk_header.payload_sz);
			break;
		case STATE_RCV_CRC:
			usart_rcv_state -= 2*usart_receiving((uint8_t *)&pack->crc, USARTx, 2);
			if (usart_rcv_state == STATE_RCV_HEADER && check_req(pack))
			{
				rxne_check_whoami(pack, flags);
				return 1;
			}
			break;
	}
	
	return 0;
}




/* USER CODE END 1 */
