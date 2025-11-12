#include "app_interrupt_handler.h"
#include "app_display.h"
#include "app_modekey.h"
#include "app_setkey.h"
#include "app_state.h"
#include "tm1637.h"
void exti_interrupt_handler(uint16_t gpio_pin)
{
    /* 秒中断处理 */
    if (gpio_pin == SEC_INT_PIN && isInitCompleted)
    {
        if (currentMode == MODE_SHOW_TIME || currentMode == MODE_SHOW_SECOND || currentMode == MODE_SHOW_TEMPERTURE)
        {
            refreshTimeDisplay();
        }
        else if (currentMode >= MODE_SET_HOUR && currentMode <= MODE_SET_ROT_STOP)
        {
            // 这个函数用来控制字符的闪烁的显示，不起到实际更改设置的作用
            refreshSettingsDisplay();
        }

        blinkControl = ~blinkControl;
    }
    /* MODE键中断处理 */
    else if (gpio_pin == MODE_KEY_PIN)
    {
        if (HAL_GPIO_ReadPin(MODE_KEY_GPIO_PORT, MODE_KEY_PIN) == GPIO_PIN_RESET)
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
        if (HAL_GPIO_ReadPin(SET_KEY_GPIO_PORT, SET_KEY_PIN) == GPIO_PIN_RESET)
        {
            set_key_pressed();
        }
        else
        {
            set_key_released();
        }
    }
}
void tim_interrupt_handler(TIM_HandleTypeDef *htim)
{
    // TIM17: 闹钟响铃节奏控制
    if (htim->Instance == ALARM_CONTROL_TIMER)
    {
        alarmTimerTick(); // 控制蜂鸣器响铃节奏
    }
    // TIM16: 自动亮度调节(根据光敏电阻ADC值)
    else if (htim->Instance == LIGHT_CONTROL_TIMER)
    {
        if (savedBrightness == 0) // 自动亮度模式
        {
            /* 环境光线变强 -> 提高亮度 */
            if (isWeakBrightness && adcValue[0] > STRONG_BRIGHTNESS_ADC_VALUE)
            {
                isWeakBrightness = false;
                
                if (strongBrightness > 0)
                {
                    tm1637_set_brightness(strongBrightness);
                }
            }
            /* 环境光线变弱 -> 降低亮度 */
            else if (!isWeakBrightness && adcValue[0] < WEAK_BRIGHTNESS_ADC_VALUE)
            {
                isWeakBrightness = true;
                
                if (weakBrightness > 0)
                {
                    tm1637_set_brightness(weakBrightness);
                }
            }
        }
    }
}
void dma_interrupt_handler(void)
{
}