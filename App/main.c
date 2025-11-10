#include "adc.h"
#include "common.h"
#include "dma.h"
#include "gpio.h"
#include "tim.h"
#include "sd3077.h"
#include "stdbool.h"
#include "tm1637.h"

#define USE_HAL_DRIVER
#define STM32F030x6
#define KEY_LONG_PRESS_EFFECT_TIME 800
#define KEY_REPEAT_TIME_INTERVAL 250
#define KEY_CLICK_EFFECT_TIME 50
#define YEAR_MAX_SET 38
#define YEAR_MIN_SET 15
#define TEMPERTURE_MAX_SHOW_TIME 15
#define TEMPERTURE_MAX_HIDE_TIME 30
#define RING_ON_TIME_LONG 1000
#define STRONG_BRIGHTNESS_ADC_VALUE 2800
#define WEAK_BRIGHTNESS_ADC_VALUE 2300
#define STRONG_BRIGHTNESS_VALUE 8
#define WEAK_BRIGHTNESS_VALUE 1

#define ALARM_CONTROL_TIMER_HANDLE htim17
#define ALARM_CONTROL_TIMER TIM17
#define LIGHT_CONTROL_TIMER_HANDLE htim16
#define LIGHT_CONTROL_TIMER TIM16
#define TEMPERTURE_ADC_HANDLE hadc

#define BAK_DATA_SIZE 13
#define BAK_POWER_DOWN_IND_INDEX 0x00
#define BAK_ALARM_ENABLED_INDEX 0x02
#define BAK_ALARM_HOUR_INDEX 0x03
#define BAK_ALARM_MINUTE_INDEX 0x04
#define BAK_TEMP_SHOW_TIME_INDEX 0x05
#define BAK_TEMP_HIDE_TIME_INDEX 0x06
#define BAK_ROT_ENABLED_INDEX 0x07
#define BAK_ROT_START_INDEX 0x08
#define BAK_ROT_STOP_INDEX 0x09
#define BAK_BRIGHTNESS_INDEX 0x0A
#define BAK_BRIGHTNESS_STRONG_INDEX 0x0B
#define BAK_BRIGHTNESS_WEAK_INDEX 0x0C

#define POWER_DOWN_IND_DATA 0xFA

#define TEMP_BUFFER_SIZE 8
#define TEMP_MAP_SIZE 126
const uint16_t tempertureMap[] = {
    1054, 1091, 1128, 1165, 1203, 1242, 1280, 1320, 1359, 1399, 1439, 1479, 1520, 1560, 1601, 1642, 1683, 1724,
    1765, 1805, 1846, 1887, 1927, 1968, 2008, 2048, 2087, 2126, 2165, 2204, 2242, 2280, 2318, 2355, 2391, 2427,
    2463, 2498, 2533, 2567, 2601, 2634, 2667, 2699, 2730, 2761, 2791, 2821, 2850, 2879, 2907, 2935, 2962, 2988,
    3014, 3040, 3064, 3089, 3113, 3136, 3158, 3181, 3202, 3224, 3244, 3264, 3284, 3303, 3322, 3341, 3359, 3376,
    3393, 3410, 3426, 3442, 3457, 3472, 3487, 3501, 3515, 3529, 3542, 3555, 3568, 3580, 3592, 3603, 3615, 3626,
    3637, 3647, 3657, 3667, 3677, 3687, 3696, 3705, 3714, 3722, 3730, 3739, 3747, 3754, 3762, 3769, 3776, 3783,
    3790, 3797, 3803, 3809, 3815, 3821, 3827, 3833, 3839, 3844, 3849, 3854, 3859, 3864, 3869, 3874, 3878, 3883,
};

DisplayMode currentMode = MODE_SHOW_TIME;
bool isInitCompleted = false;
uint32_t lastDisplayChangeTime;

uint32_t adcValue[2];

uint8_t savedBrightness = 0;
bool isWeakBrightness = true;
uint8_t strongBrightness, weakBrightness;

DateTime time, lastTime;


bool isAlarmEnabled = false;
bool isAlarmed = false, isAlarming = false;
uint8_t alarmHour = 0, alarmMin = 0;

bool isRingOnTimeEnabled;
uint8_t ringOnTimeStart, ringOnTimeStop;
uint8_t lastRingOnTimeHour;
uint32_t ringStartTime;

uint32_t lastModeKeyPressTime, lastSetKeyPressTime, lastSetKeyPressReportTime;
uint32_t alarmTimestamp, alarmBeepCount;
bool setKeyRepeatReported = false;

uint8_t temperture = 25;
uint8_t tempertureShowTime, tempertureHideTime;
uint8_t tempBuffered = 0;
uint16_t tempBuffer[TEMP_BUFFER_SIZE + 1];

