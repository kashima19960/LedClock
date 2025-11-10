#include "app_keyhandler.h"
#include "app_state.h"
#include "app_display.h"
#include "app_modekey.h"
#include "app_setkey.h"

/**
 * @brief 按键中断统一处理函数
 * @param gpio_pin 触发中断的GPIO引脚
 * @note 根据不同的引脚分发到对应的处理函数
 */
void key_interrupt_handler(uint16_t gpio_pin)
{
    /* 秒中断处理 */
    if (gpio_pin == SEC_INT_PIN && isInitCompleted)
    {
        if (currentMode == MODE_SHOW_TIME 
            || currentMode == MODE_SHOW_SECOND 
            || currentMode == MODE_SHOW_TEMPERTURE)
        {
            refreshTimeDisplay();
        }
        else if (currentMode >= MODE_SET_HOUR 
                 && currentMode <= MODE_SET_ROT_STOP)
        {
            refreshSettingsDisplay();
        }
        
        blinkControl = ~blinkControl;
    }
    /* MODE键中断处理 */
    else if (gpio_pin == MODE_KEY_PIN)
    {
        if (HAL_GPIO_ReadPin(MODE_KEY_GPIO_PORT, MODE_KEY_PIN) 
            == GPIO_PIN_RESET)
        {
            mode_key_pressed();
        }
        else
        {
            mode_key_released();
        }
    }
    /* SET键中断处理 */
    else if (gpio_pin == SET_KEY_PIN)
    {
        if (HAL_GPIO_ReadPin(SET_KEY_GPIO_PORT, SET_KEY_PIN) 
            == GPIO_PIN_RESET)
        {
            set_key_pressed();
        }
        else
        {
            set_key_released();
        }
    }
}
