#ifndef __APP_INTERRUPT_HANDLER_H
#define __APP_INTERRUPT_HANDLER_H

#include "app_config.h"

/**
 * @brief 应用层中断统一处理函数
 * @param gpio_pin 触发中断的GPIO引脚
 * @note 处理SEC_INT秒中断、MODE键中断、SET键中断
 */
void exti_interrupt_handler(uint16_t gpio_pin);
void tim_interrupt_handler(TIM_HandleTypeDef *htim);
#endif
