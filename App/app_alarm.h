#ifndef __APP_ALARM_H
#define __APP_ALARM_H

#include "app_config.h"

void checkRingOnTime(void);
void alarmStart(void);
void alarmTimerTick(void);
void alarmStop(void);
void checkAlarm(void);

#endif
