#include "app_modekey.h"
#include "app_alarm.h"
#include "app_display.h"
#include "app_settings.h"
#include "app_state.h"
#include "sd3077.h"
#include "tm1637.h"

/**
 * @brief MODE键单击事件处理
 * @note 实现状态机切换逻辑
 */
void mode_key_clicked(void)
{
    /* 响铃时任意键停止 */
    if (is_alarming)
    {
        alarm_stop();
        return;
    }

    /* 显示模式: 进入设置 */
    if (current_mode == MODE_SHOW_TIME || current_mode == MODE_SHOW_TEMPERTURE)
    {
        current_mode = MODE_SET_HOUR;
        blink_control = 0xFF;
        refresh_settings_display();
        set_interrupt_output(F_2_HZ); /* 设置RTC输出2Hz用于闪烁 */
    }
    else if (current_mode == MODE_SHOW_SECOND)
    {
        /* 显示秒时按MODE键: 清零秒数 */
        last_time.seconds = 0;
        set_time(&last_time);
        refresh_time_display();
    }
    /* 设置模式: 逐级切换设置项 */
    else if (current_mode == MODE_SET_HOUR)
    {
        current_mode = MODE_SET_MINUTE;
        blink_control = 0xFF;
        refresh_settings_display();
    }
    else if (current_mode == MODE_SET_MINUTE)
    {
        current_mode = MODE_SET_ALARM_ENABLE;
        blink_control = 0x00;
        refresh_settings_display();
    }
    else if (current_mode == MODE_SET_ALARM_ENABLE)
    {
        current_mode = is_alarm_enabled ? MODE_SET_ALARM_HOUR : MODE_SET_TEMP_SHOW;
        blink_control = 0xFF;
        refresh_settings_display();
    }
    else if (current_mode == MODE_SET_ALARM_HOUR)
    {
        current_mode = MODE_SET_ALARM_MINUTE;
        blink_control = 0xFF;
        refresh_settings_display();
    }
    else if (current_mode == MODE_SET_ALARM_MINUTE)
    {
        current_mode = MODE_SET_TEMP_SHOW;
        blink_control = 0x00;
        refresh_settings_display();
    }
    else if (current_mode == MODE_SET_TEMP_SHOW)
    {
        if (temperature_show_time != 0)
        {
            current_mode = MODE_SET_TEMP_HIDE;
        }
        else
        {
            current_mode = MODE_SET_BRIGHTNESS;
        }

        blink_control = 0x00;
        refresh_settings_display();
    }
    else if (current_mode == MODE_SET_TEMP_HIDE)
    {
        current_mode = MODE_SET_BRIGHTNESS;
        blink_control = 0x00;
        refresh_settings_display();
    }
    else if (current_mode == MODE_SET_BRIGHTNESS)
    {
        if (save_brightness == 0)
        {
            current_mode = MODE_SET_BRIGHTNESS_STRONG;
        }
        else
        {
            current_mode = MODE_SET_ROT_ENABLE;
        }

        blink_control = 0x00;
        refresh_settings_display();
    }
    else if (current_mode == MODE_SET_BRIGHTNESS_STRONG)
    {
        current_mode = MODE_SET_BRIGHTNESS_WEAK;
        blink_control = 0x00;
        tm1637_set_brightness(weak_brightness);
        refresh_settings_display();
    }
    else if (current_mode == MODE_SET_BRIGHTNESS_WEAK)
    {
        current_mode = MODE_SET_ROT_ENABLE;
        blink_control = 0x00;
        tm1637_set_brightness(strong_brightness);
        is_weak_brightness = false;
        refresh_settings_display();
    }
    else if (current_mode == MODE_SET_ROT_ENABLE)
    {
        blink_control = 0x00;

        if (!is_ring_on_time_enabled)//is_ring_on_time_enabled=false
        {
            current_mode = MODE_SHOW_TIME;
            time_now(&time);
            last_time.seconds = time.seconds;
            set_time(&last_time);
            last_ring_on_time_hour = last_time.hours;

            save_settings();

            refresh_time_display();
            enable_second_interrupt_output();
            last_display_change_time = HAL_GetTick();
        }
        else
        {
            current_mode = MODE_SET_ROT_START;
            refresh_settings_display();
        }
    }
    else if (current_mode == MODE_SET_ROT_START)
    {
        current_mode = MODE_SET_ROT_STOP;
        blink_control = 0x00;
        refresh_settings_display();
    }
    else if (current_mode == MODE_SET_ROT_STOP)
    {
        /* 最后一个设置项: 保存并退出设置模式 */
        current_mode = MODE_SHOW_TIME;

        /* 将修改后的时间写入RTC */
        time_now(&time);
        last_time.seconds = time.seconds;
        set_time(&last_time);
        last_ring_on_time_hour = last_time.hours;

        save_settings(); /* 保存所有设置到备份寄存器 */

        refresh_time_display();
        enable_second_interrupt_output(); /* 恢复1Hz秒中断 */
        last_display_change_time = HAL_GetTick();
    }
}

/**
 * @brief MODE键长按事件处理
 * @note 长按直接退出设置模式
 */
void mode_key_long_pressed(void)
{
    if (is_alarming)
    {
        alarm_stop();
        return;
    }

    if (current_mode >= MODE_SET_HOUR && current_mode <= MODE_SET_ROT_STOP)
    {
        current_mode = MODE_SHOW_TIME; /* 放弃修改,直接退出 */
        refresh_time_display();
        last_display_change_time = HAL_GetTick();
    }
}

/**
 * @brief MODE键按下事件
 * @note 记录按下时间
 */
void mode_key_pressed(void)
{
    last_mode_key_press_time = HAL_GetTick();
}

/**
 * @brief MODE键释放事件
 * @note 根据按下时长判断是单击还是长按
 */
void mode_key_released(void)
{
    uint32_t current_val;

    current_val = HAL_GetTick();

    if (last_mode_key_press_time > current_val)
    {
        mode_key_clicked();
    }
    else if (current_val - last_mode_key_press_time > KEY_LONG_PRESS_EFFECT_TIME)
    {
        mode_key_long_pressed();
    }
    else if (current_val - last_mode_key_press_time > KEY_CLICK_EFFECT_TIME)
    {
        mode_key_clicked();
    }
}
