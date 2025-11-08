#ifndef __TM1637_H
#define __TM1637_H

#include "stm32f0xx_hal.h"

#define TM1637_CLK_PORT GPIOF   //PF0 - CLK
#define TM1637_CLK_PIN  GPIO_PIN_0

#define TM1637_DIO_PORT GPIOF   //PF1 - DIO
#define TM1637_DIO_PIN  GPIO_PIN_1


#define TM1637_DP_0  0x01
#define TM1637_DP_1  0x02
#define TM1637_DP_2  0x04
#define TM1637_DP_3  0x08
#define TM1637_DP_4  0x10
#define TM1637_DP_5  0x20

//IO操作

#define TM1637_CLK(x) \
    do { \
        HAL_GPIO_WritePin(TM1637_CLK_PORT, TM1637_CLK_PIN, (x) ? GPIO_PIN_SET : GPIO_PIN_RESET); \
    } while(0)

#define TM1637_DIO(x) \
    do { \
        HAL_GPIO_WritePin(TM1637_DIO_PORT, TM1637_DIO_PIN, (x) ? GPIO_PIN_SET : GPIO_PIN_RESET); \
    } while(0)

#define TM1637_CLK_DIO_GPIO_CLK_ENABLE() do{__HAL_RCC_GPIOF_CLK_ENABLE();}while(0)
#define TM1637_READ_DIO HAL_GPIO_ReadPin(TM1637_DIO_PORT, TM1637_DIO_PIN)

void tm1637_init(void);

void tm1637_show_number_right(uint8_t index, uint32_t num, uint8_t pointLocation, uint8_t isPaddingZero);

void tm1637_set_char(uint8_t index, char c, uint8_t point);

void tm1637_set_raw_data(uint8_t index, uint8_t data);

void tm1637_set_brightness(uint8_t brightness);

#endif
