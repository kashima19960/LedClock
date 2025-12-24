#include "function_test.h"
#include "adc.h"
#include "app_alarm.h"
#include "app_state.h"
#include "delay.h"
#include "sd3077.h"
#include "tm1637.h"
#include "buzzer.h"
#include "app_interrupt_handler.h"
extern void register_timer_interrupt_callback(timer_interrupt_callback_t callback);

/*util start*/
bool key_press_detect(bool *key_pressed, GPIO_TypeDef *gpio_port, uint16_t gpio_pin)
{
    GPIO_PinState key_state = HAL_GPIO_ReadPin(gpio_port, gpio_pin);

    // 检测按下
    if (*key_pressed == false && key_state == GPIO_PIN_RESET)
    {
        delay_ms(10); // 消抖
        if (HAL_GPIO_ReadPin(gpio_port, gpio_pin) == GPIO_PIN_RESET)
        {
            *key_pressed = true;
        }
    }
    // 检测释放
    else if (*key_pressed == true && key_state == GPIO_PIN_SET)
    {
        delay_ms(10); // 消抖
        if (HAL_GPIO_ReadPin(gpio_port, gpio_pin) == GPIO_PIN_SET)
        {
            *key_pressed = false;
            return true; // 完整按下一次
        }
    }

    return false;
}
#define buzzer_on()  HAL_GPIO_WritePin(BUZZER_GPIO_PORT, BUZZER_PIN, GPIO_PIN_RESET);
#define buzzer_off() HAL_GPIO_WritePin(BUZZER_GPIO_PORT, BUZZER_PIN, GPIO_PIN_SET);

static void display_text(const char *text)
{
    tm1637_set_char(0, text[0], 0);
    tm1637_set_char(1, text[1], 0);
    tm1637_set_char(2, text[2], 0);
    tm1637_set_char(3, text[3], 0);
}
static void beep_short_three(void)
{
    for (uint8_t i = 0; i < 3; i++)
    {
        buzzer_on();
        delay_ms(100);
        buzzer_off();
        delay_ms(100);
    }
}
static void beep_long_one(void)
{
    buzzer_on();
    delay_ms(600);
    buzzer_off();
}
static void display_pass(void)
{
    display_text("PASS");
    beep_short_three();
}

static void display_fail(void)
{
    display_text("FAIL");
    beep_long_one();
}
/*util end*/


/*tm1637 test start*/

/*
brief:测试是否能正确设置亮度
phenomenon:每按下一次 SET 键，亮度加 1，数码管会显示当前亮度值（0-7）
result:passed
*/
void tm1637_test_setbrightness(void)
{
    static uint8_t brightness = 0;
    static bool initialized = false;
    static bool set_key_pressed = false;

    if (!initialized)
    {
        tm1637_init();
        tm1637_set_brightness(brightness);
        tm1637_set_char(0, '0' + brightness, 0);
        tm1637_set_char(1, '0' + brightness, 0);
        tm1637_set_char(2, '0' + brightness, 0);
        tm1637_set_char(3, '0' + brightness, 0);
        initialized = true;
    }

    // SET 键：完整按下一次仅递增一次
    if (key_press_detect(&set_key_pressed, SET_KEY_GPIO_PORT, SET_KEY_PIN))
    {
        brightness = (brightness + 1) % 8;
        tm1637_set_brightness(brightness);
        tm1637_set_char(0, '0' + brightness, 1);
        tm1637_set_char(1, '0' + brightness, 1);
        tm1637_set_char(2, '0' + brightness, 1);
        tm1637_set_char(3, '0' + brightness, 1);
    }
}

/*
brief:测试是否能正确设置原始数据
phenomenon:每按下一次 SET 键，数码管显示 A、B、C、D 字符，并点亮小数点
result:passed
*/
void tm1637_test_set_raw_data(void)
{
    static bool initialized = false;
    static bool key_press_detected = false;
    if (!initialized)
    {
        tm1637_init();
        initialized = true;
    }
    if (key_press_detect(&key_press_detected, SET_KEY_GPIO_PORT, SET_KEY_PIN))
    {
        tm1637_set_raw_data(0, 0x77 | 0x80); // A
        tm1637_set_raw_data(1, 0x7C | 0x80); // B
        tm1637_set_raw_data(2, 0x39 | 0x80); // C
        tm1637_set_raw_data(3, 0x5E | 0x80); // D
        delay_ms(5000);
    }
    tm1637_set_raw_data(0, 0x77); // A
    tm1637_set_raw_data(1, 0x7C); // B
    tm1637_set_raw_data(2, 0x39); // C
    tm1637_set_raw_data(3, 0x5E); // D
}

