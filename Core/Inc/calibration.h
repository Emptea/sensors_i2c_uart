#ifndef __CALIBRATION_H
#define __CALIBRATION_H
#include "main.h"
#include "usart_ex.h"

void calibration_init(void);
uint32_t calibration_get_count(void);
uint32_t calibration_save(struct offset *offset);
uint32_t calibration_clear(void);

#endif /* __CALIBRATION_H */
