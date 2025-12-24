#ifndef __FACTORY_TEST_H
#define __FACTORY_TEST_H

#include "app_config.h"
#include "key.h"
#include "tm1637.h"
#include <stdbool.h>
#include "tim.h"
/*
 * 测试用例选择（条件编译）。
 * 将某一项设置为 1 以启用该测试，其余保持为 0。
 */
#define TEST_TM1637_BRIGHTNESS        1
#define TEST_TM1637_CHAR              0
#define TEST_TM1637_RAW               0
#define TEST_TM1637_SHOW_NUMBER       0
#define TEST_SD3077_ID                0
#define TEST_SD3077_TIME              0
#define TEST_SD3077_BACKUP            0
#define TEST_LIGHT_ADC                0
#define TEST_AUTO_BRIGHTNESS_INJECT   0  /* 自动亮度阈值注入（极端值） */
#define TEST_ALARM_10S                0  /* 闹钟 10 秒倒计时触发 */
#define TEST_CHIME_10S                0  /* 整点报时 10 秒倒计时触发 */

/* 编译期防呆：仅允许选择 1 个测试项 */
#define TEST_SELECTED_COUNT ( \
	TEST_TM1637_BRIGHTNESS + \
	TEST_TM1637_CHAR + \
	TEST_TM1637_RAW + \
	TEST_TM1637_SHOW_NUMBER + \
	TEST_SD3077_ID + \
	TEST_SD3077_TIME + \
	TEST_SD3077_BACKUP + \
	TEST_LIGHT_ADC + \
	TEST_AUTO_BRIGHTNESS_INJECT + \
	TEST_ALARM_10S + \
	TEST_CHIME_10S)

#if (TEST_SELECTED_COUNT > 1)
#error "Select exactly ONE TEST_* macro (set to 1)."
#endif

/* 测试入口 */
void test_run_entry(void);

#endif
