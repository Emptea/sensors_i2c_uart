#include "lm75bd.h"

static uint32_t lm75bd_read(uint8_t* data, uint32_t nbytes)
{
	/* Master Generate Start condition for a read request :              */
	/*    - to the Slave with a 7-Bit SLAVE_OWN_ADDRESS                   */
	/*    - with a auto stop condition generation when transmit all bytes */
	uint32_t cnt = 0;
	LL_I2C_HandleTransfer(I2C1, (LM75BD_ADDR << 1), LL_I2C_ADDRSLAVE_7BIT, 2, LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_READ);
//	LL_I2C_SetSlaveAddr(I2C1, (LM75BD_ADDR << 1));
//	LL_I2C_SetTransferRequest(I2C1, LL_I2C_REQUEST_READ);
//	LL_I2C_SetTransferSize(I2C1, nbytes);
//	
  LL_I2C_GenerateStartCondition(I2C1);
	

//	LL_I2C_AcknowledgeNextData(I2C1, LL_I2C_ACK);
	while(LL_I2C_IsActiveFlag_RXNE(I2C1) & LL_I2C_IsActiveFlag_BUSY(I2C1));
	*data++ = LL_I2C_ReceiveData8(I2C1);
	
	LL_I2C_AcknowledgeNextData(I2C1, LL_I2C_NACK);
	while(LL_I2C_IsActiveFlag_RXNE(I2C1) & LL_I2C_IsActiveFlag_BUSY(I2C1));
	*data++ = LL_I2C_ReceiveData8(I2C1);
	
	while(LL_I2C_IsActiveFlag_BUSY(I2C1));
	LL_I2C_GenerateStopCondition(I2C1);
	if(LL_I2C_IsActiveFlag_STOP(I2C1))
  {
    /* End of Transfer */
    LL_I2C_ClearFlag_STOP(I2C1);
	}
	
	//while(LL_I2C_IsActiveFlag_BUSY(I2C1));
	//LL_I2C_GenerateStopCondition(I2C1);
	return (cnt == nbytes);
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
	uint8_t byte_temp[2] = {0};
	int32_t signed_temp = 0;
	if (lm75bd_read(byte_temp, 2)); //signed_temp = two_complement_11bits(byte_temp);
	
	return (float)(signed_temp*0.125f);
}