void SystemClock_Config(void);

void readBackupSettings()
{
    uint8_t data[3];
    ReadBackData(0, data, 3);

    alarmHour = data[0];
    alarmMin = data[1];
    isAlarmEnabled = data[2];

    // 检查闹铃设置是否合规
    if (alarmHour > 23)
    {
        alarmHour = 0;
    }
    if (alarmMin > 59)
    {
        alarmMin = 0;
    }
}

void saveSettings()
{
    uint8_t data[BAK_DATA_SIZE] = {
        POWER_DOWN_IND_DATA, POWER_DOWN_IND_DATA, isAlarmEnabled,      alarmHour,       alarmMin,
        tempertureShowTime,  tempertureHideTime,  isRingOnTimeEnabled, ringOnTimeStart, ringOnTimeStop,
        savedBrightness,     strongBrightness,    weakBrightness,
    };
    WriteBackData(0, data, BAK_DATA_SIZE);
}

void resetSettings()
{
    isAlarmEnabled = false;
    alarmHour = 0;
    alarmMin = 0;
    tempertureShowTime = 2;
    tempertureHideTime = 10;
    isRingOnTimeEnabled = false;
    ringOnTimeStart = 8;
    ringOnTimeStop = 20;
    savedBrightness = 8;
    strongBrightness = 8;
    weakBrightness = 1;
}

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

        lastRingOnTimeHour = time.hours;  // 记录已报时,防止重复
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
    uint16_t onTime = 50, offTime = 50, restTime = 500;  // 单位:ms
    uint8_t ringCounts = 4;  // 每轮响铃次数
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

void refreshTemperture()
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


void refreshTimeDisplay()
{
    TimeNow(&time);  // 从SD3077 RTC读取当前时间
    lastTime = time;
    if (currentMode == MODE_SHOW_TIME)
    {
        uint16_t number = time.hours * 100 + time.minutes;
        TM1637ShowNumberRight(3, number, blinkControl ? 2 : 0xFF, 1);
    }
    else if (currentMode == MODE_SHOW_SECOND)
    {
        TM1637SetChar(0, ' ', false);
        TM1637SetChar(1, ' ', true);
        TM1637SetChar(2, time.seconds / 10 + '0', false);
        TM1637SetChar(3, time.seconds % 10 + '0', false);
    }
    else if (currentMode == MODE_SHOW_TEMPERTURE)
    {
        refreshTemperture();
        if (temperture > 99)
        {
            temperture = 99;
        }
        TM1637SetChar(0, ' ', false);
        TM1637SetChar(1, temperture / 10 + '0', false);
        TM1637SetChar(2, temperture % 10 + '0', false);
        TM1637SetChar(3, 'C', false);
    }

    checkAlarm();
    checkRingOnTime();
}

void refreshSettingsDisplay()
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

    TM1637SetChar(0, disp[0], false);
    TM1637SetChar(1, disp[1], true);
    TM1637SetChar(2, disp[2], false);
    TM1637SetChar(3, disp[3], false);
}