/*
brief:测试是否能正确设置字符显示
phenomenon:数码管显示 A、B、C、D 字符，并点亮小数点
result:passed
*/
void tm1637_test_setchar(void)
{
    static bool initialized = false;

    if (!initialized)
    {
        tm1637_init();
        initialized = true;
    }
    tm1637_set_char(0, 'A', 1);
    tm1637_set_char(1, 'B', 1);
    tm1637_set_char(2, 'C', 1);
    tm1637_set_char(3, 'D', 1);
}

/*
brief:测试是否能正确显示数字
phenomenon:数码管显示数字 9527
result:passed
*/
void tm1637_test_show_number_right()
{
    tm1637_show_number_right(3, 9527, 0xFF, 1);
    delay_ms(1000);
}
/*tm1637 test end*/

/*sd3077 test start*/

/*
brief:测试是否能正确读取ID
result:
1,0x72 (Year),55,0x25,2025 年
2,0x73 (Month),8,0x08,8 月
3,0x74 (Day),1,0x01,1 日
4,0x75 (Machine),102,0x42,机台号: 0x42
5,0x76 (Order1),55,0x25,工单号1: 0x25
6,0x77 (Order2),56,0x26,工单号2: 0x26
7,0x78 (Serial1),7,0x07,序号高位: 07
8,0x79 (Serial2),9,0x09,序号低位: 09
*/
void sd3077_test_readid()
{
    static uint8_t id[8] = {0};
    static bool initialized = false;
    static bool set_key_pressed = false;
    static uint8_t i = 0;

    if (!initialized)
    {
        sd3077_iic_init();
        sd3077_sec_int_gpio_init();
        HAL_I2C_Mem_Read(&SD3077_IIC_HANDLE, SD3077_IIC_ADDR_READ, SD3077_REG_ID_START, 1, id, 8, HAL_MAX_DELAY);
        tm1637_show_number_right(3, bcd2bin(id[0]), 0xFF, 1);
        initialized = true;
    }

    if (key_press_detect(&set_key_pressed, SET_KEY_GPIO_PORT, SET_KEY_PIN))
    {
        i = (i + 1) % 8;
        tm1637_show_number_right(3, bcd2bin(id[i]), 0xFF, 1);
    }
}

/*
brief:测试是否能正确读取和设置时间
result:passed
*/
void sd3077_test_read_set_time(void)
{
    static bool initialized = false;
    static date_time dt = {.year = 25, // 2025年
                          .month = 12,
                          .day_of_month = 8, 
                          .day_of_week = 1, // 星期一
                          .hours = 12,
                          .minutes = 11,
                          .seconds = 50,
                          .ampm = HOUR24};

    if (!initialized)
    {
        sd3077_iic_init();
        sd3077_sec_int_gpio_init();
        initialized = true;
        set_time(&dt);
        delay_ms(500);
    }
    // 持续读取并显示当前时间
    time_now(&dt);
    tm1637_show_number_right(3, dt.minutes * 100 + dt.seconds, 0xFF, 1); // 显示分钟和秒钟
    delay_ms(50);
}

/*
brief:测试是否能正确读写备份RAM
result:passed
*/
void sd3077_test_backup_ram(void)
{
    static bool initialized = false;
    if (!initialized)
    {
        sd3077_iic_init();
        sd3077_sec_int_gpio_init();
        tm1637_init();
        initialized = true;
    }
    uint8_t write_data[4] = {0x12, 0x34, 0x56, 0x78};
    uint8_t read_data[4] = {0};

    // 写入数据
    write_backup_data(50, write_data, 4);
    delay_ms(300);

    // 读取数据
    read_backup_data(50, read_data, 4);
    // 验证数据是否一致
    bool pass = true;
    for (int i = 0; i < 4; i++)
    {
        if (write_data[i] != read_data[i])
        {
            pass = false;
            break;
        }
    }
    pass?display_pass():display_fail();
    delay_ms(3000);
}
/*sd3077 test end*/

