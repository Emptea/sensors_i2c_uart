#include "i2c.h"

#define ZS05_ADDR 0x5C

struct zs05_data
{
	uint32_t temp;
	uint32_t hum;
	uint32_t crc;
};

void zs05_read(struct zs05_data *res);
