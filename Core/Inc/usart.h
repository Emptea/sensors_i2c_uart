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

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_ll_usart.h"
#include "uid_hash.h"
#include "crc16.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */
#define PC_ID 0x00000000

#define HEADER_SIZE 32
#define CHUNK_HEADER_SIZE 6
#define DATA_SIZE 4*2
#define WHOAMI_BIT 2

#define PROTOCOL_AURA 0x41525541

typedef struct usart_header
{
	uint32_t protocol;
	uint32_t cnt : 16;
	uint32_t dist : 8;
	uint32_t flags : 8;
	uint32_t src;
	uint32_t dest;
	uint32_t path[4];
} usart_header;

enum data_type
{
	DATA_TYPE_CHAR = 1,
	DATA_TYPE_UCHAR,
	DATA_TYPE_SHORT,
	DATA_TYPE_USHORT,
	DATA_TYPE_INT,
	DATA_TYPE_UINT,
	DATA_TYPE_FLOAT,
	DATA_TYPE_DOUBLE,
	DATA_TYPE_STRING
};

enum sensor_type
{
	SENSOR_TYPE_LM75BD = 1,
	SENSOR_TYPE_TMP112,
	SENSOR_TYPE_SHT30,
	SENSOR_TYPE_ZS05,
	SENSOR_TYPE_BMP180
};

enum usart_rcv_state
{
	STATE_RCV_HEADER = 0,
	STATE_RCV_DATA,
	STATE_RCV_CRC,
};

enum usart_send_state
{
	STATE_SEND_HEADER = 0,
	STATE_SEND_DATA,
	STATE_SEND_CRC,
	STATE_END,
};

// should be sent before sending data
typedef struct __attribute__((packed)) usart_chunk_head
{
	uint32_t id: 16;
	uint32_t type: 16;
	uint16_t payload_sz;
} usart_chunk_head;

typedef struct  __attribute__((packed)) usart_data_header
{
	usart_header header;
	usart_chunk_head chunk_header;
} usart_data_header;

typedef struct __attribute__((packed)) data_pack
{
	float temp;
	float hum_or_press;
} data_pack;

typedef struct __attribute__((packed)) usart_packet
{
	usart_header header;
	usart_chunk_head chunk_header;
	data_pack data;
	uint16_t crc;
} usart_packet;
extern usart_packet pack;

extern struct flags
{
	uint32_t usart1_tx_busy : 1;
	uint32_t usart1_rx_end : 1;
} flags;

/* USER CODE END Private defines */

void MX_USART1_UART_Init(void);
void usart_init(usart_packet *pack);
void usart_whoami(usart_data_header *data_header);
void usart_create_data(usart_packet *pack);

void usart_send (const void *s, uint32_t len);

uint32_t usart_rxne_callback(usart_packet *pack, USART_TypeDef *USARTx);
void usart_txe_callback(usart_packet *pack);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

