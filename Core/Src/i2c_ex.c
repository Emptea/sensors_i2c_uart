#include "i2c_ex.h"

static uint32_t i2c1_init(uint8_t *addr)
{
    static uint32_t timeout = 10000;
    timeout = 10000;
    LL_I2C_HandleTransfer(I2C1, (*addr << 1), LL_I2C_ADDRSLAVE_7BIT, 1, LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_WRITE);
    while(!LL_I2C_IsActiveFlag_TXIS(I2C1))
    {
        if (!timeout--) 
            return 0;
        
        if (LL_I2C_IsActiveFlag_NACK(I2C1))
		{
			LL_I2C_ClearFlag_NACK(I2C1);
			return 0;     
		}
    }
    LL_I2C_TransmitData8(I2C1, 0);
    return 1;
}

uint32_t i2c1_scan(uint8_t *addreses, uint32_t cnt)
{
    static uint32_t timeout = 10000;
    for (uint32_t i = 1; i < cnt+1; i++)
    {
        if(i2c1_init(addreses++))
            return i;
    }
    return 0;
}

void i2c1_send (uint8_t data)
{	
	while (!LL_I2C_IsActiveFlag_TXIS(I2C1))
	{
		if(LL_I2C_IsActiveFlag_NACK(I2C1)) return;
	}
	LL_I2C_TransmitData8(I2C1, data);
}

uint32_t i2c1_read(uint8_t *data, uint32_t addr, uint32_t nbytes)
{
	uint32_t cnt = 0;
	LL_I2C_HandleTransfer(I2C1, (addr << 1), LL_I2C_ADDRSLAVE_7BIT, nbytes, LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_READ);
    uint32_t timeout = 100;
	for (uint32_t i = 0; i < nbytes; i++){
        while(!LL_I2C_IsActiveFlag_RXNE(I2C1))
        {
            if (!(timeout--)) return 0;
        }
		*data-- = LL_I2C_ReceiveData8(I2C1);
		cnt++;
	}
	
	return cnt;
}

uint32_t i2c1_pointer_read(uint8_t *data, uint32_t addr, uint32_t pointer, uint32_t nbytes)
{
    static uint32_t fault_cnt = 0;
    static uint32_t i2c_timeout = 10000;
	uint32_t cnt = 0;
	LL_I2C_HandleTransfer(I2C1, (addr << 1), LL_I2C_ADDRSLAVE_7BIT, 1, LL_I2C_MODE_SOFTEND, LL_I2C_GENERATE_START_WRITE);
	
    uint32_t timeout = 0;
	while(!LL_I2C_IsActiveFlag_TXIS(I2C1))
	{
        if (timeout++ >= i2c_timeout) 
            return 0;
		if (LL_I2C_IsActiveFlag_NACK(I2C1))
		{
			LL_I2C_ClearFlag_NACK(I2C1);
			return 0;     
		}
	}
	LL_I2C_TransmitData8(I2C1, (uint8_t)pointer);
	
	LL_I2C_HandleTransfer(I2C1, (addr << 1), LL_I2C_ADDRSLAVE_7BIT, nbytes, LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_RESTART_7BIT_READ);

	for (uint32_t i = 0; i < nbytes; i++){
        timeout = 0;
		while(!LL_I2C_IsActiveFlag_RXNE(I2C1))
        {
            if (timeout++ >= i2c_timeout)
                return 0;
        }
		*data-- = LL_I2C_ReceiveData8(I2C1);
        fault_cnt = 0;
		cnt++;
	}
	
	return cnt;
}