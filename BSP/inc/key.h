#ifndef __KEY_H__
#define __KEY_H__

#include "stm32f0xx_hal.h"
#include "app_config.h"

#define MODE_KEY_CLK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()
#define SET_KEY_CLK_ENABLE()  __HAL_RCC_GPIOA_CLK_ENABLE()

/**
 * @brief 按键GPIO初始化
 */
void key_init(void);

#endif
