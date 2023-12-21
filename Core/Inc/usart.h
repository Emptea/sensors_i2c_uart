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

#define PC_ID 0x00000000

#define HEADER_SIZE 20
#define CHUNK_HEADER_SIZE 4

#define PROTOCOL_AURA 0x41525541

#define DELAY

enum data_type
{
	DATA_TYPE_INT8 = 1,
	DATA_TYPE_UINT8,
	DATA_TYPE_INT16,
	DATA_TYPE_UINT16,
	DATA_TYPE_INT32,
	DATA_TYPE_UINT32,
	DATA_TYPE_FLOAT32,
	DATA_TYPE_FLOAR64,
	DATA_TYPE_STRING
};

enum chunk_id
{
	CHUNK_ID_TYPE = 1,
	CHUNK_ID_UIDS,
	CHUNK_ID_ON_OFF,
	CHUNK_ID_TEMP,
	CHUNK_ID_HUM,
	CHUNK_ID_PRESS,
	CHUNK_ID_WETSENS
};

enum sensor_type
{
	SENSOR_TYPE_NONE,
	SENSOR_TYPE_LM75BD = 1,
	SENSOR_TYPE_TMP112,
	SENSOR_TYPE_SHT30,
	SENSOR_TYPE_ZS05,
	SENSOR_TYPE_BMP180,
	SENSOR_TYPE_LPS22HB,
	SENSOR_TYPE_DOORKNOT,
	SENSOR_TYPE_EXPANDER,
	SENSOR_TYPE_WETSENS
};

enum cmd
{
	CMD_NONE,
	CMD_REQ_WHOAMI = 1,
	CMD_ANS_WHOAMI,
	CMD_REQ_DATA,
	CMD_ANS_DATA,
	CMD_CNT
};
extern enum cmd cmd;

/** FSMs **/
enum usart_rcv_state
{
	STATE_RCV_HEADER = 1,
	STATE_RCV_CHECK_SZ,
	STATE_RCV_CHUNK_HEADER,
	STATE_RCV_DATA,
	STATE_RCV_LOOP_CHUNK,
	STATE_RCV_CRC,
};

enum usart_send_state
{
	STATE_SEND_HDR = 1,
	STATE_SEND_CHUNK_HDR,
	STATE_SEND_DATA,
	STATE_SEND_LOOP_CHUNK,
	STATE_SEND_CRC,
	STATE_END,
};

/**UART STRUCTs **/
typedef struct
{
	uint32_t protocol;
	uint32_t cnt;
	uint32_t src;
	uint32_t dest;
	uint32_t cmd: 16;
	uint32_t data_sz : 16;
} usart_header;
extern usart_header send_hdr;

typedef struct
{
	uint32_t id: 8;
	uint32_t type: 8;
	uint32_t payload_sz : 16;
} usart_chunk_head;

typedef struct
{
	usart_chunk_head chunk_hdr;
	uint8_t data[16];
} usart_packet;
extern usart_packet data_pack[2], whoami_pack;

extern uint16_t pack_crc;

extern struct flags
{
	uint32_t usart1_tx_busy : 1;
	uint32_t usart1_rx_end : 1;
} flags;

extern uint32_t chunk_cnt;
extern uint32_t sensor_type;


void MX_USART1_UART_Init(void);

uint16_t usart_calc_crc (usart_header *hdr, usart_packet pack[], uint32_t chunk_cnt);
void usart_calc_data_sz (usart_header *hdr, usart_packet pack[], uint32_t chunk_cnt);

uint32_t usart_rxne_callback(usart_header *hdr, usart_packet pack[], enum cmd *cmd, USART_TypeDef *USARTx);
void usart_txe_callback(usart_header *hdr, usart_packet pack[], uint16_t crc_send, uint32_t pack_count);
void usart_send_pack (usart_header *hdr);

uint32_t usart_start_data_sending (usart_header *hdr, usart_packet pack[], uint16_t *crc, uint32_t sensor_type);

void usart_recv_timeout_callback(USART_TypeDef *USARTx);

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

