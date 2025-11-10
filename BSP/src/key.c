#include "key.h"#include "key.h"



/**/**

 * @brief 按键GPIO初始化 * @brief 按键GPIO初始化

 * @note 配置MODE键和SET键为上升沿和下降沿触发中断 * @note 配置MODE键和SET键为上升沿和下降沿触发中断

 */ */

void key_init(void)void key_init(void)

{{

    GPIO_InitTypeDef gpio_init_struct;    GPIO_InitTypeDef gpio_init_struct;

        

    MODE_KEY_CLK_ENABLE();    MODE_KEY_CLK_ENABLE();

    SET_KEY_CLK_ENABLE();    SET_KEY_CLK_ENABLE();



    gpio_init_struct.Pin = MODE_KEY_PIN | SET_KEY_PIN;    gpio_init_struct.Pin = MODE_KEY_PIN | SET_KEY_PIN;

    gpio_init_struct.Mode = GPIO_MODE_IT_RISING_FALLING;    gpio_init_struct.Mode = GPIO_MODE_IT_RISING_FALLING;

    gpio_init_struct.Pull = GPIO_PULLUP;    gpio_init_struct.Pull = GPIO_PULLUP;

    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;

    HAL_GPIO_Init(GPIOA, &gpio_init_struct);    HAL_GPIO_Init(GPIOA, &gpio_init_struct);



    HAL_NVIC_SetPriority(EXTI2_3_IRQn, 0, 0);    HAL_NVIC_SetPriority(EXTI2_3_IRQn, 0, 0);

    HAL_NVIC_EnableIRQ(EXTI2_3_IRQn);    HAL_NVIC_EnableIRQ(EXTI2_3_IRQn);

}}


{
    // 响铃时任意键停止
    if (isAlarming)
    {
        alarmStop();
        return;
    }

    // 显示模式: 进入设置
    if (currentMode == MODE_SHOW_TIME || currentMode == MODE_SHOW_TEMPERTURE)
    {
        currentMode = MODE_SET_HOUR;  // 进入设置小时模式
        blinkControl = 0xFF;
        refreshSettingsDisplay();
        SetInterruptOuput(F_2_HZ);  // 设置RTC输出2Hz用于闪烁
    }
    else if (currentMode == MODE_SHOW_SECOND)
    {
        // 显示秒时按MODE键: 清零秒数
        lastTime.seconds = 0;
        SetTime(&lastTime);
        refreshTimeDisplay();
    }
    // 设置模式: 逐级切换设置项
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
        TM1637SetBrightness(weakBrightness);
        refreshSettingsDisplay();
    }
    else if (currentMode == MODE_SET_BRIGHTNESS_WEAK)
    {
        currentMode = MODE_SET_ROT_ENABLE;
        blinkControl = 0x00;
        TM1637SetBrightness(strongBrightness);
        isWeakBrightness = false;
        refreshSettingsDisplay();
    }
    else if (currentMode == MODE_SET_ROT_ENABLE)
    {
        blinkControl = 0x00;
        if (!isRingOnTimeEnabled)
        {
            currentMode = MODE_SHOW_TIME;
            TimeNow(&time);
            lastTime.seconds = time.seconds;
            SetTime(&lastTime);
            lastRingOnTimeHour = lastTime.hours;

            saveSettings();

            refreshTimeDisplay();
            EnableSencodInterruptOuput();
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
        // 最后一个设置项: 保存并退出设置模式
        currentMode = MODE_SHOW_TIME;
        // 将修改后的时间写入RTC
        TimeNow(&time);
        lastTime.seconds = time.seconds;
        SetTime(&lastTime);
        lastRingOnTimeHour = lastTime.hours;
        saveSettings();  // 保存所有设置到备份寄存器
        refreshTimeDisplay();
        EnableSencodInterruptOuput();  // 恢复1Hz秒中断
        lastDisplayChangeTime = HAL_GetTick();
    }
}

void modeKeyLongPressed()
{
    if (isAlarming)
    {
        alarmStop();
        return;
    }

    if (currentMode >= MODE_SET_HOUR && currentMode <= MODE_SET_ROT_STOP)
    {
        currentMode = MODE_SHOW_TIME;  // 放弃修改,直接退出
        refreshTimeDisplay();
        lastDisplayChangeTime = HAL_GetTick();
    }
}

void setKeyClicked()
{
    if (isAlarming)
    {
        alarmStop();
        return;
    }

    // 显示模式: 切换到显示秒
    if (currentMode == MODE_SHOW_TIME || currentMode == MODE_SHOW_TEMPERTURE)
    {
        currentMode = MODE_SHOW_SECOND;
        refreshTimeDisplay();
    }
    else if (currentMode == MODE_SHOW_SECOND)
    {
        currentMode = MODE_SHOW_TIME;  // 返回显示时间
        refreshTimeDisplay();
        lastDisplayChangeTime = HAL_GetTick();
    }
    // 设置模式: 增加当前设置项的值
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

void setKeyPresseRepeatReport()
{
    setKeyRepeatReported = true;

    // 只对需要快速调整的模式生效
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
 * @brief SET键按下事件(记录按下时间)
 */
void setKeyPressed()
{
    lastSetKeyPressTime = HAL_GetTick();
}

void setKeyReleased()
{
    uint32_t currentVal = HAL_GetTick();
    if (lastSetKeyPressTime > currentVal)
    {
        setKeyClicked();
    }
    else if (currentVal - lastSetKeyPressTime > KEY_CLICK_EFFECT_TIME && !setKeyRepeatReported)
    {
        setKeyClicked();
    }
    setKeyRepeatReported = false;
}

void modeKeyPressed()
{
    lastModeKeyPressTime = HAL_GetTick();
}

void modeKeyReleased()
{
    uint32_t currentVal = HAL_GetTick();
    if (lastModeKeyPressTime > currentVal)
    {
        modeKeyClicked();
    }
    else if (currentVal - lastModeKeyPressTime > KEY_LONG_PRESS_EFFECT_TIME)
    {
        modeKeyLongPressed();
    }
    else if (currentVal - lastModeKeyPressTime > KEY_CLICK_EFFECT_TIME)
    {
        modeKeyClicked();
    }
}

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
