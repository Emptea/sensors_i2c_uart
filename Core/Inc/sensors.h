#include "lm75bd.h"
#include "tmp112.h"
#include "sht3x_dis.h"
#include "zs05.h"
#include "bmp180.h"
#include "tools.h"
#include "gpio.h"

//#define LM75BD
//#define ZS05
#define BMP180
//#define WET_SENSOR

void sensors_init(void);
void sensors_measure(usart_packet p[]);
