#ifndef __APP_SETKEY_H
#define __APP_SETKEY_H

#include "app_config.h"

/**
 * @brief SET键单击事件处理
 */
void set_key_clicked(void);

/**
 * @brief SET键连按快速调整事件处理
 */
void set_key_presse_repeat_report(void);

/**
 * @brief SET键按下事件
 * @note 记录按下时间
 */
void set_key_pressed(void);

/**
 * @brief SET键释放事件
 */
void set_key_released(void);

#endif
