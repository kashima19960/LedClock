#ifndef __TIM_H__
#define __TIM_H__
#include "common.h"

extern TIM_HandleTypeDef g_tim3_handle;
extern TIM_HandleTypeDef g_tim16_handle;
extern TIM_HandleTypeDef g_tim17_handle;

void tim3_init(void);
void tim16_init(void);
void tim17_init(void);

#endif
