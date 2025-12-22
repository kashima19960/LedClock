#include "exti.h"

#include "bsp_config.h" 


static exti_interrupt_callback_t g_interrupt_callback = NULL;

/*
注册外部中断
*/
void register_exti_interrupt_callback(exti_interrupt_callback_t callback)
{
    g_interrupt_callback = callback;
}

void SysTick_Handler(void)
{ 
    HAL_IncTick();
}

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


