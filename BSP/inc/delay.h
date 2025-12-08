#ifndef __DELAY_H
#define __DELAY_H

#include "stm32f0xx_hal.h"

void delay_init(uint16_t sysclk);
void delay_ms(uint32_t nms);
void delay_us(uint32_t nus);

#endif
