#ifndef __APP_ALARM_H
#define __APP_ALARM_H

#include "app_config.h"

void check_ring_on_time(void);
void alarm_start(void);
void alarm_timer_tick(void);
void alarm_stop(void);
void check_alarm(void);

#endif
