#include "delay.h"
static uint32_t g_fac_us = 0;

/*
sysclk: 系统时钟频率，单位MHz
*/
void delay_init(uint16_t sysclk)
{
    g_fac_us = sysclk;
}

/*
微秒延时，抄正点原子的程序
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

            if (tnow < told)                /*还未溢出*/
            {
                tcnt += told - tnow;        /* 这里注意一下SYSTICK是一个递减的计数器就可以了 */
            }
            else                            /*溢出,这种情况tnow会比told大,要用reload相减得到增量*/
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
    /*
     * 防止溢出，如果延时较长，分段延时
     * 假设 g_fac_us 最大为 48 (48MHz)，uint32_t 最大 4294967295
     * max_us = 4294967295 / 48 ≈ 89,478,485 us ≈ 89s
     * 所以一般情况下直接调用 delay_us 即可，但为了稳健，可以循环调用
     */
    
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

/*
HAL_Delay必须是ms延时，不然会出错
*/
void HAL_Delay(uint32_t Delay)
{
     delay_ms(Delay);
}
