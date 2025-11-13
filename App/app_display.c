#include "app_display.h"
#include "app_state.h"
#include "tm1637.h"
#include "sd3077.h"
#include "app_alarm.h"

void refresh_temperture()
{
    // 开启温度转换ADC

    uint32_t value;
    uint32_t srcValue = adcValue[1];  // 读取ADC通道1(NTC)

    // 滑动平均滤波: 缓冲区未满时直接使用原始值
    if (tempBuffered < TEMP_BUFFER_SIZE)
    {
        tempBuffer[tempBuffered++] = (uint16_t)srcValue;
        value = srcValue;
    }
    else
    {
        // 缓冲区已满,计算8次采样的平均值
        tempBuffer[TEMP_BUFFER_SIZE] = (uint16_t)srcValue;
        value = 0;
        for (uint8_t i = 0; i < TEMP_BUFFER_SIZE; i++)
        {
            tempBuffer[i] = tempBuffer[i + 1];  // 数据左移
            value += tempBuffer[i];
        }
        value = value / TEMP_BUFFER_SIZE;
    }

    // 查表法: ADC值转温度(tempertureMap数组存储了-20°C到105°C的ADC对应值)
    for (uint8_t i = 0; i < TEMP_MAP_SIZE; i++)
    {
        if (value <= tempertureMap[i])
        {
            temperture = i;  // 温度 = 数组索引(°C)
            return;
        }
    }

    temperture = 0;  // 超出范围,返回0
    return;
}

void refresh_time_display()
{
    time_now(&time);  // 从SD3077 RTC读取当前时间
    lastTime = time;
    if (currentMode == MODE_SHOW_TIME)
    {
        uint16_t number = time.hours * 100 + time.minutes;
        tm1637_show_number_right(3, number, blinkControl ? 2 : 0xFF, 1);
    }
    else if (currentMode == MODE_SHOW_SECOND)
    {
        tm1637_set_char(0, ' ', false);
        tm1637_set_char(1, ' ', true);
        tm1637_set_char(2, time.seconds / 10 + '0', false);
        tm1637_set_char(3, time.seconds % 10 + '0', false);
    }
    else if (currentMode == MODE_SHOW_TEMPERTURE)
    {
        refresh_temperture();
        if (temperture > 99)
        {
            temperture = 99;
        }
        tm1637_set_char(0, ' ', false);
        tm1637_set_char(1, temperture / 10 + '0', false);
        tm1637_set_char(2, temperture % 10 + '0', false);
        tm1637_set_char(3, 'C', false);
    }

    checkAlarm(); // 检查是否需要触发闹钟
    checkRingOnTime();// 检查整点报时
}

void refresh_settings_display()
{
    char disp[5] = {0};  // 4位数码管显示缓冲
    if (currentMode == MODE_SET_HOUR)
    {
        disp[0] = blinkControl ? ' ' : (lastTime.hours / 10 + '0');
        disp[1] = blinkControl ? ' ' : (lastTime.hours % 10 + '0');
        disp[2] = lastTime.minutes / 10 + '0';
        disp[3] = lastTime.minutes % 10 + '0';
    }
    else if (currentMode == MODE_SET_MINUTE)
    {
        disp[0] = lastTime.hours / 10 + '0';
        disp[1] = lastTime.hours % 10 + '0';
        disp[2] = blinkControl ? ' ' : lastTime.minutes / 10 + '0';
        disp[3] = blinkControl ? ' ' : lastTime.minutes % 10 + '0';
    }
    else if (currentMode == MODE_SET_ALARM_ENABLE)
    {
        disp[0] = 'A';
        disp[1] = 'L';
        disp[2] = blinkControl ? ' ' : 'o';
        disp[3] = blinkControl ? ' ' : isAlarmEnabled ? 'n' : 'F';
    }
    else if (currentMode == MODE_SET_ALARM_HOUR)
    {
        disp[0] = blinkControl ? ' ' : (alarmHour / 10 + '0');
        disp[1] = blinkControl ? ' ' : (alarmHour % 10 + '0');
        disp[2] = alarmMin / 10 + '0';
        disp[3] = alarmMin % 10 + '0';
    }
    else if (currentMode == MODE_SET_ALARM_MINUTE)
    {
        disp[0] = alarmHour / 10 + '0';
        disp[1] = alarmHour % 10 + '0';
        disp[2] = blinkControl ? ' ' : alarmMin / 10 + '0';
        disp[3] = blinkControl ? ' ' : alarmMin % 10 + '0';
    }
    else if (currentMode == MODE_SET_TEMP_SHOW)
    {
        disp[0] = 'T';
        disp[1] = 'S';
        disp[2] = blinkControl ? ' ' : tempertureShowTime / 10 + '0';
        disp[3] = blinkControl ? ' ' : tempertureShowTime % 10 + '0';
    }
    else if (currentMode == MODE_SET_TEMP_HIDE)
    {
        disp[0] = 'T';
        disp[1] = 'H';
        disp[2] = blinkControl ? ' ' : tempertureHideTime / 10 + '0';
        disp[3] = blinkControl ? ' ' : tempertureHideTime % 10 + '0';
    }
    else if (currentMode == MODE_SET_BRIGHTNESS)
    {
        disp[0] = 'b';
        disp[1] = 'r';
        if (savedBrightness)
        {
            disp[2] = blinkControl ? ' ' : '0';
            disp[3] = blinkControl ? ' ' : savedBrightness + '0';
        }
        else
        {
            disp[2] = 'A';
            disp[3] = 'U';
        }
    }
    else if (currentMode == MODE_SET_BRIGHTNESS_STRONG)
    {
        disp[0] = 'b';
        disp[1] = '1';
        disp[2] = blinkControl ? ' ' : '0';
        disp[3] = blinkControl ? ' ' : strongBrightness + '0';
    }
    else if (currentMode == MODE_SET_BRIGHTNESS_WEAK)
    {
        disp[0] = 'b';
        disp[1] = '2';
        disp[2] = blinkControl ? ' ' : '0';
        disp[3] = blinkControl ? ' ' : weakBrightness + '0';
    }
    else if (currentMode == MODE_SET_ROT_ENABLE)
    {
        disp[0] = 'r';
        disp[1] = 'o';
        disp[2] = blinkControl ? ' ' : 'o';
        disp[3] = blinkControl ? ' ' : isRingOnTimeEnabled ? 'n' : 'F';
    }
    else if (currentMode == MODE_SET_ROT_START)
    {
        disp[0] = 'r';
        disp[1] = 'S';
        disp[2] = blinkControl ? ' ' : ringOnTimeStart / 10 + '0';
        disp[3] = blinkControl ? ' ' : ringOnTimeStart % 10 + '0';
    }
    else if (currentMode == MODE_SET_ROT_STOP)
    {
        disp[0] = 'r';
        disp[1] = 'e';
        disp[2] = blinkControl ? ' ' : ringOnTimeStop / 10 + '0';
        disp[3] = blinkControl ? ' ' : ringOnTimeStop % 10 + '0';
    }

    tm1637_set_char(0, disp[0], false);
    tm1637_set_char(1, disp[1], true);
    tm1637_set_char(2, disp[2], false);
    tm1637_set_char(3, disp[3], false);
}
