#include "app_alarm.h"
#include "app_state.h"
#include "tim.h"
/*
蜂鸣器低电平响，高电平静音
*/
void check_ring_on_time()
{
    if(is_ring_on_time_enabled  // 整点报时功能启用
        && time.minutes == 0     // 整点
        && time.hours != last_ring_on_time_hour 
        && (is_alarm_enabled == false  // 非闹钟响铃时间
            || alarm_min     != 0 
            || alarm_hour    != time.hours))
    {
        // 判断是否在整点报时时间段内
        if (ring_on_time_start <= ring_on_time_stop)
        {
            // 时间段不跨0点: 例如 8:00-20:00
            if (time.hours >= ring_on_time_start && time.hours <= ring_on_time_stop)
            {
                ring_start_time = HAL_GetTick();
                HAL_GPIO_WritePin(BUZZER_GPIO_PORT, BUZZER_PIN, GPIO_PIN_RESET);
            }
        }
        else 
        {
            // 时间段跨0点: 例如 22:00-6:00
            if (time.hours >= ring_on_time_start || time.hours <= ring_on_time_stop)
            {
                ring_start_time = HAL_GetTick();
                HAL_GPIO_WritePin(BUZZER_GPIO_PORT, BUZZER_PIN, GPIO_PIN_RESET);
            }
        }

        last_ring_on_time_hour = time.hours; // 记录已报时,防止重复
    }
}

void alarm_start()
{
    is_alarming = true;
    is_alarmed  = true;
    HAL_TIM_Base_Start_IT(&ALARM_CONTROL_TIMER_HANDLE);
}

void alarm_timer_tick()
{
    uint16_t on_time    = 50;       // 单位:ms
    uint16_t off_time   = 50;       // 单位:ms
    uint16_t rest_time  = 500;      // 单位:ms
    uint8_t ring_counts = 4;        // 每轮响铃次数
    alarm_timestamp++;
    if (HAL_GPIO_ReadPin(BUZZER_GPIO_PORT, BUZZER_PIN) == GPIO_PIN_RESET)
    {
        // 短鸣XX毫秒
        if (alarm_timestamp > on_time)
        {
            HAL_GPIO_WritePin(BUZZER_GPIO_PORT, BUZZER_PIN, GPIO_PIN_SET);
            alarm_beep_count++;
            alarm_timestamp = 0;
        }
    }
    else
    {
        // 鸣X+1次
        if (alarm_beep_count < ring_counts)
        {
            // 两次短鸣时间间隔
            if (alarm_timestamp > off_time)
            {
                HAL_GPIO_WritePin(BUZZER_GPIO_PORT, BUZZER_PIN, GPIO_PIN_RESET);
                alarm_timestamp = 0;
            }
        }
        // 鸣X+1次后停止响铃一段时间
        else if (alarm_timestamp > rest_time)
        {
            HAL_GPIO_WritePin(BUZZER_GPIO_PORT, BUZZER_PIN, GPIO_PIN_RESET);
            alarm_timestamp = 0;
            alarm_beep_count = 0;
        }
    }
}

void alarm_stop()
{
    is_alarming = false;
    HAL_TIM_Base_Stop_IT(&ALARM_CONTROL_TIMER_HANDLE);
    HAL_GPIO_WritePin(BUZZER_GPIO_PORT, BUZZER_PIN, GPIO_PIN_SET);
}

void check_alarm()
{
    // 时间匹配且闹钟启用,启动响铃
    if(alarm_hour == time.hours
        && alarm_min  == time.minutes
        && is_alarmed == false
        && is_alarm_enabled)
    {
        alarm_start();
    }

    // 时间不匹配,重置已响铃标志
    if (alarm_hour != time.hours || alarm_min != time.minutes)
    {
        if (is_alarming)
        {
            alarm_stop();
        }
        is_alarmed = false;
    }
}
