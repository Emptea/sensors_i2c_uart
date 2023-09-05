#include "zs05.h"

uint32_t zs05_check_crc(uint32_t *data, uint32_t len,uint32_t sent_crc)
{
	uint32_t calculated_crc = 0;
	while(len--)
	{
		calculated_crc = (calculated_crc+*data++)&0xFF;
	}
	return (sent_crc == calculated_crc);
}

void zs05_data_processing(struct zs05_data *res, uint32_t *buf)
{
	res->temp = *buf++;
	if (*buf++ & 0x80) 
		res->temp = - res->temp;
	
	res->hum = *buf++;
}

void zs05_read(struct zs05_data *res)
{
	uint32_t buf[4] = {0};
	if (i2c1_start_read(ZS05_ADDR, 5))
	{
	for (uint32_t i = 0; i < 4; i++)
		{
			LL_I2C_AcknowledgeNextData(I2C1, LL_I2C_ACK);
			while(!LL_I2C_IsActiveFlag_RXNE(I2C1)){};
			buf[i]= LL_I2C_ReceiveData8(I2C1);
		}
	}
	// read last byte
	LL_I2C_AcknowledgeNextData(I2C1, LL_I2C_NACK);
	while(!LL_I2C_IsActiveFlag_RXNE(I2C1)){};
	res->crc = LL_I2C_ReceiveData8(I2C1);
	LL_I2C_GenerateStopCondition(I2C1);
		
	if(zs05_check_crc(buf, 4, res->crc))
		zs05_data_processing(res, buf);
}