void modeKeyClicked()
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
            // 设置时间
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

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    dma_init();
    sd3077_iic_init();
    sec_int_gpio_init();
    adc_init();
    tim17_init();
    tim3_init();
    tim16_init();

    uint8_t backupData[BAK_DATA_SIZE];
    ReadBackData(BAK_POWER_DOWN_IND_INDEX, backupData, BAK_DATA_SIZE);
    //非正常关机，初始化时间为2020-01-01 00:00:00
    if (backupData[0] != POWER_DOWN_IND_DATA && backupData[1] != POWER_DOWN_IND_DATA)
    {
        time.year = YEAR_MIN_SET;
        time.month = 1;
        time.dayOfMonth = 1;
        time.dayOfWeek = 1;
        time.hours = 0;
        time.minutes = 0;
        time.ampm = HOUR24;
        time.seconds = 0;
        SetTime(&time);

        resetSettings();
        saveSettings();
    }
    else//正常关机，读取设置
    {
        isAlarmEnabled = backupData[BAK_ALARM_ENABLED_INDEX];
        alarmHour = backupData[BAK_ALARM_HOUR_INDEX];
        alarmMin = backupData[BAK_ALARM_MINUTE_INDEX];
        tempertureShowTime = backupData[BAK_TEMP_SHOW_TIME_INDEX];
        tempertureHideTime = backupData[BAK_TEMP_HIDE_TIME_INDEX];
        isRingOnTimeEnabled = backupData[BAK_ROT_ENABLED_INDEX];
        ringOnTimeStart = backupData[BAK_ROT_START_INDEX];
        ringOnTimeStop = backupData[BAK_ROT_STOP_INDEX];
        savedBrightness = backupData[BAK_BRIGHTNESS_INDEX];
        strongBrightness = backupData[BAK_BRIGHTNESS_STRONG_INDEX];
        weakBrightness = backupData[BAK_BRIGHTNESS_WEAK_INDEX];
    }

    if (alarmHour > (uint8_t)23 || 
        alarmMin > (uint8_t)59 || 
        ringOnTimeStart > 23 || 
        ringOnTimeStop > 23 ||
        savedBrightness > 8 || 
        strongBrightness > 8 || 
        strongBrightness == 0 || 
        weakBrightness > 8 ||
        weakBrightness == 0)
    {
        resetSettings();
         saveSettings();
    }

    tm1637_init();
    if (savedBrightness != 0)
    {
        TM1637SetBrightness(savedBrightness);
    }
    else
    {
        TM1637SetBrightness(STRONG_BRIGHTNESS_VALUE);
        isWeakBrightness = false;
    }

    HAL_ADCEx_Calibration_Start(&hadc);
    HAL_ADC_Start_DMA(&hadc, adcValue, 2);
    HAL_TIM_Base_Start(&htim3);

    HAL_TIM_Base_Start_IT(&LIGHT_CONTROL_TIMER_HANDLE);

    TimeNow(&time);
    lastRingOnTimeHour = time.hours;

    EnableSencodInterruptOuput();

    lastDisplayChangeTime = HAL_GetTick();

    isInitCompleted = true;

    uint32_t now = 0, passedTime;
    //蜂鸣器高电平关闭，低电平响铃
    while (1)
    {
        if (HAL_GPIO_ReadPin(BUZZER_GPIO_PORT, BUZZER_PIN) == GPIO_PIN_RESET && !isAlarming)
        {
            now = HAL_GetTick();
            if (now < ringStartTime || (now - ringStartTime >= RING_ON_TIME_LONG))
            {
                //关闭蜂鸣器
                HAL_GPIO_WritePin(BUZZER_GPIO_PORT, BUZZER_PIN, GPIO_PIN_SET);
            }
        }

        if (tempertureShowTime > 0)
        {
            if (currentMode == MODE_SHOW_TIME)
            {
                now = HAL_GetTick();
                passedTime = now - lastDisplayChangeTime;
                if (now < lastDisplayChangeTime || passedTime >= (tempertureHideTime * 1000))
                {
                    currentMode = MODE_SHOW_TEMPERTURE;
                    lastDisplayChangeTime = now;
                }
            }
            else if (currentMode == MODE_SHOW_TEMPERTURE && tempertureHideTime > 0)
            {
                now = HAL_GetTick();
                passedTime = now - lastDisplayChangeTime;
                if (now < lastDisplayChangeTime || passedTime >= (tempertureShowTime * 1000))
                {
                    currentMode = MODE_SHOW_TIME;
                    lastDisplayChangeTime = now;
                }
            }
        }
        else if (currentMode == MODE_SHOW_TEMPERTURE)
        {
            currentMode = MODE_SHOW_TIME;
            lastDisplayChangeTime = now;
        }

        if (HAL_GPIO_ReadPin(SET_KEY_GPIO_PORT, SET_KEY_PIN) == GPIO_PIN_RESET && currentMode >= MODE_SET_HOUR &&
            currentMode <= MODE_SET_ROT_STOP)
        {
            uint32_t curVal = HAL_GetTick();
            uint32_t timePassed = curVal - lastSetKeyPressTime;
            if (timePassed > KEY_LONG_PRESS_EFFECT_TIME)
            {
                if (curVal - lastSetKeyPressReportTime > KEY_REPEAT_TIME_INTERVAL)
                {
                    setKeyPresseRepeatReport();
                    lastSetKeyPressReportTime = curVal;
                }
            }
        }
    }
}

/**
 * 项目使用HSI,进入PLL倍频前有个二分频，然后PLL选4，最终SYSCLK为16MHz，具体的可以参照时钟树
 * HSI: 8 MHz
 * HSI14: 14 MHz
 *   - SYSCLK : 16 MHz
 *   - HCLK : 16 MHz
 *   - PCLK1 : 16 MHz
 *   - I2C1 : HSI = 8 MHz
 *   - ADC : HSI14 = 14 MHz (max 14 MHz)
 */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_HSI14;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSI14State = RCC_HSI14_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.HSI14CalibrationValue = 16;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL4;
    RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
    {
        Error_Handler();
    }
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1;
    PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
        Error_Handler();
    }
}

void Error_Handler(void)
{
}