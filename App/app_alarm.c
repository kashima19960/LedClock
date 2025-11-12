#include "app_alarm.h"
#include "app_state.h"
#include "gpio.h"
/*
蜂鸣器低电平响，高电平静音
*/
void checkRingOnTime()
{
    // 启用整点报时
    if (isRingOnTimeEnabled
        // 当前是整点
        && time.minutes == 0
        // 当前整点尚未报时
        && time.hours != lastRingOnTimeHour
        // 闹铃未开启或闹铃设置的时间不是整点或不是当前整点
        && (!isAlarmEnabled || alarmMin != 0 || alarmHour != time.hours))
    {
        // 判断是否在整点报时时间段内
        if (ringOnTimeStart <= ringOnTimeStop)
        {
            // 时间段不跨0点: 例如 8:00-20:00
            if (time.hours >= ringOnTimeStart && time.hours <= ringOnTimeStop)
            {
                ringStartTime = HAL_GetTick();
                HAL_GPIO_WritePin(BUZZER_GPIO_PORT, BUZZER_PIN, GPIO_PIN_RESET);
            }
        }
        else
        {
            // 时间段跨0点: 例如 22:00-6:00
            if (time.hours >= ringOnTimeStart || time.hours <= ringOnTimeStop)
            {
                ringStartTime = HAL_GetTick();
                HAL_GPIO_WritePin(BUZZER_GPIO_PORT, BUZZER_PIN, GPIO_PIN_RESET);
            }
        }

        lastRingOnTimeHour = time.hours; // 记录已报时,防止重复
    }
}

void alarmStart()
{
    isAlarming = true;
    isAlarmed = true;
    HAL_TIM_Base_Start_IT(&ALARM_CONTROL_TIMER_HANDLE);
}

void alarmTimerTick()
{
    uint16_t onTime = 50, offTime = 50, restTime = 500; // 单位:ms
    uint8_t ringCounts = 4;                             // 每轮响铃次数
    alarmTimestamp++;
    if (HAL_GPIO_ReadPin(BUZZER_GPIO_PORT, BUZZER_PIN) == GPIO_PIN_RESET)
    {
        // 短鸣XX毫秒
        if (alarmTimestamp > onTime)
        {
            HAL_GPIO_WritePin(BUZZER_GPIO_PORT, BUZZER_PIN, SET);
            alarmBeepCount++;
            alarmTimestamp = 0;
        }
    }
    else
    {
        // 鸣X+1次
        if (alarmBeepCount < ringCounts)
        {
            // 两次短鸣时间间隔
            if (alarmTimestamp > offTime)
            {
                HAL_GPIO_WritePin(BUZZER_GPIO_PORT, BUZZER_PIN, RESET);
                alarmTimestamp = 0;
            }
        }
        // 鸣X+1次后停止响铃一段时间
        else if (alarmTimestamp > restTime)
        {
            HAL_GPIO_WritePin(BUZZER_GPIO_PORT, BUZZER_PIN, RESET);
            alarmTimestamp = 0;
            alarmBeepCount = 0;
        }
    }
}

void alarmStop()
{
    isAlarming = false;
    HAL_TIM_Base_Stop_IT(&ALARM_CONTROL_TIMER_HANDLE);
    HAL_GPIO_WritePin(BUZZER_GPIO_PORT, BUZZER_PIN, SET);
}

void checkAlarm()
{
    // 时间匹配且闹钟启用,启动响铃
    if (alarmHour == time.hours && alarmMin == time.minutes && isAlarmed == false && isAlarmEnabled)
    {
        alarmStart();
    }

    // 时间不匹配,重置已响铃标志
    if (alarmHour != time.hours || alarmMin != time.minutes)
    {
        if (isAlarming)
        {
            alarmStop();
        }
        isAlarmed = false;
    }
}
