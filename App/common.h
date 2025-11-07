#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f0xx_hal.h"

void Error_Handler(void);

#define TM1637_CLK_Pin GPIO_PIN_0
#define TM1637_CLK_GPIO_Port GPIOF
#define TM1637_DIO_Pin GPIO_PIN_1
#define TM1637_DIO_GPIO_Port GPIOF
#define LIGHT_Pin GPIO_PIN_0
#define LIGHT_GPIO_Port GPIOA
#define NTC_Pin GPIO_PIN_1
#define NTC_GPIO_Port GPIOA
#define MODE_KEY_Pin GPIO_PIN_2
#define MODE_KEY_GPIO_Port GPIOA
#define MODE_KEY_EXTI_IRQn EXTI2_3_IRQn
#define SET_KEY_Pin GPIO_PIN_3
#define SET_KEY_GPIO_Port GPIOA
#define SET_KEY_EXTI_IRQn EXTI2_3_IRQn
#define BUZZER_Pin GPIO_PIN_6
#define BUZZER_GPIO_Port GPIOA
#define SEC_INT_Pin GPIO_PIN_1
#define SEC_INT_GPIO_Port GPIOB
#define SEC_INT_EXTI_IRQn EXTI0_1_IRQn

#ifdef __cplusplus
}
#endif

#endif
