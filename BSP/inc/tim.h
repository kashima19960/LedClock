#ifndef __TIM_H
#define __TIM_H
#include "app_config.h"

extern TIM_HandleTypeDef g_tim3_handle;
extern TIM_HandleTypeDef g_tim16_handle;
extern TIM_HandleTypeDef g_tim17_handle;
typedef void (*timer_interrupt_callback_t)(TIM_HandleTypeDef *htim);
void tim3_init(void);
void tim16_init(void);
void tim17_init(void);

#endif
