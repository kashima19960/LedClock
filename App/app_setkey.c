#include "app_setkey.h"
#include "app_state.h"
#include "app_display.h"
#include "app_alarm.h"
#include "tm1637.h"

/**
 * @brief SET键单击事件处理
 * @note 增加当前设置项的值
 */
void set_key_clicked(void)
{
    if (is_alarming)
    {
        alarm_stop();
        return;
    }

    /* 显示模式: 切换到显示秒 */
    if (current_mode == MODE_SHOW_TIME || current_mode == MODE_SHOW_TEMPERTURE)
    {
        current_mode = MODE_SHOW_SECOND;
        refresh_time_display();
    }
    else if (current_mode == MODE_SHOW_SECOND)
    {
        current_mode = MODE_SHOW_TIME; /* 返回显示时间 */
        refresh_time_display();
        last_display_change_time = HAL_GetTick();
    }
    /* 设置模式: 增加当前设置项的值 */
    else if (current_mode == MODE_SET_HOUR)
    {
        last_time.hours++;

        if (last_time.hours > 23)
        {
            last_time.hours = 0;
        }

        blink_control = 0;
        refresh_settings_display();
    }
    else if (current_mode == MODE_SET_MINUTE)
    {
        last_time.minutes++;

        if (last_time.minutes > 59)
        {
            last_time.minutes = 0;
        }

        blink_control = 0;
        refresh_settings_display();
    }
    else if (current_mode == MODE_SET_ALARM_ENABLE)
    {
        is_alarm_enabled = is_alarm_enabled ? false : true;
        blink_control = 0;
        refresh_settings_display();
    }
    else if (current_mode == MODE_SET_ALARM_HOUR)
    {
        alarm_hour++;

        if (alarm_hour > 23)
        {
            alarm_hour = 0;
        }

        blink_control = 0;
        refresh_settings_display();
    }
    else if (current_mode == MODE_SET_ALARM_MINUTE)
    {
        alarm_min++;

        if (alarm_min > 59)
        {
            alarm_min = 0;
        }

        blink_control = 0;
        refresh_settings_display();
    }
    else if (current_mode == MODE_SET_TEMP_SHOW)
    {
        temperature_show_time++;

        if (temperature_show_time > TEMPERTURE_MAX_SHOW_TIME)
        {
            temperature_show_time = temperature_hide_time == 0 ? 1 : 0;
        }

        blink_control = 0;
        refresh_settings_display();
    }
    else if (current_mode == MODE_SET_TEMP_HIDE)
    {
        temperature_hide_time++;

        if (temperature_hide_time > TEMPERTURE_MAX_HIDE_TIME)
        {
            temperature_hide_time = temperature_show_time == 0 ? 1 : 0;
        }

        blink_control = 0;
        refresh_settings_display();
    }
    else if (current_mode == MODE_SET_BRIGHTNESS)
    {
        save_brightness++;

        if (save_brightness > 8)
        {
            save_brightness = 0;
        }

        tm1637_set_brightness(save_brightness == 0 ? 1 : save_brightness);
        blink_control = 0;
        refresh_settings_display();
    }
    else if (current_mode == MODE_SET_BRIGHTNESS_STRONG)
    {
        strong_brightness++;

        if (strong_brightness > 8)
        {
            strong_brightness = 1;
        }

        blink_control = 0;
        tm1637_set_brightness(strong_brightness);
        refresh_settings_display();
    }
    else if (current_mode == MODE_SET_BRIGHTNESS_WEAK)
    {
        weak_brightness++;

        if (weak_brightness > 8)
        {
            weak_brightness = 1;
        }

        blink_control = 0;
        tm1637_set_brightness(weak_brightness);
        refresh_settings_display();
    }
    else if (current_mode == MODE_SET_ROT_ENABLE)
    {
        is_ring_on_time_enabled = is_ring_on_time_enabled ? false : true;
        blink_control = 0;
        refresh_settings_display();
    }
    else if (current_mode == MODE_SET_ROT_START)
    {
        ring_on_time_start++;

        if (ring_on_time_start > 23)
        {
            ring_on_time_start = 0;
        }

        blink_control = 0;
        refresh_settings_display();
    }
    else if (current_mode == MODE_SET_ROT_STOP)
    {
        ring_on_time_stop++;

        if (ring_on_time_stop > 23)
        {
            ring_on_time_stop = 0;
        }

        blink_control = 0;
        refresh_settings_display();
    }
}

/**
 * @brief SET键连按快速调整事件处理
 * @note 只对需要快速调整的模式生效
 */
void set_key_presse_repeat_report(void)
{
    set_key_repeat_reported = true;

    /* 只对需要快速调整的模式生效 */
    if (current_mode == MODE_SET_HOUR)
    {
        last_time.hours++;

        if (last_time.hours > 23)
        {
            last_time.hours = 0;
        }

        blink_control = 0;
        refresh_settings_display();
    }
    else if (current_mode == MODE_SET_MINUTE)
    {
        last_time.minutes++;

        if (last_time.minutes > 59)
        {
            last_time.minutes = 0;
        }

        blink_control = 0;
        refresh_settings_display();
    }
    else if (current_mode == MODE_SET_ALARM_HOUR)
    {
        alarm_hour++;

        if (alarm_hour > 23)
        {
            alarm_hour = 0;
        }

        blink_control = 0;
        refresh_settings_display();
    }
    else if (current_mode == MODE_SET_ALARM_MINUTE)
    {
        alarm_min++;

        if (alarm_min > 59)
        {
            alarm_min = 0;
        }

        blink_control = 0;
        refresh_settings_display();
    }
    else if (current_mode == MODE_SET_TEMP_SHOW)
    {
        temperature_show_time++;

        if (temperature_show_time > TEMPERTURE_MAX_SHOW_TIME)
        {
            temperature_show_time = temperature_hide_time == 0 ? 1 : 0;
        }

        blink_control = 0;
        refresh_settings_display();
    }
    else if (current_mode == MODE_SET_TEMP_HIDE)
    {
        temperature_hide_time++;

        if (temperature_hide_time > TEMPERTURE_MAX_HIDE_TIME)
        {
            temperature_hide_time = temperature_show_time == 0 ? 1 : 0;
        }

        blink_control = 0;
        refresh_settings_display();
    }
    else if (current_mode == MODE_SET_ROT_START)
    {
        ring_on_time_start++;

        if (ring_on_time_start > 23)
        {
            ring_on_time_start = 0;
        }

        blink_control = 0;
        refresh_settings_display();
    }
    else if (current_mode == MODE_SET_ROT_STOP)
    {
        ring_on_time_stop++;

        if (ring_on_time_stop > 23)
        {
            ring_on_time_stop = 0;
        }

        blink_control = 0;
        refresh_settings_display();
    }
}

/**
 * @brief SET键按下事件
 * @note 记录按下时间
 */
void set_key_pressed(void)
{
    last_set_key_press_time = HAL_GetTick();
}

/**
 * @brief SET键释放事件
 */
void set_key_released(void)
{
    uint32_t current_val;

    current_val = HAL_GetTick();

    if (last_set_key_press_time > current_val)
    {
        set_key_clicked();
    }
    else if (current_val - last_set_key_press_time > KEY_CLICK_EFFECT_TIME && !set_key_repeat_reported)
    {
        set_key_clicked();
    }

    set_key_repeat_reported = false;
}

