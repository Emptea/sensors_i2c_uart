#include "tmp112.h"

void tmp112_read(uint8_t* data, uint32_t nbytes)
{
	if(i2c1_start_read(TMP112_ADDR,nbytes))
	{
		for (uint32_t i = 0; i < nbytes; i++)
		{
			LL_I2C_AcknowledgeNextData(I2C1, LL_I2C_ACK);
			while(!LL_I2C_IsActiveFlag_RXNE(I2C1)){};
			*data++ = LL_I2C_ReceiveData8(I2C1);
		}
	}
	LL_I2C_GenerateStopCondition(I2C1);
}

int32_t two_complement_13bits(uint32_t num)
{
	int32_t two_complement = (~num) + 1;
	if (num & 0x80)
	{
		two_complement = (((-1)*two_complement)& 0x1FFF);
		return two_complement;
	}
	else 
		return (int32_t)num;
}

float tmp112_read_temp(void)
{
	uint32_t byte_temp = 0;
	tmp112_read ((uint8_t *)byte_temp, 2);
	int32_t signed_temp = two_complement_13bits(byte_temp);
	
	return (float)(signed_temp*0.0625f);
}
