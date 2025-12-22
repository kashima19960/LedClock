#ifndef __EXTI_H
#define __EXTI_H
#include "stm32f0xx_hal.h"

typedef void (*exti_interrupt_callback_t)(uint16_t gpio_pin);
void register_exti_interrupt_callback(exti_interrupt_callback_t callback);

#endif
