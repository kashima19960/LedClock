#ifndef __APP_KEYHANDLER_H
#define __APP_KEYHANDLER_H

#include "app_config.h"

/**
 * @brief 按键中断统一处理函数
 * @param gpio_pin 触发中断的GPIO引脚
 * @note 分发到具体的按键处理函数
 */
void key_interrupt_handler(uint16_t gpio_pin);

#endif
