#ifndef __I2C_EX_H__
#define __I2C_EX_H__

#include "main.h"

#define I2C_GPIO GPIOB
#define I2C_SDA LL_GPIO_PIN_9
#define I2C_SCL LL_GPIO_PIN_8

void i2c1_send(uint8_t data);
uint32_t i2c1_read(uint8_t *data, uint32_t addr, uint32_t nbytes);
uint32_t i2c1_pointer_read(uint8_t *data, uint32_t addr, uint32_t pointer, uint32_t nbytes);
uint32_t i2c1_scan(uint8_t *addreses, uint32_t cnt);

inline static uint32_t i2c_is_on()
{
    return LL_GPIO_IsInputPinSet(I2C_GPIO, I2C_SCL) && LL_GPIO_IsInputPinSet(I2C_GPIO, I2C_SDA);
}
#endif /* __I2C_EX_H__ */
