#include "delay.h"

static uint32_t g_fac_us = 0;

/**
 * @brief     初始化延迟函数
 * @param     sysclk: 系统时钟频率(MHz)
 * @retval    无
 */
void delay_init(uint16_t sysclk)
{
    g_fac_us = sysclk;
}

/**
 * @brief     延时nus
 * @param     nus: 要延时的us数
 * @retval    无
 */
void delay_us(uint32_t nus)
{
    uint32_t ticks;
    uint32_t told, tnow, tcnt = 0;
    uint32_t reload = SysTick->LOAD;        /* LOAD的值 */
    ticks = nus * g_fac_us;                 /* 需要的节拍数 */
    told = SysTick->VAL;                    /* 刚进入时的计数器值 */
    while (1)
    {
        tnow = SysTick->VAL;
        if (tnow != told)
        {
            if (tnow < told)
            {
                tcnt += told - tnow;        /* 这里注意一下SYSTICK是一个递减的计数器就可以了 */
            }
            else
            {
                tcnt += reload - tnow + told;
            }
            told = tnow;
            if (tcnt >= ticks) 
            {
                break;                      /* 时间超过/等于要延迟的时间,则退出 */
            }
        }
    }
}

/**
 * @brief     延时nms
 * @param     nms: 要延时的ms数
 * @retval    无
 */
void delay_ms(uint32_t nms)
{
    // 防止溢出，如果延时较长，分段延时
    // 假设 g_fac_us 最大为 48 (48MHz)，uint32_t 最大 4294967295
    // max_us = 4294967295 / 48 ≈ 89,478,485 us ≈ 89s
    // 所以一般情况下直接调用 delay_us 即可，但为了稳健，可以循环调用
    
    while(nms > 50) // 每次延时 50ms，避免单次 delay_us 溢出风险（虽然在F0 48M下很难溢出）
    {
        delay_us(50 * 1000);
        nms -= 50;
    }
    if(nms > 0)
    {
        delay_us(nms * 1000);
    }
}

/**
 * @brief       HAL库内部函数用到的延时
 * @note        HAL库的延时默认用Systick，如果我们没有开Systick的中断会导致调用这个延时后无法退出
 * @param       Delay : 要延时的毫秒数
 * @retval      None
 */
void HAL_Delay(uint32_t Delay)
{
     delay_ms(Delay);
}
