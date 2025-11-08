#include "main.h"
#include "stm32f0xx_it.h"

extern DMA_HandleTypeDef hdma_adc;
extern TIM_HandleTypeDef g_tim16_handle;
extern TIM_HandleTypeDef g_tim17_handle;

void SysTick_Handler(void)
{
    HAL_IncTick();
}

void EXTI0_1_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(SEC_INT_Pin);
}

void EXTI2_3_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(MODE_KEY_Pin);
    HAL_GPIO_EXTI_IRQHandler(SET_KEY_Pin);
}

void DMA1_Channel1_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_adc);
}

void TIM16_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_tim16_handle);
}

void TIM17_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_tim17_handle);
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
}