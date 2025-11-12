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
    if (isAlarming)
    {
        alarmStop();
        return;
    }

    /* 显示模式: 进入设置 */
    if (currentMode == MODE_SHOW_TIME || currentMode == MODE_SHOW_TEMPERTURE)
    {
        currentMode = MODE_SET_HOUR;
        blinkControl = 0xFF;
        refreshSettingsDisplay();
        set_interrupt_output(F_2_HZ); /* 设置RTC输出2Hz用于闪烁 */
    }
    else if (currentMode == MODE_SHOW_SECOND)
    {
        /* 显示秒时按MODE键: 清零秒数 */
        lastTime.seconds = 0;
        set_time(&lastTime);
        refreshTimeDisplay();
    }
    /* 设置模式: 逐级切换设置项 */
    else if (currentMode == MODE_SET_HOUR)
    {
        currentMode = MODE_SET_MINUTE;
        blinkControl = 0xFF;
        refreshSettingsDisplay();
    }
    else if (currentMode == MODE_SET_MINUTE)
    {
        currentMode = MODE_SET_ALARM_ENABLE;
        blinkControl = 0x00;
        refreshSettingsDisplay();
    }
    else if (currentMode == MODE_SET_ALARM_ENABLE)
    {
        currentMode = isAlarmEnabled ? MODE_SET_ALARM_HOUR : MODE_SET_TEMP_SHOW;
        blinkControl = 0xFF;
        refreshSettingsDisplay();
    }
    else if (currentMode == MODE_SET_ALARM_HOUR)
    {
        currentMode = MODE_SET_ALARM_MINUTE;
        blinkControl = 0xFF;
        refreshSettingsDisplay();
    }
    else if (currentMode == MODE_SET_ALARM_MINUTE)
    {
        currentMode = MODE_SET_TEMP_SHOW;
        blinkControl = 0x00;
        refreshSettingsDisplay();
    }
    else if (currentMode == MODE_SET_TEMP_SHOW)
    {
        if (tempertureShowTime != 0)
        {
            currentMode = MODE_SET_TEMP_HIDE;
        }
        else
        {
            currentMode = MODE_SET_BRIGHTNESS;
        }

        blinkControl = 0x00;
        refreshSettingsDisplay();
    }
    else if (currentMode == MODE_SET_TEMP_HIDE)
    {
        currentMode = MODE_SET_BRIGHTNESS;
        blinkControl = 0x00;
        refreshSettingsDisplay();
    }
    else if (currentMode == MODE_SET_BRIGHTNESS)
    {
        if (savedBrightness == 0)
        {
            currentMode = MODE_SET_BRIGHTNESS_STRONG;
        }
        else
        {
            currentMode = MODE_SET_ROT_ENABLE;
        }

        blinkControl = 0x00;
        refreshSettingsDisplay();
    }
    else if (currentMode == MODE_SET_BRIGHTNESS_STRONG)
    {
        currentMode = MODE_SET_BRIGHTNESS_WEAK;
        blinkControl = 0x00;
        tm1637_set_brightness(weakBrightness);
        refreshSettingsDisplay();
    }
    else if (currentMode == MODE_SET_BRIGHTNESS_WEAK)
    {
        currentMode = MODE_SET_ROT_ENABLE;
        blinkControl = 0x00;
        tm1637_set_brightness(strongBrightness);
        isWeakBrightness = false;
        refreshSettingsDisplay();
    }
    else if (currentMode == MODE_SET_ROT_ENABLE)
    {
        blinkControl = 0x00;

        if (!isRingOnTimeEnabled)//isRingOnTimeEnabled=false
        {
            currentMode = MODE_SHOW_TIME;
            time_now(&time);
            lastTime.seconds = time.seconds;
            set_time(&lastTime);
            lastRingOnTimeHour = lastTime.hours;

            saveSettings();

            refreshTimeDisplay();
            enable_second_interrupt_output();
            lastDisplayChangeTime = HAL_GetTick();
        }
        else
        {
            currentMode = MODE_SET_ROT_START;
            refreshSettingsDisplay();
        }
    }
    else if (currentMode == MODE_SET_ROT_START)
    {
        currentMode = MODE_SET_ROT_STOP;
        blinkControl = 0x00;
        refreshSettingsDisplay();
    }
    else if (currentMode == MODE_SET_ROT_STOP)
    {
        /* 最后一个设置项: 保存并退出设置模式 */
        currentMode = MODE_SHOW_TIME;

        /* 将修改后的时间写入RTC */
        time_now(&time);
        lastTime.seconds = time.seconds;
        set_time(&lastTime);
        lastRingOnTimeHour = lastTime.hours;

        saveSettings(); /* 保存所有设置到备份寄存器 */

        refreshTimeDisplay();
        enable_second_interrupt_output(); /* 恢复1Hz秒中断 */
        lastDisplayChangeTime = HAL_GetTick();
    }
}

/**
 * @brief MODE键长按事件处理
 * @note 长按直接退出设置模式
 */
void mode_key_long_pressed(void)
{
    if (isAlarming)
    {
        alarmStop();
        return;
    }

    if (currentMode >= MODE_SET_HOUR && currentMode <= MODE_SET_ROT_STOP)
    {
        currentMode = MODE_SHOW_TIME; /* 放弃修改,直接退出 */
        refreshTimeDisplay();
        lastDisplayChangeTime = HAL_GetTick();
    }
}

/**
 * @brief MODE键按下事件
 * @note 记录按下时间
 */
void mode_key_pressed(void)
{
    lastModeKeyPressTime = HAL_GetTick();
}

/**
 * @brief MODE键释放事件
 * @note 根据按下时长判断是单击还是长按
 */
void mode_key_released(void)
{
    uint32_t current_val;

    current_val = HAL_GetTick();

    if (lastModeKeyPressTime > current_val)
    {
        mode_key_clicked();
    }
    else if (current_val - lastModeKeyPressTime > KEY_LONG_PRESS_EFFECT_TIME)
    {
        mode_key_long_pressed();
    }
    else if (current_val - lastModeKeyPressTime > KEY_CLICK_EFFECT_TIME)
    {
        mode_key_clicked();
    }
}
