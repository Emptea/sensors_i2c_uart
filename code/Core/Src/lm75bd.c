#include "lm75bd.h"
#include "i2c_ex.h"

static uint32_t lm75bd_read(uint8_t *data, uint32_t nbytes)
{
    /* Master Generate Start condition for a read request :              */
    /*    - to the Slave with a 7-Bit SLAVE_OWN_ADDRESS                   */
    /*    - with a auto stop condition generation when transmit all bytes */
    uint32_t cnt = 0;
    LL_I2C_HandleTransfer(I2C1, (LM75BD_ADDR << 1), LL_I2C_ADDRSLAVE_7BIT, nbytes, LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_READ);

    for (uint32_t i = 0; i < nbytes - 1; i++) {
        while (!LL_I2C_IsActiveFlag_RXNE(I2C1))
            ;
        *data-- = LL_I2C_ReceiveData8(I2C1);
        cnt++;
    }

    while (!LL_I2C_IsActiveFlag_RXNE(I2C1))
        ;
    *data-- = LL_I2C_ReceiveData8(I2C1);
    cnt++;
    return (cnt == nbytes);
}

static int32_t two_complement_11bits(uint16_t num)
{
    num = (num & 0xFFE0) >> 5;
    int32_t two_complement = (~num) + 1;
    if (num & 0x400) {
        two_complement = (((-1) * two_complement) & 0x7FF);
        return two_complement;
    } else {
        return (int32_t)num;
    }
}

float lm75bd_read_temp(void)
{
    static union {
        uint8_t array[2];
        uint16_t num;
    } temp = {0};

    int32_t signed_temp = 0;
    if (i2c1_read((temp.array + 1), LM75BD_ADDR, 2)) {
        signed_temp = two_complement_11bits(temp.num);
    }

    return (float)(signed_temp * 0.125f);
}
