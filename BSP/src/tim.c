#include "tim.h"

TIM_HandleTypeDef g_tim3_handle;
TIM_HandleTypeDef g_tim16_handle;
TIM_HandleTypeDef g_tim17_handle;
/*
定时器时钟：16MHz / (15+1) = 1MHz
计数周期：(9999+1) / 1MHz = 10000/1000000 = 10ms
中断频率：每10ms产生一次更新中断
*/
void tim3_init(void)
{
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    g_tim3_handle.Instance = TIM3;
    g_tim3_handle.Init.Prescaler = 15;
    g_tim3_handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    g_tim3_handle.Init.Period = 10000-1;
    g_tim3_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    g_tim3_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&g_tim3_handle) != HAL_OK)
    {
        Error_Handler();
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&g_tim3_handle, &sClockSourceConfig) != HAL_OK)
    {
        Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&g_tim3_handle, &sMasterConfig) != HAL_OK)
    {
        Error_Handler();
    }
}
/*
定时器时钟：16MHz / (1599+1) = 10kHz
计数周期：(9999+1) / 10kHz = 10000/10000 = 1s
中断频率：每1秒产生一次更新中断
*/
void tim16_init(void)
{
    g_tim16_handle.Instance = TIM16;
    g_tim16_handle.Init.Prescaler = 1599;
    g_tim16_handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    g_tim16_handle.Init.Period = 9999;
    g_tim16_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    g_tim16_handle.Init.RepetitionCounter = 0;
    g_tim16_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    if (HAL_TIM_Base_Init(&g_tim16_handle) != HAL_OK)
    {
        Error_Handler();
    }
}
/*
定时器时钟：16MHz / (15+1) = 1MHz
计数周期：(999+1) / 1MHz = 1000/1000000 = 1ms
中断频率：每1ms产生一次更新中断
*/
void tim17_init(void)
{
    g_tim17_handle.Instance = TIM17;
    g_tim17_handle.Init.Prescaler = 15;
    g_tim17_handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    g_tim17_handle.Init.Period = 999;
    g_tim17_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    g_tim17_handle.Init.RepetitionCounter = 0;
    g_tim17_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    if (HAL_TIM_Base_Init(&g_tim17_handle) != HAL_OK)
    {
        Error_Handler();
    }
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *tim_baseHandle)
{
    if (tim_baseHandle->Instance == TIM3)
    {
        __HAL_RCC_TIM3_CLK_ENABLE();
    }
    else if (tim_baseHandle->Instance == TIM16)
    {
        __HAL_RCC_TIM16_CLK_ENABLE();
        HAL_NVIC_SetPriority(TIM16_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(TIM16_IRQn);
    }
    else if (tim_baseHandle->Instance == TIM17)
    {
        __HAL_RCC_TIM17_CLK_ENABLE();
        HAL_NVIC_SetPriority(TIM17_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(TIM17_IRQn);
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
}
