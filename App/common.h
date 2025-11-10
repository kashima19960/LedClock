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
typedef enum
{
    MODE_SHOW_TIME = 0,
    MODE_SHOW_SECOND,
    MODE_SHOW_TEMPERTURE,
    MODE_SET_HOUR,
    MODE_SET_MINUTE,
    MODE_SET_ALARM_ENABLE,
    MODE_SET_ALARM_HOUR,
    MODE_SET_ALARM_MINUTE,
    MODE_SET_TEMP_SHOW,
    MODE_SET_TEMP_HIDE,
    MODE_SET_BRIGHTNESS,
    MODE_SET_BRIGHTNESS_STRONG,
    MODE_SET_BRIGHTNESS_WEAK,
    MODE_SET_ROT_ENABLE,
    MODE_SET_ROT_START,
    MODE_SET_ROT_STOP,
} DisplayMode;
#ifdef __cplusplus
}
#endif

#endif
