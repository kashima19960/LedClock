#ifndef __APP_STATE_H
#define __APP_STATE_H

#include "app_config.h"
#include "sd3077.h"
#include "stdbool.h"

extern const uint16_t temperature_map[TEMP_MAP_SIZE];

extern display_mode current_mode;
extern bool is_init_completed;
extern uint32_t last_display_change_time;

extern uint32_t adc_value[2];

extern uint8_t save_brightness;
extern bool is_weak_brightness;
extern uint8_t strong_brightness, weak_brightness;

extern date_time time, last_time;
extern uint8_t blink_control;

extern bool is_alarm_enabled;
extern bool is_alarmed, is_alarming;
extern uint8_t alarm_hour, alarm_min;

extern bool is_ring_on_time_enabled;
extern uint8_t ring_on_time_start, ring_on_time_stop;
extern uint8_t last_ring_on_time_hour;
extern uint32_t ring_start_time;

extern uint32_t last_mode_key_press_time, last_set_key_press_time, last_set_key_press_report_time;
extern uint32_t alarm_timestamp, alarm_beep_count;
extern bool set_key_repeat_reported;

extern uint8_t temperature;
extern uint8_t temperature_show_time, temperature_hide_time;
extern uint8_t temp_buffered;
extern uint16_t temp_buffer[TEMP_BUFFER_SIZE + 1];

#endif
