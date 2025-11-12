#ifndef __EXTI_H
#define __EXTI_H
#include "stm32f0xx_hal.h"
void SysTick_Handler(void);
void EXTI0_1_IRQHandler(void);
void EXTI2_3_IRQHandler(void);
void DMA1_Channel1_IRQHandler(void);
void TIM16_IRQHandler(void);
void TIM17_IRQHandler(void);

typedef void (*exti_interrupt_callback_t)(uint16_t gpio_pin);
void register_exti_interrupt_callback(exti_interrupt_callback_t callback);

#endif
