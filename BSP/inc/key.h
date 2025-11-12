#ifndef __KEY_H
#define __KEY_H

#include "stm32f0xx_hal.h"
#include "app_config.h"

#define MODE_KEY_CLK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()
#define SET_KEY_CLK_ENABLE()  __HAL_RCC_GPIOA_CLK_ENABLE()

void key_init(void);

#endif
