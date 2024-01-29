#include "i2c.h"

#define BMP180_ADDR                  0x77

#define BMP180_CONTROL_REG_ADDR      0xF4
#define BMP180_CMD_MEAS_TEMP         0x2E
#define BMP180_CMD_MEAS_PRESS        0x34

#define BMP180_EEPROM_REG_START_ADDR 0xAA
#define BMP180_EEPROM_REG_END_ADDR   0xBF

#define BMP180_INVALID_DATA          0U
#define BMP180_SUCCESS               1U

struct p_bmp180 {
    float temp;
    float press;
    int32_t ut;
    int32_t up;

    union {
        struct
        {
            int16_t ac1;
            int16_t ac2;
            int16_t ac3;
            uint16_t ac4;
            uint16_t ac5;
            uint16_t ac6;
            int16_t b1;
            int16_t b2;
            int16_t mb;
            int16_t mc;
            int16_t md;
        } params;

        union {
            uint16_t byte_data[11];
            uint8_t raw_data[2 * 11];
        } data;
    } calibr;
};

void bmp180_get_cal_param(struct p_bmp180 *p_bmp180);
void bmp180_get_uncomp_temp(struct p_bmp180 *p_bmp180);
/*
\params oss - oversampling 0...3
*/
uint32_t bmp180_get_uncomp_press(uint32_t oss, struct p_bmp180 *p_bmp180);

uint32_t bmp180_get_temp(struct p_bmp180 *p_bmp180);
uint32_t bmp180_get_press(struct p_bmp180 *p_bmp180, uint32_t oss);
