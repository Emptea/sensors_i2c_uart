#ifndef __I2C_EX_H__
#define __I2C_EX_H__

#include "main.h"

void i2c1_send(uint8_t data);
uint32_t i2c1_read(uint8_t *data, uint32_t addr, uint32_t nbytes);
uint32_t i2c1_pointer_read(uint8_t *data, uint32_t addr, uint32_t pointer, uint32_t nbytes);
uint32_t i2c1_scan(uint8_t *addreses, uint32_t cnt);

#endif /* __I2C_EX_H__ */
