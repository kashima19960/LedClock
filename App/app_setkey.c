#include "app_setkey.h"
#include "app_alarm.h"
#include "app_display.h"
#include "app_state.h"
#include "tm1637.h"

/**
 * @brief SET键单击事件处理
 * @note 增加当前设置项的值
 */
void set_key_clicked(void)
{
    if (isAlarming)
    {
        alarmStop();
        return;
    }

    /* 显示模式: 切换到显示秒 */
    if (currentMode == MODE_SHOW_TIME || currentMode == MODE_SHOW_TEMPERTURE)
    {
        currentMode = MODE_SHOW_SECOND;
        refreshTimeDisplay();
    }
    else if (currentMode == MODE_SHOW_SECOND)
    {
        currentMode = MODE_SHOW_TIME; /* 返回显示时间 */
        refreshTimeDisplay();
        lastDisplayChangeTime = HAL_GetTick();
    }
    /* 设置模式: 增加当前设置项的值 */
    else if (currentMode == MODE_SET_HOUR)
    {
        lastTime.hours++;

        if (lastTime.hours > 23)
        {
            lastTime.hours = 0;
        }

        blinkControl = 0;
        refreshSettingsDisplay();
    }
    else if (currentMode == MODE_SET_MINUTE)
    {
        lastTime.minutes++;

        if (lastTime.minutes > 59)
        {
            lastTime.minutes = 0;
        }

        blinkControl = 0;
        refreshSettingsDisplay();
    }
    else if (currentMode == MODE_SET_ALARM_ENABLE)
    {
        isAlarmEnabled = isAlarmEnabled ? false : true;
        blinkControl = 0;
        refreshSettingsDisplay();
    }
    else if (currentMode == MODE_SET_ALARM_HOUR)
    {
        alarmHour++;

        if (alarmHour > 23)
        {
            alarmHour = 0;
        }

        blinkControl = 0;
        refreshSettingsDisplay();
    }
    else if (currentMode == MODE_SET_ALARM_MINUTE)
    {
        alarmMin++;

        if (alarmMin > 59)
        {
            alarmMin = 0;
        }

        blinkControl = 0;
        refreshSettingsDisplay();
    }
    else if (currentMode == MODE_SET_TEMP_SHOW)
    {
        tempertureShowTime++;

        if (tempertureShowTime > TEMPERTURE_MAX_SHOW_TIME)
        {
            tempertureShowTime = tempertureHideTime == 0 ? 1 : 0;
        }

        blinkControl = 0;
        refreshSettingsDisplay();
    }
    else if (currentMode == MODE_SET_TEMP_HIDE)
    {
        tempertureHideTime++;

        if (tempertureHideTime > TEMPERTURE_MAX_HIDE_TIME)
        {
            tempertureHideTime = tempertureShowTime == 0 ? 1 : 0;
        }

        blinkControl = 0;
        refreshSettingsDisplay();
    }
    else if (currentMode == MODE_SET_BRIGHTNESS)
    {
        savedBrightness++;

        if (savedBrightness > 8)
        {
            savedBrightness = 0;
        }

        TM1637SetBrightness(savedBrightness == 0 ? 1 : savedBrightness);
        blinkControl = 0;
        refreshSettingsDisplay();
    }
    else if (currentMode == MODE_SET_BRIGHTNESS_STRONG)
    {
        strongBrightness++;

        if (strongBrightness > 8)
        {
            strongBrightness = 1;
        }

        blinkControl = 0;
        TM1637SetBrightness(strongBrightness);
        refreshSettingsDisplay();
    }
    else if (currentMode == MODE_SET_BRIGHTNESS_WEAK)
    {
        weakBrightness++;

        if (weakBrightness > 8)
        {
            weakBrightness = 1;
        }

        blinkControl = 0;
        TM1637SetBrightness(weakBrightness);
        refreshSettingsDisplay();
    }
    else if (currentMode == MODE_SET_ROT_ENABLE)
    {
        isRingOnTimeEnabled = isRingOnTimeEnabled ? false : true;
        blinkControl = 0;
        refreshSettingsDisplay();
    }
    else if (currentMode == MODE_SET_ROT_START)
    {
        ringOnTimeStart++;

        if (ringOnTimeStart > 23)
        {
            ringOnTimeStart = 0;
        }

        blinkControl = 0;
        refreshSettingsDisplay();
    }
    else if (currentMode == MODE_SET_ROT_STOP)
    {
        ringOnTimeStop++;

        if (ringOnTimeStop > 23)
        {
            ringOnTimeStop = 0;
        }

        blinkControl = 0;
        refreshSettingsDisplay();
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
    if (currentMode == MODE_SET_HOUR)
    {
        lastTime.hours++;

        if (lastTime.hours > 23)
        {
            lastTime.hours = 0;
        }

        blinkControl = 0;
        refreshSettingsDisplay();
    }
    else if (currentMode == MODE_SET_MINUTE)
    {
        lastTime.minutes++;

        if (lastTime.minutes > 59)
        {
            lastTime.minutes = 0;
        }

        blinkControl = 0;
        refreshSettingsDisplay();
    }
    else if (currentMode == MODE_SET_ALARM_HOUR)
    {
        alarmHour++;

        if (alarmHour > 23)
        {
            alarmHour = 0;
        }

        blinkControl = 0;
        refreshSettingsDisplay();
    }
    else if (currentMode == MODE_SET_ALARM_MINUTE)
    {
        alarmMin++;

        if (alarmMin > 59)
        {
            alarmMin = 0;
        }

        blinkControl = 0;
        refreshSettingsDisplay();
    }
    else if (currentMode == MODE_SET_TEMP_SHOW)
    {
        tempertureShowTime++;

        if (tempertureShowTime > TEMPERTURE_MAX_SHOW_TIME)
        {
            tempertureShowTime = tempertureHideTime == 0 ? 1 : 0;
        }

        blinkControl = 0;
        refreshSettingsDisplay();
    }
    else if (currentMode == MODE_SET_TEMP_HIDE)
    {
        tempertureHideTime++;

        if (tempertureHideTime > TEMPERTURE_MAX_HIDE_TIME)
        {
            tempertureHideTime = tempertureShowTime == 0 ? 1 : 0;
        }

        blinkControl = 0;
        refreshSettingsDisplay();
    }
    else if (currentMode == MODE_SET_ROT_START)
    {
        ringOnTimeStart++;

        if (ringOnTimeStart > 23)
        {
            ringOnTimeStart = 0;
        }

        blinkControl = 0;
        refreshSettingsDisplay();
    }
    else if (currentMode == MODE_SET_ROT_STOP)
    {
        ringOnTimeStop++;

        if (ringOnTimeStop > 23)
        {
            ringOnTimeStop = 0;
        }

        blinkControl = 0;
        refreshSettingsDisplay();
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
