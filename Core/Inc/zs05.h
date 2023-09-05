#include "i2c.h"

#define ZS05_ADDR 0xB8

struct zs05_data
{
	uint32_t temp;
	uint32_t hum;
	uint32_t crc;
};
