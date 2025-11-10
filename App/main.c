#include "adc.h"
#include "app_config.h"
#include "app_state.h"
#include "app_display.h"
#include "app_alarm.h"
#include "app_settings.h"
#include "app_keyhandler.h"
#include "dma.h"
#include "exti.h"
#include "gpio.h"
#include "tim.h"
#include "sd3077.h"
#include "stdbool.h"
#include "tm1637.h"
#include "key.h"

void SystemClock_Config(void);

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
    key_init();

    /* 注册按键中断回调 */
    register_key_interrupt_callback(key_interrupt_handler);

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

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

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
