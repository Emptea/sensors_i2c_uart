#include "i2c.h"

#define ZS05_ADDR 0x5C

extern struct zs05_data
{
	uint32_t temp;
	uint32_t hum;
	uint32_t crc;
} zs05_data;

uint32_t zs05_read(float res[2]);
