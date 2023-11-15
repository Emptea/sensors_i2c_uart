#include "zs05.h"

static uint32_t zs05_check_crc(uint8_t *data, uint32_t len, uint8_t sent_crc)
{
	uint32_t calculated_crc = 0;
	while(len--)
	{
		calculated_crc = (calculated_crc+*data++)&0xFF;
	}
	return (sent_crc == calculated_crc);
}

static void zs05_data_processing(struct data_pack *res, uint8_t *buf)
{
	if (*buf++ & 0x80) 	res->temp = -*buf++;
	else 	res->temp = *buf++;
	buf++;
	res->hum_or_press = *buf++;
	
//	res->temp = *buf++;
//	if (*buf++ & 0x80) 
//		res->temp = -res->temp;
}

uint32_t zs05_read(struct data_pack *res)
{
	uint8_t buf[5] = {0};
	i2c1_pointer_read(buf + 4, ZS05_ADDR, 0, 5);
	uint8_t crc = buf[0];
	LL_mDelay(50);	
	if(zs05_check_crc(buf+1, 4, crc))
	{
		zs05_data_processing(res, buf+1);
		return 1;
	}
	else return 0;
}
