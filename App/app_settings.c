#include "app_settings.h"
#include "app_state.h"
#include "sd3077.h"

void read_backup_settings()
{
    uint8_t data[3];
    read_backup_data(0, data, 3);

    alarm_hour = data[0];
    alarm_min = data[1];
    is_alarm_enabled = data[2];

    // 检查闹铃设置是否合规
    if (alarm_hour > 23)
    {
        alarm_hour = 0;
    }
    if (alarm_min > 59)
    {
        alarm_min = 0;
    }
}

void save_settings()
{
    uint8_t backup_data[BAK_DATA_SIZE];
    backup_data[BAK_POWER_DOWN_IND_INDEX]       = POWER_DOWN_IND_DATA;
    backup_data[BAK_POWER_DOWN_IND_INDEX + 1]   = POWER_DOWN_IND_DATA;
    backup_data[BAK_ALARM_ENABLED_INDEX]        = is_alarm_enabled;
    backup_data[BAK_ALARM_HOUR_INDEX]           = alarm_hour;
    backup_data[BAK_ALARM_MINUTE_INDEX]         = alarm_min;
    backup_data[BAK_TEMP_SHOW_TIME_INDEX]       = temperature_show_time;
    backup_data[BAK_TEMP_HIDE_TIME_INDEX]       = temperature_hide_time;
    backup_data[BAK_ROT_ENABLED_INDEX]          = is_ring_on_time_enabled;
    backup_data[BAK_ROT_START_INDEX]            = ring_on_time_start;
    backup_data[BAK_ROT_STOP_INDEX]             = ring_on_time_stop;
    backup_data[BAK_BRIGHTNESS_INDEX]           = save_brightness;
    backup_data[BAK_BRIGHTNESS_STRONG_INDEX]    = strong_brightness;
    backup_data[BAK_BRIGHTNESS_WEAK_INDEX]      = weak_brightness;
    write_backup_data(BAK_POWER_DOWN_IND_INDEX, backup_data, BAK_DATA_SIZE);
}

void reset_settings()
{
    is_alarm_enabled        = false;
    alarm_hour             = 0;
    alarm_min              = 0;
    temperature_show_time    = 2;
    temperature_hide_time    = 10;
    is_ring_on_time_enabled   = false;
    ring_on_time_start       = 8;
    ring_on_time_stop        = 20;
    save_brightness       = 8;
    strong_brightness      = 8;
    weak_brightness       = 1;
}