/*light test start*/

/*
brief: 测试光敏电阻ADC采样值显示
result: 在TM1637上实时显示光敏电阻的ADC值(0-4095)
*/
void test_light_sensor_adc(void)
{
    static bool initialized = false;
    if (!initialized)
    {
        // 初始化必要的外设
        dma_init();
        adc_init();
        tim3_init();
        tm1637_init();
        
        // 启动ADC校准和DMA传输
        HAL_ADCEx_Calibration_Start(&g_adc_handle);
        HAL_ADC_Start_DMA(&g_adc_handle, adc_value, 2);
        HAL_TIM_Base_Start(&g_tim3_handle);
        
        initialized = true;
        delay_ms(100); // 等待ADC稳定
    }
    
    // 实时显示光敏电阻ADC值(通道0)
    tm1637_show_number_right(3, adc_value[0], 0xFF, 1);
    delay_ms(50); // 每200ms更新一次显示
}
/*light test end*/

/*
brief:自动亮度阈值与滞回测试（方法B：注入极端值）
 * 要求：save_brightness = 0（自动模式），TIM16 中断有效。
 * 提示：建议将 FUNCTION_TEST_NO_INTERRUPTS 设为 1，以便 main 初始化
 * tim16 并注册回调；此函数内也调用 tim16_init 与回调注册以增强稳健性。
 * phenomenon:数码管每2秒在高光（4095）和低光（0）之间切换，观察自动亮度切换情况
result:
 */
void test_auto_brightness_inject(void)
{
    static bool initialized         = false;
    static bool injection_high      = false;
    static bool pass_shown          = false;
    static bool seen_strong         = false;
    static bool seen_weak           = false;
    static uint32_t last_toggle_ts  = 0;

    if (!initialized)
    {
        tm1637_init();

        /* 自动亮度模式参数设置 */
        save_brightness   = 0;
        strong_brightness = STRONG_BRIGHTNESS_VALUE;
        weak_brightness   = WEAK_BRIGHTNESS_VALUE;
        is_weak_brightness = true;

        /* 确保 TIM16 运行且回调已注册 */
        tim16_init();
        register_timer_interrupt_callback(tim_interrupt_handler);
        HAL_TIM_Base_Start_IT(&g_tim16_handle);

        /* 初始注入为弱光（低端） */
        adc_value[0] = 0;
        tm1637_set_char(0, 'L', 0);
        tm1637_set_char(1, 'O', 0);
        tm1637_set_char(2, ' ', 0);
        tm1637_set_char(3, ' ', 0);

        last_toggle_ts = HAL_GetTick();
        injection_high = false;
        initialized    = true;
    }

    /* 每 2 秒在极端值之间切换 */
    uint32_t now = HAL_GetTick();
    if (now < last_toggle_ts || (now - last_toggle_ts >= 2000))
    {
        injection_high = !injection_high;
        last_toggle_ts = now;

        if (injection_high)
        {
            adc_value[0] = 4095;
            tm1637_set_char(0, 'H', 0);
            tm1637_set_char(1, 'I', 0);
            tm1637_set_char(2, ' ', 0);
            tm1637_set_char(3, ' ', 0);
        }
        else
        {
            adc_value[0] = 0;
            tm1637_set_char(0, 'L', 0);
            tm1637_set_char(1, 'O', 0);
            tm1637_set_char(2, ' ', 0);
            tm1637_set_char(3, ' ', 0);
        }
    }

    /* 观察自动亮度切换：is_weak_brightness 取值变化 */
    if (!seen_strong && !is_weak_brightness)
    {
        seen_strong = true;
    }

    if (!seen_weak && is_weak_brightness)
    {
        seen_weak = true;
    }

    if (!pass_shown && seen_strong && seen_weak)
    {
        display_pass();
        pass_shown = true;
    }
}

/*
 * 闹钟测试：设置时间距离触发点 10 秒。
 * 要求：TIM17 中断有效，秒中断使能（1Hz）。
 */
