#include "i2c.h"

#define BMP180_ADDR	0x77

#define BMP180_CONTROL_REG_ADDR 0xF4
#define BMP180_CMD_MEAS_TEMP 0x2E
#define BMP180_CMD_MEAS_PRESS 0x34

#define BMP180_EEPROM_REG_START_ADDR 0xAA
#define BMP180_EEPROM_REG_END_ADDR 0xBF

#define BMP180_INVALID_DATA 0U
#define BMP180_SUCCESS 1U

struct p_bmp180
{
	float temp;
	float press;
	int32_t ut;
	int32_t up;
	union {
		struct
		{
			int32_t ac1;
			int32_t ac2;
			int32_t ac3;
			uint32_t ac4;
			uint32_t ac5;
			uint32_t ac6;
			int32_t b1;
			int32_t b2;
			int32_t mb;
			int32_t mc;
			int32_t md;
		} params;
		uint16_t raw_data[2*11];
	} calibr;
};

void bmp180_get_cal_param(struct p_bmp180 *p_bmp180);
void bmp180_get_uncomp_temp(struct p_bmp180 *p_bmp180);
/*
\params oss - oversampling 0...3
*/
uint32_t bmp180_get_uncomp_press(uint32_t oss, struct p_bmp180 *p_bmp180);

uint32_t bmp180_get_temp(struct p_bmp180 *p_bmp180);
uint32_t bmp180_get_press(struct p_bmp180 *p_bmp180, uint32_t oss);

