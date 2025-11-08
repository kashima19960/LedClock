#include "key.h"

void key_init(void)
{
    GPIO_InitTypeDef gpio_init_struct = {0};
    gpio_init_struct.Pin = MODE_KEY_PIN | SET_KEY_PIN;
    gpio_init_struct.Mode = GPIO_MODE_IT_RISING_FALLING;
    gpio_init_struct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOA, &gpio_init_struct);
}