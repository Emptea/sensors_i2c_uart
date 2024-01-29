#ifndef __SENSORS_H__
#define __SENSORS_H__

#include "lm75bd.h"
#include "tmp112.h"
#include "sht3x_dis.h"
#include "zs05.h"
#include "bmp180.h"
#include "tools.h"
#include "usart_ex.h"

#define SENSORS_CNT 5

// #define WET_SENSOR

enum sensor_type {
    SENSOR_TYPE_NONE,
    SENSOR_TYPE_LM75BD = 1,
    SENSOR_TYPE_TMP112,
    SENSOR_TYPE_SHT30,
    SENSOR_TYPE_ZS05,
    SENSOR_TYPE_BMP180,
    SENSOR_TYPE_LPS22HB,
    SENSOR_TYPE_DOORKNOT,
    SENSOR_TYPE_EXPANDER,
    SENSOR_TYPE_WETSENS
};

uint32_t sensors_init(void);
void sensors_measure(usart_packet p[], uint32_t sensor_type);

#endif /* __SENSORS_H__ */