void test_alarm_trigger_in_10s(void)
{
    static bool initialized = false;
    static uint32_t start_ts = 0;

    if (!initialized)
    {
        sd3077_iic_init();
        sd3077_sec_int_gpio_init();
        tm1637_init();

        tim17_init();
        register_timer_interrupt_callback(tim_interrupt_handler);
        enable_second_interrupt_output();

        /* 设定闹钟在下一分钟触发，当前时间设到 50 秒 */
        date_time dt;
        time_now(&dt);

        uint8_t target_min  = (dt.minutes + 1) % 60;
        uint8_t target_hour = dt.hours;
        if (dt.minutes == 59)
        {
            target_hour = (dt.hours + 1) % 24;
        }

        is_alarm_enabled = true;
        is_alarmed       = false;
        alarm_hour       = target_hour;
        alarm_min        = target_min;

        dt.seconds = 50;
        set_time(&dt);

        start_ts   = HAL_GetTick();
        initialized = true;
    }

    /* 显示 10 秒倒计时 */
    uint32_t now      = HAL_GetTick();
    uint32_t elapsed  = now - start_ts;
    uint8_t  remain_s = (elapsed >= 10000) ? 0 : (10 - (elapsed / 1000));
    tm1637_show_number_right(3, remain_s, 0xFF, 1);
    delay_ms(50);

    if (is_alarming)
    {
        display_pass();
        /* 演示通过后可选择停止响铃 */
        alarm_stop();
    }
}

/*
 * 整点报时测试：设置时间距离整点 10 秒。
 * 要求：秒中断使能（1Hz）。
 */
void test_hourly_chime_in_10s(void)
{
    static bool initialized = false;
    static uint32_t start_ts = 0;

    if (!initialized)
    {
        sd3077_iic_init();
        sd3077_sec_int_gpio_init();
        tm1637_init();
        enable_second_interrupt_output();

        /* 报时全天开启，避免闹钟互斥干扰 */
        is_alarm_enabled        = false;
        is_ring_on_time_enabled = true;
        ring_on_time_start      = 0;
        ring_on_time_stop       = 23;

        date_time dt;
        time_now(&dt);
        last_ring_on_time_hour = dt.hours;

        dt.minutes = 59;
        dt.seconds = 50;
        set_time(&dt);

        start_ts   = HAL_GetTick();
        initialized = true;
    }

    /* 显示 10 秒倒计时 */
    uint32_t now      = HAL_GetTick();
    uint32_t elapsed  = now - start_ts;
    uint8_t  remain_s = (elapsed >= 10000) ? 0 : (10 - (elapsed / 1000));
    tm1637_show_number_right(3, remain_s, 0xFF, 1);
    delay_ms(50);

    /* 检测蜂鸣器低电平（响铃） */
    if (HAL_GPIO_ReadPin(BUZZER_GPIO_PORT, BUZZER_PIN) == GPIO_PIN_RESET)
    {
        display_pass();
    }
}

void test_run_entry(void)
{
    while (1)
    {
#if TEST_TM1637_BRIGHTNESS
        tm1637_test_setbrightness();
#elif TEST_TM1637_CHAR
        tm1637_test_setchar();
#elif TEST_TM1637_RAW
        tm1637_test_set_raw_data();
#elif TEST_TM1637_SHOW_NUMBER
        tm1637_test_show_number_right();
#elif TEST_SD3077_ID
        sd3077_test_readid();
#elif TEST_SD3077_TIME
        sd3077_test_read_set_time();
#elif TEST_SD3077_BACKUP
        sd3077_test_backup_ram();
#elif TEST_LIGHT_ADC
        test_light_sensor_adc();
#elif TEST_AUTO_BRIGHTNESS_INJECT
        test_auto_brightness_inject();
#elif TEST_ALARM_10S
        test_alarm_trigger_in_10s();
#elif TEST_CHIME_10S
        test_hourly_chime_in_10s();
#else
        /* 未选择测试项时，显示 NONE 提示 */
        static bool shown = false;
        if (!shown)
        {
            tm1637_init();
            tm1637_set_char(0, 'N', 0);
            tm1637_set_char(1, 'O', 0);
            tm1637_set_char(2, 'N', 0);
            tm1637_set_char(3, 'E', 0);
            shown = true;
        }
        delay_ms(200);
#endif
    }
}
