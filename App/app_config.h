#ifndef __APP_CONFIG_H
#define __APP_CONFIG_H
#include "bsp_config.h"
#define KEY_LONG_PRESS_EFFECT_TIME       800                  // 按键长按生效时间阈值(ms)
#define KEY_REPEAT_TIME_INTERVAL         250                  // 按键长按重复触发间隔(ms)
#define KEY_CLICK_EFFECT_TIME            50                   // 按键单击消抖时间(ms)
#define YEAR_MAX_SET                     38                   // 年份设置最大值(2000+38=2038)
#define YEAR_MIN_SET                     15                   // 年份设置最小值(2000+15=2015)
#define TEMPERTURE_MAX_SHOW_TIME         15                   // 温度轮换显示最长时间(秒)
#define TEMPERTURE_MAX_HIDE_TIME         30                   // 温度轮换隐藏最长时间(秒)
#define RING_ON_TIME_LONG                1000                 // 闹钟响铃持续时间(ms)
#define STRONG_BRIGHTNESS_ADC_VALUE      2800                 // 强光环境判定 ADC 阈值
#define WEAK_BRIGHTNESS_ADC_VALUE        2300                 // 弱光环境判定 ADC 阈值
#define STRONG_BRIGHTNESS_VALUE          8                    // 强光环境下显示亮度等级(0-8)
#define WEAK_BRIGHTNESS_VALUE            1                    // 弱光环境下显示亮度等级(0-8)
#define ALARM_CONTROL_TIMER_HANDLE       g_tim17_handle       // 闹钟控制定时器句柄
#define ALARM_CONTROL_TIMER              TIM17                // 闹钟控制定时器外设
#define LIGHT_CONTROL_TIMER_HANDLE       g_tim16_handle       // 亮度采样控制定时器句柄
#define LIGHT_CONTROL_TIMER              TIM16                // 亮度采样控制定时器外设
#define TEMPERTURE_ADC_HANDLE            g_adc_handle         // 温度采样 ADC 句柄
#define BAK_DATA_SIZE                    13                   // 备份寄存器使用数量
#define BAK_POWER_DOWN_IND_INDEX         0x00                 // 断电标识存储索引
#define BAK_ALARM_ENABLED_INDEX          0x02                 // 闹钟使能状态存储索引
#define BAK_ALARM_HOUR_INDEX             0x03                 // 闹钟小时设置存储索引
#define BAK_ALARM_MINUTE_INDEX           0x04                 // 闹钟分钟设置存储索引
#define BAK_TEMP_SHOW_TIME_INDEX         0x05                 // 温度显示时长设置存储索引
#define BAK_TEMP_HIDE_TIME_INDEX         0x06                 // 温度隐藏时长设置存储索引
#define BAK_ROT_ENABLED_INDEX            0x07                 // 温度轮换显示使能存储索引
#define BAK_ROT_START_INDEX              0x08                 // 温度轮换开始时间存储索引
#define BAK_ROT_STOP_INDEX               0x09                 // 温度轮换结束时间存储索引
#define BAK_BRIGHTNESS_INDEX             0x0A                 // 亮度模式设置存储索引
#define BAK_BRIGHTNESS_STRONG_INDEX      0x0B                 // 强光亮度值存储索引
#define BAK_BRIGHTNESS_WEAK_INDEX        0x0C                 // 弱光亮度值存储索引
#define POWER_DOWN_IND_DATA              0xFA                 // 断电标识魔术字节
typedef enum
{
    MODE_SHOW_TIME = 0,           // 显示模式: 显示时分
    MODE_SHOW_SECOND,             // 显示模式: 显示秒数
    MODE_SHOW_TEMPERTURE,         // 显示模式: 显示温度
    MODE_SET_HOUR,                // 设置模式: 调整小时
    MODE_SET_MINUTE,              // 设置模式: 调整分钟
    MODE_SET_ALARM_ENABLE,        // 设置模式: 闹钟开关
    MODE_SET_ALARM_HOUR,          // 设置模式: 闹钟小时
    MODE_SET_ALARM_MINUTE,        // 设置模式: 闹钟分钟
    MODE_SET_TEMP_SHOW,           // 设置模式: 温度显示时长
    MODE_SET_TEMP_HIDE,           // 设置模式: 温度隐藏时长
    MODE_SET_BRIGHTNESS,          // 设置模式: 亮度模式选择
    MODE_SET_BRIGHTNESS_STRONG,   // 设置模式: 强光亮度值
    MODE_SET_BRIGHTNESS_WEAK,     // 设置模式: 弱光亮度值
    MODE_SET_ROT_ENABLE,          // 设置模式: 温度轮换开关
    MODE_SET_ROT_START,           // 设置模式: 温度轮换开始时间
    MODE_SET_ROT_STOP,            // 设置模式: 温度轮换结束时间
} display_mode;

#endif
