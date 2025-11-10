#include "key.h"
uint8_t blinkControl;
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

void KEY_EXTI_CALLBACK(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == SEC_INT_PIN && isInitCompleted)
    {
        if (currentMode == MODE_SHOW_TIME || currentMode == MODE_SHOW_SECOND || currentMode == MODE_SHOW_TEMPERTURE)
        {
            refreshTimeDisplay();
        }
        else if (currentMode >= MODE_SET_HOUR && currentMode <= MODE_SET_ROT_STOP)
        {
            refreshSettingsDisplay();
        }
        blinkControl = ~blinkControl;
    }
    else if (GPIO_Pin == MODE_KEY_PIN)
    {
        if (HAL_GPIO_ReadPin(MODE_KEY_GPIO_PORT, MODE_KEY_PIN) == GPIO_PIN_RESET)
        {
            modeKeyPressed();
        }
        else
        {
            modeKeyReleased();
        }
    }
    else if (GPIO_Pin == SET_KEY_PIN)
    {
        if (HAL_GPIO_ReadPin(SET_KEY_GPIO_PORT, SET_KEY_PIN) == GPIO_PIN_RESET)
        {
            setKeyPressed();
        }
        else
        {
            setKeyReleased();
        }
    }
}
