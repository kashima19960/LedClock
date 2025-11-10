#include "exti.h"
#include "app_config.h"
#include "stm32f0xx_it.h"

extern DMA_HandleTypeDef hdma_adc;
extern TIM_HandleTypeDef g_tim16_handle;
extern TIM_HandleTypeDef g_tim17_handle;

/* 全局回调函数指针 */
static key_interrupt_callback_t g_key_callback = NULL;

/**
 * @brief 注册按键中断回调函数
 * @param callback 回调函数指针
 */
void register_key_interrupt_callback(key_interrupt_callback_t callback)
{
    g_key_callback = callback;
}

void SysTick_Handler(void)
{
    HAL_IncTick();
}

void EXTI0_1_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(SEC_INT_PIN);
}

/**
 * @brief EXTI2_3中断处理函数
 * @note 处理MODE键和SET键的中断
 */
void EXTI2_3_IRQHandler(void)
{
    /* MODE键中断 */
    if (__HAL_GPIO_EXTI_GET_IT(MODE_KEY_PIN) != 0x00u)
    {
        __HAL_GPIO_EXTI_CLEAR_IT(MODE_KEY_PIN);
        
        if (g_key_callback != NULL)
        {
            g_key_callback(MODE_KEY_PIN);
        }
    }
    
    /* SET键中断 */
    if (__HAL_GPIO_EXTI_GET_IT(SET_KEY_PIN) != 0x00u)
    {
        __HAL_GPIO_EXTI_CLEAR_IT(SET_KEY_PIN);
        
        if (g_key_callback != NULL)
        {
            g_key_callback(SET_KEY_PIN);
        }
    }
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

/**
 * @brief GPIO EXTI回调函数
 * @param gpio_pin 触发中断的GPIO引脚
 * @note 用于处理SEC_INT_PIN的中断
 */
void HAL_GPIO_EXTI_Callback(uint16_t gpio_pin)
{
    if (gpio_pin == SEC_INT_PIN && g_key_callback != NULL)
    {
        g_key_callback(gpio_pin);
    }
}