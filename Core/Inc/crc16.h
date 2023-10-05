#include "main.h"
/*
* Function to compute CRC-16-IBM
* CRC-16-ANSI (aka CRC-16-IBM) Polynomial: x^16 + x^15 + x^2 + 1
*
* @param crc - init value of crc
* @param buf - data to compute crc for
* @param size - size of buf
*/
uint16_t crc16(uint16_t crc, const void *buf, uint32_t size);
	