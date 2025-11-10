#ifndef __STM32F0xx_IT_H
#define __STM32F0xx_IT_H

#include "stm32f0xx_hal.h"

#ifdef __cplusplus
 extern "C" {
#endif

void NMI_Handler(void);
void HardFault_Handler(void);
void SVC_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);
void EXTI0_1_IRQHandler(void);
void EXTI2_3_IRQHandler(void);
void DMA1_Channel1_IRQHandler(void);
void TIM16_IRQHandler(void);
void TIM17_IRQHandler(void);

/**
 * @brief 按键中断回调函数类型
 * @param gpio_pin 触发中断的GPIO引脚
 */
typedef void (*key_interrupt_callback_t)(uint16_t gpio_pin);

/**
 * @brief 注册按键中断回调函数
 * @param callback 回调函数指针
 */
void register_key_interrupt_callback(key_interrupt_callback_t callback);

#ifdef __cplusplus
}
#endif

#endif /* __STM32F0xx_IT_H */
