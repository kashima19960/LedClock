#include "adc.h"
#include "common.h"
#include "dma.h"
#include "gpio.h"
#include "i2c.h"
#include "tim.h"

#include "sd3077.h"
#include "stdbool.h"
#include "tm1637.h"

#define USE_HAL_DRIVER
#define STM32F030x6
typedef enum
{
    MODE_SHOW_TIME = 0,
    MODE_SHOW_SECOND,
    MODE_SHOW_TEMPERTURE,
    MODE_SET_HOUR,
    MODE_SET_MINUTE,
    MODE_SET_ALARM_ENABLE,
    MODE_SET_ALARM_HOUR,
    MODE_SET_ALARM_MINUTE,
    MODE_SET_TEMP_SHOW,
    MODE_SET_TEMP_HIDE,
    MODE_SET_BRIGHTNESS,
    MODE_SET_BRIGHTNESS_STRONG,
    MODE_SET_BRIGHTNESS_WEAK,
    MODE_SET_ROT_ENABLE,
    MODE_SET_ROT_START,
    MODE_SET_ROT_STOP,
} DisplayMode;

#define SECOND_INT_PIN SEC_INT_Pin
#define SECOND_INT_EXTI_IRQN SEC_INT_EXTI_IRQn
#define KEY_EXTI_IRQN MODE_KEY_EXTI_IRQn

#define MODE_KEY_GPIO_PORT MODE_KEY_GPIO_Port
#define MODE_KEY_PIN MODE_KEY_Pin

#define SET_KEY_GPIO_PORT SET_KEY_GPIO_Port
#define SET_KEY_PIN SET_KEY_Pin

#define BUZZER_GPIO_PORT BUZZER_GPIO_Port
#define BUZZER_PIN BUZZER_Pin

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
uint8_t blinkControl;

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
}

void resetSettings()
{
}

void checkRingOnTime()
{
}

void alarmStart()
{
}

void alarmTimerTick()
{
}

void alarmStop()
{
}

void checkAlarm()
{
}

void refreshTemperture()
{
}

void refreshTimeDisplay()
{
}

void refreshSettingsDisplay()
{
}

void modeKeyClicked()
{
}

void modeKeyLongPressed()
{
}

void setKeyClicked()
{
}

void setKeyPresseRepeatReport()
{
}

void setKeyPressed()
{
}

void setKeyReleased()
{
}

void modeKeyPressed()
{
}

void modeKeyReleased()
{
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
    else
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

    bool hasError = false;
    if (alarmHour > (uint8_t)23 || alarmMin > (uint8_t)59)
    {
        hasError = true;
    }
    if (ringOnTimeStart > 23 || ringOnTimeStop > 23)
    {
        hasError = true;
    }
    if (savedBrightness > 8 || strongBrightness > 8 || strongBrightness == 0 || weakBrightness > 8 ||
        weakBrightness == 0)
    {
        hasError = true;
    }

    if (hasError)
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
    while (1)
    {
        if (HAL_GPIO_ReadPin(BUZZER_GPIO_PORT, BUZZER_PIN) == GPIO_PIN_RESET && !isAlarming)
        {
            now = HAL_GetTick();
            if (now < ringStartTime || (now - ringStartTime >= RING_ON_TIME_LONG))
            {
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
 * 项目使用HSI,进入PLL倍频前有个二分频，然后PLL选4，最终SYSCLK为32MHz，具体的可以参照时钟树
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