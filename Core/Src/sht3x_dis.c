#include "sht3x_dis.h"

void i2c1_send (uint8_t data)
{
	while (!LL_I2C_IsActiveFlag_TXIS(I2C1))
	{
		if(LL_I2C_IsActiveFlag_NACK(I2C1)) return;
	}
	LL_I2C_TransmitData8(I2C1, data);
}

void sht3x_dis_send_cmd (uint32_t cmd)
{
	LL_I2C_SetSlaveAddr(I2C1,SHT3X_DIS_ADDR);
	LL_I2C_SetTransferRequest(I2C1, LL_I2C_REQUEST_WRITE);
	LL_I2C_SetTransferSize(I2C1, 2);
	
	LL_I2C_GenerateStartCondition(I2C1);
	i2c1_send((uint8_t)(cmd<<8));
	i2c1_send((uint8_t)cmd);
}

uint32_t check_crc (uint32_t crc)
{
	
}

void data_processing (struct sht3x_dis_temp_hum processed ,uint32_t *buf)
{
	processed.temp = *buf++ << 8;
	processed.temp &= *buf++;
	processed.crc_temp = *buf++;
	processed.hum = *buf++ << 8;
	processed.hum &= *buf++;
	processed.crc_hum = *buf++;
}

void sht3x_dis_single_meas_no_stretching (struct sht3x_dis_temp_hum data,enum meas_mode meas_mode)
{
	static uint32_t cmd = 0;
	uint32_t buf[6] = {0};
	switch (meas_mode)
	{
		case HIGH:
			cmd = SHT3X_DIS_CMD_SINGLE_SHOT_HIGH;
		case MEDIUM:
			cmd = SHT3X_DIS_CMD_SINGLE_SHOT_MEDIUM;
		default:
			cmd = SHT3X_DIS_CMD_SINGLE_SHOT_LOW;
	}
	
	sht3x_dis_send_cmd(cmd);
	LL_I2C_SetSlaveAddr(I2C1,SHT3X_DIS_ADDR);
	LL_I2C_SetTransferRequest(I2C1, LL_I2C_REQUEST_READ);
	LL_I2C_SetTransferSize(I2C1, 6);
	
	LL_I2C_GenerateStartCondition(I2C1);
	
	for (uint32_t i = 0; i < 6; i++)
	{
		LL_I2C_AcknowledgeNextData(I2C1, LL_I2C_ACK);
		while(!LL_I2C_IsActiveFlag_RXNE(I2C1)){};
		buf[i] = LL_I2C_ReceiveData8(I2C1);
	}
}



