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
#include "main.h"
#include "uid_hash.h"
#include "crc16.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */
#define PC_ID 0x00000000

#define HEADER_SIZE 32
#define CHUNK_HEADER_SIZE 6
#define WHOAMI_BIT 2

#define PROTOCOL_AURA 0x41525541

typedef struct usart_header
{
	uint32_t protocol;
	uint32_t cnt;
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

// should be sent before sending data
typedef struct usart_chunk_head
{
	uint32_t id: 16;
	uint32_t type: 16;
	uint32_t payload_sz: 16;
} usart_chunk_head;

typedef struct usart_data_header
{
	usart_header header;
	usart_chunk_head chunk_header;
} usart_data_header;

enum mode
{
	MODE_IDLE,
	MODE_RECEIVE,
};

/* USER CODE END Private defines */

void MX_USART1_UART_Init(void);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

