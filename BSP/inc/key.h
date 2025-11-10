#ifndef __KEY_H__
#define __KEY_H__


#include "stm32f0xx_hal.h"
#include "common.h"

#define MODE_KEY_CLK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()
#define SET_KEY_CLK_ENABLE()  __HAL_RCC_GPIOA_CLK_ENABLE()
#define KEY_EXTI_CALLBACK HAL_GPIO_EXTI_Callback
#endif
