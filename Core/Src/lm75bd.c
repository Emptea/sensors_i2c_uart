#include "lm75bd.h"

static uint32_t lm75bd_read(uint8_t* data, uint32_t nbytes)
{
	if(i2c1_start_read(LM75BD_ADDR, nbytes))
	{
		for (uint32_t i = 0; i < nbytes - 1; i++)
		{
			LL_I2C_AcknowledgeNextData(I2C1, LL_I2C_ACK);
			while(!LL_I2C_IsActiveFlag_RXNE(I2C1)){};
			*data++ = LL_I2C_ReceiveData8(I2C1);
		}
		
		// read last byte
		LL_I2C_AcknowledgeNextData(I2C1, LL_I2C_NACK);
		while(!LL_I2C_IsActiveFlag_RXNE(I2C1)){};
		*data++ = LL_I2C_ReceiveData8(I2C1);
		LL_I2C_GenerateStopCondition(I2C1);
		return 1;
	}
	return 0;
}


static int32_t two_complement_11bits(uint32_t num)
{
	int32_t two_complement = (~num) + 1;
	if (num & 0x80)
	{
		two_complement = (((-1)*two_complement)& 0x7FF);
		return two_complement;
	}
	else 
		return (int32_t) num;
}

float lm75bd_read_temp(void)
{
	uint32_t byte_temp = 0;
	lm75bd_read((uint8_t *)byte_temp, 2);
	int32_t signed_temp = two_complement_11bits(byte_temp);
	
	return (float)(signed_temp*0.125f);
}

