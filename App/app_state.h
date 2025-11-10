#ifndef __APP_STATE_H
#define __APP_STATE_H

#include "app_config.h"
#include "sd3077.h"
#include "stdbool.h"

extern const uint16_t tempertureMap[TEMP_MAP_SIZE];

extern DisplayMode currentMode;
extern bool isInitCompleted;
extern uint32_t lastDisplayChangeTime;

extern uint32_t adcValue[2];

extern uint8_t savedBrightness;
extern bool isWeakBrightness;
extern uint8_t strongBrightness, weakBrightness;

extern DateTime time, lastTime;
extern uint8_t blinkControl;

extern bool isAlarmEnabled;
extern bool isAlarmed, isAlarming;
extern uint8_t alarmHour, alarmMin;

extern bool isRingOnTimeEnabled;
extern uint8_t ringOnTimeStart, ringOnTimeStop;
extern uint8_t lastRingOnTimeHour;
extern uint32_t ringStartTime;

extern uint32_t lastModeKeyPressTime, lastSetKeyPressTime, lastSetKeyPressReportTime;
extern uint32_t alarmTimestamp, alarmBeepCount;
extern bool setKeyRepeatReported;

extern uint8_t temperture;
extern uint8_t tempertureShowTime, tempertureHideTime;
extern uint8_t tempBuffered;
extern uint16_t tempBuffer[TEMP_BUFFER_SIZE + 1];

#endif
