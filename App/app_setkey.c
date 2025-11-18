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
    if (isAlarming)
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
        lastDisplayChangeTime = HAL_GetTick();
    }
    /* 设置模式: 增加当前设置项的值 */
    else if (current_mode == MODE_SET_HOUR)
    {
        lastTime.hours++;

        if (lastTime.hours > 23)
        {
            lastTime.hours = 0;
        }

        blink_control = 0;
        refresh_settings_display();
    }
    else if (current_mode == MODE_SET_MINUTE)
    {
        lastTime.minutes++;

        if (lastTime.minutes > 59)
        {
            lastTime.minutes = 0;
        }

        blink_control = 0;
        refresh_settings_display();
    }
    else if (current_mode == MODE_SET_ALARM_ENABLE)
    {
        isAlarmEnabled = isAlarmEnabled ? false : true;
        blink_control = 0;
        refresh_settings_display();
    }
    else if (current_mode == MODE_SET_ALARM_HOUR)
    {
        alarmHour++;

        if (alarmHour > 23)
        {
            alarmHour = 0;
        }

        blink_control = 0;
        refresh_settings_display();
    }
    else if (current_mode == MODE_SET_ALARM_MINUTE)
    {
        alarmMin++;

        if (alarmMin > 59)
        {
            alarmMin = 0;
        }

        blink_control = 0;
        refresh_settings_display();
    }
    else if (current_mode == MODE_SET_TEMP_SHOW)
    {
        tempertureShowTime++;

        if (tempertureShowTime > TEMPERTURE_MAX_SHOW_TIME)
        {
            tempertureShowTime = tempertureHideTime == 0 ? 1 : 0;
        }

        blink_control = 0;
        refresh_settings_display();
    }
    else if (current_mode == MODE_SET_TEMP_HIDE)
    {
        tempertureHideTime++;

        if (tempertureHideTime > TEMPERTURE_MAX_HIDE_TIME)
        {
            tempertureHideTime = tempertureShowTime == 0 ? 1 : 0;
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
        isRingOnTimeEnabled = isRingOnTimeEnabled ? false : true;
        blink_control = 0;
        refresh_settings_display();
    }
    else if (current_mode == MODE_SET_ROT_START)
    {
        ringOnTimeStart++;

        if (ringOnTimeStart > 23)
        {
            ringOnTimeStart = 0;
        }

        blink_control = 0;
        refresh_settings_display();
    }
    else if (current_mode == MODE_SET_ROT_STOP)
    {
        ringOnTimeStop++;

        if (ringOnTimeStop > 23)
        {
            ringOnTimeStop = 0;
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
    setKeyRepeatReported = true;

    /* 只对需要快速调整的模式生效 */
    if (current_mode == MODE_SET_HOUR)
    {
        lastTime.hours++;

        if (lastTime.hours > 23)
        {
            lastTime.hours = 0;
        }

        blink_control = 0;
        refresh_settings_display();
    }
    else if (current_mode == MODE_SET_MINUTE)
    {
        lastTime.minutes++;

        if (lastTime.minutes > 59)
        {
            lastTime.minutes = 0;
        }

        blink_control = 0;
        refresh_settings_display();
    }
    else if (current_mode == MODE_SET_ALARM_HOUR)
    {
        alarmHour++;

        if (alarmHour > 23)
        {
            alarmHour = 0;
        }

        blink_control = 0;
        refresh_settings_display();
    }
    else if (current_mode == MODE_SET_ALARM_MINUTE)
    {
        alarmMin++;

        if (alarmMin > 59)
        {
            alarmMin = 0;
        }

        blink_control = 0;
        refresh_settings_display();
    }
    else if (current_mode == MODE_SET_TEMP_SHOW)
    {
        tempertureShowTime++;

        if (tempertureShowTime > TEMPERTURE_MAX_SHOW_TIME)
        {
            tempertureShowTime = tempertureHideTime == 0 ? 1 : 0;
        }

        blink_control = 0;
        refresh_settings_display();
    }
    else if (current_mode == MODE_SET_TEMP_HIDE)
    {
        tempertureHideTime++;

        if (tempertureHideTime > TEMPERTURE_MAX_HIDE_TIME)
        {
            tempertureHideTime = tempertureShowTime == 0 ? 1 : 0;
        }

        blink_control = 0;
        refresh_settings_display();
    }
    else if (current_mode == MODE_SET_ROT_START)
    {
        ringOnTimeStart++;

        if (ringOnTimeStart > 23)
        {
            ringOnTimeStart = 0;
        }

        blink_control = 0;
        refresh_settings_display();
    }
    else if (current_mode == MODE_SET_ROT_STOP)
    {
        ringOnTimeStop++;

        if (ringOnTimeStop > 23)
        {
            ringOnTimeStop = 0;
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
    lastSetKeyPressTime = HAL_GetTick();
}

/**
 * @brief SET键释放事件
 */
void set_key_released(void)
{
    uint32_t current_val;

    current_val = HAL_GetTick();

    if (lastSetKeyPressTime > current_val)
    {
        set_key_clicked();
    }
    else if (current_val - lastSetKeyPressTime > KEY_CLICK_EFFECT_TIME && !setKeyRepeatReported)
    {
        set_key_clicked();
    }

    setKeyRepeatReported = false;
}

