#ifndef __GPIO_EX_H__
#define __GPIO_EX_H__
#include "gpio.h"

inline static void blink_red()
{
    LL_GPIO_SetOutputPin(GPIO_LED, PIN_RED_LED);
    LL_mDelay(500);
    LL_GPIO_ResetOutputPin(GPIO_LED, PIN_RED_LED);
    LL_mDelay(500);
}

inline static void turn_red_on()
{
    LL_GPIO_SetOutputPin(GPIO_LED, PIN_RED_LED);
}

inline static void turn_red_off()
{
    LL_GPIO_ResetOutputPin(GPIO_LED, PIN_RED_LED);
}

inline static void turn_green_on()
{
    LL_GPIO_SetOutputPin(GPIO_LED, PIN_GREEN_LED);
}

inline static void turn_green_off()
{
    LL_GPIO_ResetOutputPin(GPIO_LED, PIN_GREEN_LED);
}

inline static uint32_t is_green_on()
{
    return LL_GPIO_IsOutputPinSet(GPIO_LED, PIN_GREEN_LED);
}
#endif /*__GPIO_EX_H__*/