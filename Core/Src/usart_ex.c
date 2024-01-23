#include "usart_ex.h"
#include "i2c.h"
#include "lm75bd.h"
#include "tmp112.h"
#include "sht3x_dis.h"
#include "zs05.h"
#include "bmp180.h"

static enum usart_rcv_state usart_rcv_state = {STATE_RCV_HEADER};
static uint32_t recv_cnt = {0};
static uint16_t crc = 0;

static void usart_stop_recv(void)
{
    LL_USART_DisableRxTimeout(USART1);
}

void usart_recv_timeout_callback(USART_TypeDef *USARTx)
{
    usart_rcv_state = STATE_RCV_HEADER;
    recv_cnt = 0;
    usart_stop_recv();
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

void usart_txe_callback(usart_header *hdr, usart_packet pack[], uint16_t crc_send, uint32_t pack_count)
{
	static enum usart_send_state usart_send_state = STATE_SEND_HDR;
	static uint32_t pack_counter = 0;
	
	switch(usart_send_state)
	{
		case STATE_SEND_HDR:
			usart_send_state += usart_sending(hdr, HEADER_SIZE);
			pack_counter = 0;
			break;
		case STATE_SEND_CHUNK_HDR:
			usart_send_state += usart_sending(&pack[pack_counter].chunk_hdr, CHUNK_HEADER_SIZE);
			break;
		case STATE_SEND_DATA:
			usart_send_state += usart_sending(&pack[pack_counter].data, pack[pack_counter].chunk_hdr.payload_sz);
			break;
		case STATE_SEND_LOOP_CHUNK:
			if(pack_counter < pack_count - 1)
				{
					usart_send_state = STATE_SEND_CHUNK_HDR;
					pack_counter++;
				}
				else
				{
					usart_send_state = STATE_SEND_CRC;
				}
			break;
		case STATE_SEND_CRC:
			usart_send_state += usart_sending(&crc_send, 2);
			break;
		case STATE_END:
			LL_USART_DisableIT_TXE(USART1);
			LL_USART_EnableIT_TC(USART1);
			hdr->cnt++;
			usart_send_state = 	STATE_SEND_HDR;
			break;
	}
}

/** RXNE FUNCTIONS **/
static uint32_t usart_receiving(uint8_t *data, USART_TypeDef *USARTx, uint32_t sz)
{
	if (sz == 0) 
		return 1;
	
	if(LL_USART_IsActiveFlag_RXNE(USARTx) && LL_USART_IsEnabledIT_RXNE(USARTx))
  {
		*(data + recv_cnt) = LL_USART_ReceiveData8(USARTx);
		recv_cnt++;
		
		if (recv_cnt > sz - 1)
		{
			recv_cnt = 0;
			return 1;
		}
	}
	return 0;
}

uint16_t usart_calc_crc (usart_header *hdr, usart_packet pack[], uint32_t chunk_cnt)
{
	uint16_t crc =  crc16(0xFFFF, hdr ,HEADER_SIZE);
	uint32_t cnt = 0;
	while(chunk_cnt--)
	{
		crc =  crc16(crc, &pack[cnt], CHUNK_HEADER_SIZE + pack[cnt].chunk_hdr.payload_sz);
		cnt++;
	}
	
	return crc;
}

static uint32_t check_crc(usart_header *hdr, usart_packet pack[], uint16_t crc, uint32_t chunk_cnt)
{
	uint16_t checking_crc =  usart_calc_crc(hdr, pack, chunk_cnt);
	return (crc == checking_crc);
}

static void enable_recv_timeout(USART_TypeDef *USARTx)
{
    if(LL_USART_IsActiveFlag_RXNE(USARTx) && LL_USART_IsEnabledIT_RXNE(USARTx)) 
        LL_USART_EnableRxTimeout(USARTx);
}
static uint32_t check_adr(usart_header *hdr)
{
    if (hdr->dest == send_hdr.src || hdr->dest == 0x00) return 1;
    else return 0;
}
    
uint32_t usart_rxne_callback(usart_header *hdr, usart_packet pack[], enum cmd *cmd, USART_TypeDef *USARTx)
{
	
	static uint32_t cnt = 0;
	static uint32_t chunk_cnt = 0;
	
	switch (usart_rcv_state){
		case STATE_RCV_HEADER:
            enable_recv_timeout(USARTx);
			usart_rcv_state += usart_receiving((uint8_t *)hdr, USARTx, HEADER_SIZE);
			cnt = 0;
			chunk_cnt = 0;
			crc = 0;
			break;
		case STATE_RCV_CHECK_SZ:
			if (!hdr->data_sz) usart_rcv_state = STATE_RCV_CRC;
			else usart_rcv_state = STATE_RCV_CHUNK_HEADER;
			break;
		case STATE_RCV_CHUNK_HEADER:
			usart_rcv_state += usart_receiving((uint8_t *)&pack[chunk_cnt].chunk_hdr, USARTx, CHUNK_HEADER_SIZE);
			cnt++;
			break;
		case STATE_RCV_DATA:
			usart_rcv_state += usart_receiving((uint8_t *)&pack[chunk_cnt].data, USARTx, pack[chunk_cnt].chunk_hdr.payload_sz);
			cnt++;
			break;
		case STATE_RCV_LOOP_CHUNK:
			if (cnt < hdr->data_sz-1)
			{
				usart_rcv_state = STATE_RCV_CHUNK_HEADER;
				chunk_cnt++;
			}
			else
				usart_rcv_state = STATE_RCV_CRC;
			break;
		case STATE_RCV_CRC:
			if (usart_receiving((uint8_t *)&crc, USARTx, 2))
			{
                usart_recv_timeout_callback(USARTx);
                if(check_crc(hdr, pack, crc, chunk_cnt)&&check_adr(hdr))
                {
                    *cmd = hdr->cmd+1; //switch from req to ans
                    usart_rcv_state = STATE_RCV_HEADER;
                    return 1;
                }
			}
			break;
	}
	
	return 0;
}

void usart_calc_data_sz (usart_header *hdr, usart_packet pack[], uint32_t chunk_cnt)
{
		do
		{
			hdr->data_sz += (CHUNK_HEADER_SIZE+pack[--chunk_cnt].chunk_hdr.payload_sz);
		} while(chunk_cnt);
}

uint32_t usart_start_data_sending (usart_header *hdr, usart_packet pack[], uint16_t *crc, uint32_t sensor_type)
{
	uint32_t cnt = 0;
	hdr->data_sz = 0;
	if(sensor_type > SENSOR_TYPE_TMP112 && sensor_type < SENSOR_TYPE_DOORKNOT)
		cnt = 2;
	else
		cnt = 1;
	usart_calc_data_sz(hdr, pack, cnt);
	
	*crc = usart_calc_crc(hdr, pack, cnt);
	
	usart_txe_callback(hdr, pack, *crc, cnt);			
	LL_USART_EnableIT_TXE(USART1);
	return cnt;
}