#include "bmp180.h"

/*
\params mode - BMP180_CMD_MEAS_TEMP
							 BMP180_CMD_MEAS_PRESS + (oss << 6)
*/

void bmp180_start_meas(uint32_t mode)
{
	uint32_t cnt = 0;
	LL_I2C_HandleTransfer(I2C1, (BMP180_ADDR << 1), LL_I2C_ADDRSLAVE_7BIT, 2, LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_WRITE);
	
	while(!LL_I2C_IsActiveFlag_TXIS(I2C1));
	LL_I2C_TransmitData8(I2C1, 0xF4);
	while(!LL_I2C_IsActiveFlag_TXIS(I2C1));
	LL_I2C_TransmitData8(I2C1, mode);
}

void bmp180_get_cal_param(struct p_bmp180 *p_bmp180)
{
	uint32_t i = 0;
	
	for (uint32_t addr = BMP180_EEPROM_REG_START_ADDR; addr < BMP180_EEPROM_REG_END_ADDR; addr = addr + 0x02)
	{
		i2c1_pointer_read(&p_bmp180->calibr.data.raw_data[i+1], BMP180_ADDR, addr, 2);
		i = i + 2;
	}
}

uint32_t bmp180_read_uncomp(void)
{

	uint8_t buf[2] = {0};
	i2c1_pointer_read(buf+1,BMP180_ADDR,0xF6,2);
	return ((buf[1] <<8) + buf[0]);
}

void bmp180_get_uncomp_temp(struct p_bmp180 *p_bmp180)
{
	bmp180_start_meas(BMP180_CMD_MEAS_TEMP);
	LL_mDelay(5);
	p_bmp180->ut = bmp180_read_uncomp();
}


/*
\params oss - oversampling 0...3
*/
uint32_t bmp180_get_uncomp_press(uint32_t oss, struct p_bmp180 *p_bmp180)
{
	if (oss > 3) return BMP180_INVALID_DATA;
	bmp180_start_meas(BMP180_CMD_MEAS_PRESS+(oss<<6));
	switch(oss)
	{
		case 1:
			LL_mDelay(8);
		case 2:
			LL_mDelay(14);
		case 3:
			LL_mDelay(26);
		default:
			LL_mDelay(5);
	}
	p_bmp180->up = bmp180_read_uncomp();
	return BMP180_SUCCESS;
}

uint32_t bmp180_get_temp(struct p_bmp180 *p_bmp180)
{
	bmp180_get_uncomp_temp(p_bmp180);
	int32_t x1 = (((p_bmp180->ut - p_bmp180->calibr.params.ac6)*p_bmp180->calibr.params.ac5) >> 15);
	if (x1 == 0 && p_bmp180->calibr.params.md == 0)
		return BMP180_INVALID_DATA;
	int32_t x2 = (p_bmp180->calibr.params.mc << 11)/(x1+p_bmp180->calibr.params.md);
	
	p_bmp180->temp = ((x1+x2+8) >> 4)*0.1;
	return BMP180_SUCCESS;
}

uint32_t bmp180_get_press(struct p_bmp180 *p_bmp180, uint32_t oss)
{
	if (!bmp180_get_uncomp_press(oss, p_bmp180)) 
		return BMP180_INVALID_DATA;
	
	int32_t x1 = (((p_bmp180->ut - p_bmp180->calibr.params.ac6)*p_bmp180->calibr.params.ac5) >> 15);
	if (x1 == 0 && p_bmp180->calibr.params.md == 0)
		return BMP180_INVALID_DATA;
	int32_t x2 = (p_bmp180->calibr.params.mc << 11)/(x1+p_bmp180->calibr.params.md);
	int32_t b6 = x1 + x2 - 4000;
	
	/** CALCULATE B3 **/
	x1 = ((b6*b6) >> 12)*p_bmp180->calibr.params.b2;
	x1 >>= 11;	
	x2 = ((p_bmp180->calibr.params.ac2*b6) >> 11);	
	int32_t x3 = x1 + x2;	
	int32_t b3 = ((((p_bmp180->calibr.params.ac1 * 4 + x3) << oss) + 2) >> 2);
	
	/** CALCULATE B4 **/	
	x1 = ((p_bmp180->calibr.params.ac3 * b6) >> 13);
	x2 = ((p_bmp180->calibr.params.b1*((b6 * b6) >> 12)) >> 16);
	x3 = ((x1 + x2 + 2) >> 2);
	uint32_t b4 = ((p_bmp180->calibr.params.ac4 *(uint32_t)(x3+32768)) >> 15);
	uint32_t b7 = (uint32_t) (p_bmp180->up - b3)*(50000 >> oss);
	
	if (b4 == 0) return BMP180_INVALID_DATA;	
	int32_t p = 0;

	if(b7 < 0x80000000)
		 p = (b7 << 1) / b4;
	else 
		p = ((b7 / b4) << 1);
	
	x1 = (p >> 8);
	x1 *= x1;
	x1 = ((x1 * 3038) >> 16);
	x2 = ((-7357*p) >> 16);
	
	/** PRESSURE IN Pa **/
	p += ((x1 + x2 + 3791) >> 4);
	p_bmp180->press = p;
	return BMP180_SUCCESS;
}

