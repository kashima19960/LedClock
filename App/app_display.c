#include "app_display.h"
#include "app_state.h"
#include "tm1637.h"
#include "sd3077.h"
#include "app_alarm.h"

void refresh_temperture()
{
    // 开启温度转换ADC
    uint32_t value;
    uint32_t src_value = adc_value[1];  // 读取ADC通道1(NTC)

    // 滑动平均滤波: 缓冲区未满时直接使用原始值
    if (temp_buffered < TEMP_BUFFER_SIZE)
    {
        temp_buffer[temp_buffered++] = (uint16_t)src_value;
        value = src_value;
    }
    else
    {
        // 缓冲区已满,计算8次采样的平均值
        temp_buffer[TEMP_BUFFER_SIZE] = (uint16_t)src_value;
        value = 0;
        for (uint8_t i = 0; i < TEMP_BUFFER_SIZE; i++)
        {
            temp_buffer[i] = temp_buffer[i + 1];  // 数据左移
            value += temp_buffer[i];
        }
        value = value / TEMP_BUFFER_SIZE;
    }
    // 查表法: ADC值转温度(temperature_map数组存储了-20°C到105°C的ADC对应值)
    for (uint8_t i = 0; i < TEMP_MAP_SIZE; i++)
    {
        if (value <= temperature_map[i])
        {
            temperature = i;  // 温度 = 数组索引(°C)
            return;
        }
    }
    temperature = 0;  // 超出范围,返回0
    return;
}

void refresh_time_display()
{
    time_now(&time);  // 从SD3077 RTC读取当前时间
    last_time = time;
    if (current_mode == MODE_SHOW_TIME)
    {
        uint16_t number = time.hours * 100 + time.minutes;
        tm1637_show_number_right(3, number, blink_control ? 2 : 0xFF, 1);
    }
    else if (current_mode == MODE_SHOW_SECOND)
    {
        tm1637_set_char(0, ' ', false);
        tm1637_set_char(1, ' ', true);
        tm1637_set_char(2, time.seconds / 10 + '0', false);
        tm1637_set_char(3, time.seconds % 10 + '0', false);
    }
    else if (current_mode == MODE_SHOW_TEMPERTURE)
    {
        refresh_temperture();
        if (temperature > 99)
        {
            temperature = 99;
        }
        tm1637_set_char(0, ' ', false);
        tm1637_set_char(1, temperature / 10 + '0', false);
        tm1637_set_char(2, temperature % 10 + '0', false);
        tm1637_set_char(3, 'C', false);
    }

    check_alarm(); // 检查是否需要触发闹钟
    check_ring_on_time();// 检查整点报时
}

void refresh_settings_display()
{
    char disp[5] = {0};  // 4位数码管显示缓冲
    if (current_mode == MODE_SET_HOUR)
    {
        disp[0] = blink_control ? ' ' : (last_time.hours / 10 + '0');
        disp[1] = blink_control ? ' ' : (last_time.hours % 10 + '0');
        disp[2] = last_time.minutes / 10 + '0';
        disp[3] = last_time.minutes % 10 + '0';
    }
    else if (current_mode == MODE_SET_MINUTE)
    {
        disp[0] = last_time.hours / 10 + '0';
        disp[1] = last_time.hours % 10 + '0';
        disp[2] = blink_control ? ' ' : last_time.minutes / 10 + '0';
        disp[3] = blink_control ? ' ' : last_time.minutes % 10 + '0';
    }
    else if (current_mode == MODE_SET_ALARM_ENABLE)
    {
        disp[0] = 'A';
        disp[1] = 'L';
        disp[2] = blink_control ? ' ' : 'o';
        disp[3] = blink_control ? ' ' : is_alarm_enabled ? 'n' : 'F';
    }
    else if (current_mode == MODE_SET_ALARM_HOUR)
    {
        disp[0] = blink_control ? ' ' : (alarm_hour / 10 + '0');
        disp[1] = blink_control ? ' ' : (alarm_hour % 10 + '0');
        disp[2] = alarm_min / 10 + '0';
        disp[3] = alarm_min % 10 + '0';
    }
    else if (current_mode == MODE_SET_ALARM_MINUTE)
    {
        disp[0] = alarm_hour / 10 + '0';
        disp[1] = alarm_hour % 10 + '0';
        disp[2] = blink_control ? ' ' : alarm_min / 10 + '0';
        disp[3] = blink_control ? ' ' : alarm_min % 10 + '0';
    }
    else if (current_mode == MODE_SET_TEMP_SHOW)
    {
        disp[0] = 'T';
        disp[1] = 'S';
        disp[2] = blink_control ? ' ' : temperature_show_time / 10 + '0';
        disp[3] = blink_control ? ' ' : temperature_show_time % 10 + '0';
    }
    else if (current_mode == MODE_SET_TEMP_HIDE)
    {
        disp[0] = 'T';
        disp[1] = 'H';
        disp[2] = blink_control ? ' ' : temperature_hide_time / 10 + '0';
        disp[3] = blink_control ? ' ' : temperature_hide_time % 10 + '0';
    }
    else if (current_mode == MODE_SET_BRIGHTNESS)
    {
        disp[0] = 'b';
        disp[1] = 'r';
        if (save_brightness)
        {
            disp[2] = blink_control ? ' ' : '0';
            disp[3] = blink_control ? ' ' : save_brightness + '0';
        }
        else
        {
            disp[2] = 'A';
            disp[3] = 'U';
        }
    }
    else if (current_mode == MODE_SET_BRIGHTNESS_STRONG)
    {
        disp[0] = 'b';
        disp[1] = '1';
        disp[2] = blink_control ? ' ' : '0';
        disp[3] = blink_control ? ' ' : strong_brightness + '0';
    }
    else if (current_mode == MODE_SET_BRIGHTNESS_WEAK)
    {
        disp[0] = 'b';
        disp[1] = '2';
        disp[2] = blink_control ? ' ' : '0';
        disp[3] = blink_control ? ' ' : weak_brightness + '0';
    }
    else if (current_mode == MODE_SET_ROT_ENABLE)
    {
        disp[0] = 'r';
        disp[1] = 'o';
        disp[2] = blink_control ? ' ' : 'o';
        disp[3] = blink_control ? ' ' : is_ring_on_time_enabled ? 'n' : 'F';
    }
    else if (current_mode == MODE_SET_ROT_START)
    {
        disp[0] = 'r';
        disp[1] = 'S';
        disp[2] = blink_control ? ' ' : ring_on_time_start / 10 + '0';
        disp[3] = blink_control ? ' ' : ring_on_time_start % 10 + '0';
    }
    else if (current_mode == MODE_SET_ROT_STOP)
    {
        disp[0] = 'r';
        disp[1] = 'e';
        disp[2] = blink_control ? ' ' : ring_on_time_stop / 10 + '0';
        disp[3] = blink_control ? ' ' : ring_on_time_stop % 10 + '0';
    }

    tm1637_set_char(0, disp[0], false);
    tm1637_set_char(1, disp[1], true);
    tm1637_set_char(2, disp[2], false);
    tm1637_set_char(3, disp[3], false);
}
