#include "adc.h"
#include "app_alarm.h"
#include "app_config.h"
#include "app_display.h"
#include "app_interrupt_handler.h"
#include "app_settings.h"
#include "app_state.h"
#include "delay.h"
#include "exti.h"
#include "function_test.h"
#include "key.h"
#include "sd3077.h"
#include "stdbool.h"
#include "tim.h"
#include "tm1637.h"
#include "buzzer.h"

#define DEBUG 0
#define ENABLE_FUNCTION_TEST 1
#define FUNCTION_TEST_NO_INTERRUPTS 0
void SystemClock_Config(void);

static void function_key_gpio_init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = MODE_KEY_PIN | SET_KEY_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    delay_init(16);
#if FUNCTION_TEST_NO_INTERRUPTS
    dma_init();
    sd3077_iic_init();
    buzzer_init();
    sd3077_sec_int_gpio_init();
    adc_init();
    tim17_init();
    tim3_init();
    tim16_init();
    key_init();
    register_exti_interrupt_callback(exti_interrupt_handler);
    register_timer_interrupt_callback(tim_interrupt_handler);
    uint8_t backup_data[BAK_DATA_SIZE];
    read_backup_data(BAK_POWER_DOWN_IND_INDEX, backup_data, BAK_DATA_SIZE);

#endif

#if ENABLE_FUNCTION_TEST
    function_key_gpio_init();
    test_run_entry();
#endif

#if DEBUG
    // 非正常关机，初始化时间为2020-01-01 00:00:00
    if (backup_data[0] != POWER_DOWN_IND_DATA && backup_data[1] != POWER_DOWN_IND_DATA)
    {
        time.year = YEAR_MIN_SET;
        time.month = 1;
        time.day_of_month = 1;
        time.day_of_week = 1;
        time.hours = 0;
        time.minutes = 0;
        time.ampm = HOUR24;
        time.seconds = 0;
        set_time(&time);

        reset_settings();
        save_settings();
    }
    else // 正常关机，读取设置
    {
        is_alarm_enabled = backup_data[BAK_ALARM_ENABLED_INDEX];
        alarm_hour = backup_data[BAK_ALARM_HOUR_INDEX];
        alarm_min = backup_data[BAK_ALARM_MINUTE_INDEX];
        temperature_show_time = backup_data[BAK_TEMP_SHOW_TIME_INDEX];
        temperature_hide_time = backup_data[BAK_TEMP_HIDE_TIME_INDEX];
        is_ring_on_time_enabled = backup_data[BAK_ROT_ENABLED_INDEX];
        ring_on_time_start = backup_data[BAK_ROT_START_INDEX];
        ring_on_time_stop = backup_data[BAK_ROT_STOP_INDEX];
        save_brightness = backup_data[BAK_BRIGHTNESS_INDEX];
        strong_brightness = backup_data[BAK_BRIGHTNESS_STRONG_INDEX];
        weak_brightness = backup_data[BAK_BRIGHTNESS_WEAK_INDEX];
    }

    if (alarm_hour > (uint8_t)23 || alarm_min > (uint8_t)59 || ring_on_time_start > 23 || ring_on_time_stop > 23 ||
        save_brightness > 8 || strong_brightness > 8 || strong_brightness == 0 || weak_brightness > 8 ||
        weak_brightness == 0)
    {
        reset_settings();
        save_settings();
    }

    tm1637_init();

    if (save_brightness != 0)
    {
        // tm1637_set_brightness(save_brightness);
        tm1637_set_brightness(4);
    }
    else
    {
        // tm1637_set_brightness(STRONG_BRIGHTNESS_VALUE);
        tm1637_set_brightness(4);
        is_weak_brightness = false;
    }

    HAL_ADCEx_Calibration_Start(&g_adc_handle);
    HAL_ADC_Start_DMA(&g_adc_handle, adc_value, 2);
    HAL_TIM_Base_Start(&g_tim3_handle);

    HAL_TIM_Base_Start_IT(&LIGHT_CONTROL_TIMER_HANDLE);

    time_now(&time);
    last_ring_on_time_hour = time.hours;

    enable_second_interrupt_output();

    last_display_change_time = HAL_GetTick();

    is_init_completed = true;

    uint32_t now = 0, passed_time;
    // 蜂鸣器高电平关闭，低电平响铃
    while (1)
    {
        if (HAL_GPIO_ReadPin(BUZZER_GPIO_PORT, BUZZER_PIN) == GPIO_PIN_RESET && !is_alarming)
        {
            now = HAL_GetTick();
            if (now < ring_start_time || (now - ring_start_time >= RING_ON_TIME_LONG))
            {
                // 关闭蜂鸣器
                HAL_GPIO_WritePin(BUZZER_GPIO_PORT, BUZZER_PIN, GPIO_PIN_SET);
            }
        }

        if (temperature_show_time > 0)
        {
            if (current_mode == MODE_SHOW_TIME)
            {
                now = HAL_GetTick();
                passed_time = now - last_display_change_time;
                if (now < last_display_change_time || passed_time >= (temperature_hide_time * 1000))
                {
                    current_mode = MODE_SHOW_TEMPERTURE;
                    last_display_change_time = now;
                }
            }
            else if (current_mode == MODE_SHOW_TEMPERTURE && temperature_hide_time > 0)
            {
                now = HAL_GetTick();
                passed_time = now - last_display_change_time;
                if (now < last_display_change_time || passed_time >= (temperature_show_time * 1000))
                {
                    current_mode = MODE_SHOW_TIME;
                    last_display_change_time = now;
                }
            }
        }
        else if (current_mode == MODE_SHOW_TEMPERTURE)
        {
            current_mode = MODE_SHOW_TIME;
            last_display_change_time = now;
        }

        if (HAL_GPIO_ReadPin(SET_KEY_GPIO_PORT, SET_KEY_PIN) == GPIO_PIN_RESET && current_mode >= MODE_SET_HOUR &&
            current_mode <= MODE_SET_ROT_STOP)
        {
            uint32_t cur_val = HAL_GetTick();
            uint32_t time_passed = cur_val - last_set_key_press_time;
            if (time_passed > KEY_LONG_PRESS_EFFECT_TIME)
            {
                if (cur_val - last_set_key_press_report_time > KEY_REPEAT_TIME_INTERVAL)
                {
                    set_key_presse_repeat_report();
                    last_set_key_press_report_time = cur_val;
                }
            }
        }
    }
#endif
}
/**
 * @brief  系统时钟配置
 * System Clock source  = PLL (HSI)
 * SYSCLK(Hz)           = 16,000,000 (16 MHz)
 * HCLK(Hz)             = 16,000,000 (16 MHz)
 * AHB Prescaler        = 1
 * APB1 Prescaler       = 1
 * HSI Frequency(Hz)    = 8,000,000
 * PLLMUL               = 4 (HSI/2 * 4)
 * I2C1 Clock           = HSI (8 MHz)
 * HSI14                = ON (14 MHz)
 * ADC Clock            = HSI14
 */
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
