#include "zs05.h"

uint32_t zs05_check_crc(uint8_t *data, uint32_t len,uint32_t sent_crc)
{
	uint32_t calculated_crc = 0;
	while(len--)
	{
		calculated_crc = (calculated_crc+*data++)&0xFF;
	}
	return (sent_crc == calculated_crc);
}

void zs05_data_processing(struct zs05_data *res, uint8_t *buf)
{
	res->hum = *buf++;
	buf++;
	
	res->temp = *buf++;
	if (*buf++ & 0x80) 
		res->temp = - res->temp;
	
}

void zs05_read(struct zs05_data *res)
{
	uint8_t buf[5] = {0};
	i2c1_pointer_read(buf, ZS05_ADDR, 0, 5);
	res->crc = buf[4];
		
	if(zs05_check_crc(buf, 4, res->crc))
		zs05_data_processing(res, buf);
}