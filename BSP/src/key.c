#include "key.h"
void key_init(void)
{
    MODE_KEY_CLK_ENABLE();
    SET_KEY_CLK_ENABLE();

    GPIO_InitTypeDef gpio_init_struct = {0};
    gpio_init_struct.Pin = MODE_KEY_PIN | SET_KEY_PIN;
    gpio_init_struct.Mode = GPIO_MODE_IT_RISING_FALLING;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &gpio_init_struct);

    HAL_NVIC_SetPriority(EXTI2_3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI2_3_IRQn);
}
