#ifndef __BSP_CONFIG_H
#define __BSP_CONFIG_H

#include "stm32f0xx_hal.h"
#define TM1637_CLK_PIN                   GPIO_PIN_1           // 时钟引脚
#define TM1637_CLK_GPIO_PORT             GPIOF                // 时钟GPIO口
#define TM1637_DIO_PIN                   GPIO_PIN_0           // 数据引脚
#define TM1637_DIO_GPIO_PORT             GPIOF                // 数据GPIO口
#define LIGHT_PIN                        GPIO_PIN_0           // 光敏电阻 ADC 输入引脚
#define LIGHT_GPIO_PORT                  GPIOA                // 光敏电阻 ADC 输入GPIO口
#define NTC_PIN                          GPIO_PIN_1           // NTC 热敏电阻 ADC 输入引脚
#define NTC_GPIO_PORT                    GPIOA                // NTC 热敏电阻 ADC 输入GPIO口
#define MODE_KEY_PIN                     GPIO_PIN_3           // 模式切换按键引脚
#define MODE_KEY_GPIO_PORT               GPIOA                // 模式切换按键GPIO口
#define MODE_KEY_EXTI_IRQn               EXTI2_3_IRQn         // 模式按键外部中断线
#define SET_KEY_PIN                      GPIO_PIN_2           // 设置调整按键引脚
#define SET_KEY_GPIO_PORT                GPIOA                // 设置调整按键GPIO口
#define SET_KEY_EXTI_IRQn                EXTI2_3_IRQn         // 设置按键外部中断线
#define BUZZER_PIN                       GPIO_PIN_6           // 蜂鸣器控制引脚
#define BUZZER_GPIO_PORT                 GPIOA                // 蜂鸣器控制GPIO口
#define SEC_INT_PIN                      GPIO_PIN_1           // SD3077 秒脉冲中断引脚
#define SEC_INT_GPIO_PORT                GPIOB                // SD3077 秒脉冲中断GPIO口
#define SEC_INT_EXTI_IRQn                EXTI0_1_IRQn         // 秒中断外部中断线
#define TEMP_BUFFER_SIZE                 8                    // 温度 ADC 采样缓冲区大小
#define TEMP_MAP_SIZE                    126                  // 温度映射表大小
void Error_Handler(void);

#endif
