#include "common.h"
#include "stm32f0xx_it.h"

extern DMA_HandleTypeDef hdma_adc;
extern TIM_HandleTypeDef htim16;
extern TIM_HandleTypeDef htim17;

void NMI_Handler(void)
{
}

void HardFault_Handler(void)
{
}

void SVC_Handler(void)
{
}

void PendSV_Handler(void)
{
}

void SysTick_Handler(void)
{
}

void EXTI0_1_IRQHandler(void)
{
}

void EXTI2_3_IRQHandler(void)
{
}

void DMA1_Channel1_IRQHandler(void)
{
}

void TIM16_IRQHandler(void)
{
}

void TIM17_IRQHandler(void)
{
}
