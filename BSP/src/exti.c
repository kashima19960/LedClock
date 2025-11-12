#include "exti.h"

#include "app_config.h" 

#include "stm32f0xx_it.h" 
extern DMA_HandleTypeDef hdma_adc;


/* 全局回调函数指针 */

static exti_interrupt_callback_t g_interrupt_callback = NULL;

/**
 * @brief 注册中断回调函数
 * @param callback 回调函数指针
 */

void register_exti_interrupt_callback(exti_interrupt_callback_t callback)
{

    g_interrupt_callback = callback;
}

void SysTick_Handler(void)
{
    HAL_IncTick();
}

/**
 * @brief EXTI0_1中断处理函数
 * @note 处理SEC_INT秒中断
 */
void EXTI0_1_IRQHandler(void)
{
    /* SEC_INT秒中断 */ 
    if (__HAL_GPIO_EXTI_GET_IT(SEC_INT_PIN) != 0x00u)
    {
        __HAL_GPIO_EXTI_CLEAR_IT(SEC_INT_PIN);
        if (g_interrupt_callback != NULL)
        {

            g_interrupt_callback(SEC_INT_PIN);
        }
    }
}

/*
 * @brief EXTI2_3中断处理函数 
 * @note 处理MODE键和SET键的中断 
 */

void EXTI2_3_IRQHandler(void) 
{
    /* MODE键中断 */
    if (__HAL_GPIO_EXTI_GET_IT(MODE_KEY_PIN) != 0x00u)
    {
        __HAL_GPIO_EXTI_CLEAR_IT(MODE_KEY_PIN);
        if (g_interrupt_callback != NULL)
        {
            g_interrupt_callback(MODE_KEY_PIN);
        }
    }
    /* SET键中断 */
    if (__HAL_GPIO_EXTI_GET_IT(SET_KEY_PIN) != 0x00u)
    {
        __HAL_GPIO_EXTI_CLEAR_IT(SET_KEY_PIN);
        if (g_interrupt_callback != NULL)
        {
            g_interrupt_callback(SET_KEY_PIN);
        }
    }
}


