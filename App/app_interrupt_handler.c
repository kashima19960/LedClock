#include "app_interrupt_handler.h"
#include "app_display.h"
#include "app_modekey.h"
#include "app_setkey.h"
#include "app_state.h"
#include "tm1637.h"
#include "app_alarm.h"
void exti_interrupt_handler(uint16_t gpio_pin)
{
    /* 秒中断处理 */
    if (gpio_pin == SEC_INT_PIN && is_init_completed)
    {
        if (current_mode == MODE_SHOW_TIME || current_mode == MODE_SHOW_SECOND || current_mode == MODE_SHOW_TEMPERTURE)
        {
            refresh_time_display();
        }
        else if (current_mode >= MODE_SET_HOUR && current_mode <= MODE_SET_ROT_STOP)
        {
            // 这个函数用来控制字符的闪烁的显示，不起到实际更改设置的作用
            refresh_settings_display();
        }

        blink_control = ~blink_control;
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

    if (htim->Instance == ALARM_CONTROL_TIMER)     // TIM17: 闹钟响铃节奏控制
    {
        alarm_timer_tick(); // 控制蜂鸣器响铃节奏
    }

    else if (htim->Instance == LIGHT_CONTROL_TIMER) // TIM16: 自动亮度调节(根据光敏电阻ADC值)
    {
        if (save_brightness == 0) // 自动亮度模式
        {
            /* 环境光线变强 -> 提高亮度 */
            if (is_weak_brightness && adc_value[0] > STRONG_BRIGHTNESS_ADC_VALUE)
            {
                is_weak_brightness = false;
                
                if (strong_brightness > 0)
                {
                    tm1637_set_brightness(strong_brightness);
                }
            }
            /* 环境光线变弱 -> 降低亮度 */
            else if (!is_weak_brightness && adc_value[0] < WEAK_BRIGHTNESS_ADC_VALUE)
            {
                is_weak_brightness = true;
                
                if (weak_brightness > 0)
                {
                    tm1637_set_brightness(weak_brightness);
                }
            }
        }
    }
}
void dma_interrupt_handler(void)
{
}
