#ifndef __APP_MODEKEY_H
#define __APP_MODEKEY_H

#include "app_config.h"

/**
 * @brief MODE键单击事件处理
 */
void mode_key_clicked(void);

/**
 * @brief MODE键长按事件处理
 */
void mode_key_long_pressed(void);

/**
 * @brief MODE键按下事件
 * @note 记录按下时间
 */
void mode_key_pressed(void);

/**
 * @brief MODE键释放事件
 * @note 判断是单击还是长按
 */
void mode_key_released(void);

#endif
