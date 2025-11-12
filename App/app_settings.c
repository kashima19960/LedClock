#include "app_settings.h"
#include "app_state.h"
#include "sd3077.h"

void readBackupSettings()
{
    uint8_t data[3];
    read_backup_data(0, data, 3);

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
    uint8_t backupData[BAK_DATA_SIZE];
    backupData[BAK_POWER_DOWN_IND_INDEX] = POWER_DOWN_IND_DATA;
    backupData[BAK_POWER_DOWN_IND_INDEX + 1] = POWER_DOWN_IND_DATA;
    backupData[BAK_ALARM_ENABLED_INDEX] = isAlarmEnabled;
    backupData[BAK_ALARM_HOUR_INDEX] = alarmHour;
    backupData[BAK_ALARM_MINUTE_INDEX] = alarmMin;
    backupData[BAK_TEMP_SHOW_TIME_INDEX] = tempertureShowTime;
    backupData[BAK_TEMP_HIDE_TIME_INDEX] = tempertureHideTime;
    backupData[BAK_ROT_ENABLED_INDEX] = isRingOnTimeEnabled;
    backupData[BAK_ROT_START_INDEX] = ringOnTimeStart;
    backupData[BAK_ROT_STOP_INDEX] = ringOnTimeStop;
    backupData[BAK_BRIGHTNESS_INDEX] = savedBrightness;
    backupData[BAK_BRIGHTNESS_STRONG_INDEX] = strongBrightness;
    backupData[BAK_BRIGHTNESS_WEAK_INDEX] = weakBrightness;
    write_backup_data(BAK_POWER_DOWN_IND_INDEX, backupData, BAK_DATA_SIZE);
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
