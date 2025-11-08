#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f0xx_hal.h"

void Error_Handler(void);

#define TM1637_CLK_PIN GPIO_PIN_0
#define TM1637_CLK_GPIO_PORT GPIOF
#define TM1637_DIO_PIN GPIO_PIN_1
#define TM1637_DIO_GPIO_PORT GPIOF

#define LIGHT_PIN GPIO_PIN_0
#define LIGHT_GPIO_PORT GPIOA

#define NTC_PIN GPIO_PIN_1
#define NTC_GPIO_PORT GPIOA

#define MODE_KEY_PIN GPIO_PIN_2
#define MODE_KEY_GPIO_PORT GPIOA
#define MODE_KEY_EXTI_IRQn EXTI2_3_IRQn

#define SET_KEY_PIN GPIO_PIN_3
#define SET_KEY_GPIO_PORT GPIOA
#define SET_KEY_EXTI_IRQn EXTI2_3_IRQn

#define BUZZER_PIN GPIO_PIN_6
#define BUZZER_GPIO_PORT GPIOA

#define SEC_INT_PIN GPIO_PIN_1
#define SEC_INT_GPIO_PORT GPIOB
#define SEC_INT_EXTI_IRQn EXTI0_1_IRQn

#ifdef __cplusplus
}
#endif

#endif
