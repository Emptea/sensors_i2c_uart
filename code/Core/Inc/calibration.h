#ifndef __CALIBRATION_H
#define __CALIBRATION_H
#include "main.h"
#include "usart_ex.h"

union offset calibration_init(void);
uint32_t calibration_get_count(void);
void calibration_save(union offset *offset);
uint32_t calibration_clear(void);

#endif /* __CALIBRATION_H */
