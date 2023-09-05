/*CRC8 0x31 calculation*/
#include <stdio.h>
#include <stdint.h>

void main (void)
{
    uint32_t data[] = {0xBE, 0xEF};
    unsigned char crc = 0xFF;
    for (uint32_t k = 0; k < 2; k++)
    {
        crc ^= data[k];

        for (unsigned int i = 0; i < 8; i++)
            crc = crc & 0x80 ? (crc << 1) ^ 0x31 : crc << 1;
    }
    printf("%d", crc);
}