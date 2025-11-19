#include "adc.h"

ADC_HandleTypeDef g_adc_handle;
DMA_HandleTypeDef g_dma_adc_handle;
void adc_init(void)
{
    /*
     * ADC1 配置说明：
     * - 模拟输入：通道 0（LIGHT_PIN/PA0，光敏电阻）、通道 1（NTC_PIN/PA1，NTC 热敏电阻）
     * - 12 位分辨率，数据右对齐
     * - 扫描模式 + 连续转换，由 TIM3 TRGO 上升沿触发
     * - 使用 DMA1_Channel1 循环模式将转换结果搬运到内存
     */
    ADC_ChannelConfTypeDef sConfig = {0};

    g_adc_handle.Instance                      = ADC1;                             // 使用 ADC1 实例
    g_adc_handle.Init.ClockPrescaler           = ADC_CLOCK_ASYNC_DIV1;             // 异步时钟不分频
    g_adc_handle.Init.Resolution               = ADC_RESOLUTION_12B;               // 12 位分辨率
    g_adc_handle.Init.DataAlign                = ADC_DATAALIGN_RIGHT;              // 数据右对齐
    g_adc_handle.Init.ScanConvMode             = ADC_SCAN_DIRECTION_FORWARD;       // 扫描方向：通道号递增（0 → 1）
    g_adc_handle.Init.EOCSelection             = ADC_EOC_SINGLE_CONV;              // 每次转换结束产生 EOC
    g_adc_handle.Init.LowPowerAutoWait         = DISABLE;                          // 关闭自动等待低功耗模式
    g_adc_handle.Init.LowPowerAutoPowerOff     = DISABLE;                          // 关闭自动关电低功耗模式
    g_adc_handle.Init.ContinuousConvMode       = ENABLE;                           // 连续转换模式
    g_adc_handle.Init.DiscontinuousConvMode    = DISABLE;                          // 不使用不连续转换
    g_adc_handle.Init.ExternalTrigConv         = ADC_EXTERNALTRIGCONV_T3_TRGO;     // 外部触发源：TIM3 TRGO
    g_adc_handle.Init.ExternalTrigConvEdge     = ADC_EXTERNALTRIGCONVEDGE_RISING;  // 触发沿：上升沿
    g_adc_handle.Init.DMAContinuousRequests    = ENABLE;                           // 使能 DMA 连续请求
    g_adc_handle.Init.Overrun                  = ADC_OVR_DATA_PRESERVED;           // 过载时保留旧数据
    if (HAL_ADC_Init(&g_adc_handle) != HAL_OK)
    {
        Error_Handler();
    }

    sConfig.Channel = ADC_CHANNEL_0;                   // 通道 0：LIGHT_PIN（PA0，光敏电阻）
    sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;            // Rank 与通道号一致
    sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5; // 采样时间 239.5 个 ADC 周期
    if (HAL_ADC_ConfigChannel(&g_adc_handle, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }

    sConfig.Channel = ADC_CHANNEL_1;                    // 通道 1：NTC_PIN（PA1，NTC 热敏电阻）
    if (HAL_ADC_ConfigChannel(&g_adc_handle, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }
}

void HAL_ADC_MspInit(ADC_HandleTypeDef* adcHandle)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if(adcHandle->Instance==ADC1)
    {
        __HAL_RCC_ADC1_CLK_ENABLE();

        __HAL_RCC_GPIOA_CLK_ENABLE();
        GPIO_InitStruct.Pin  = LIGHT_PIN|NTC_PIN;      // PA0: LIGHT_PIN（光敏），PA1: NTC_PIN（温度）
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;       // 模拟输入模式
        GPIO_InitStruct.Pull = GPIO_NOPULL;            // 无上拉下拉
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        g_dma_adc_handle.Instance                 = DMA1_Channel1;             // ADC 使用 DMA1 通道 1
        g_dma_adc_handle.Init.Direction           = DMA_PERIPH_TO_MEMORY;      // 外设到内存
        g_dma_adc_handle.Init.PeriphInc           = DMA_PINC_DISABLE;          // 外设地址不自增
        g_dma_adc_handle.Init.MemInc              = DMA_MINC_ENABLE;           // 内存地址递增
        g_dma_adc_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;       // 外设数据宽度：32 位
        g_dma_adc_handle.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;       // 内存数据宽度：32 位
        g_dma_adc_handle.Init.Mode                = DMA_CIRCULAR;              // 循环模式
        g_dma_adc_handle.Init.Priority            = DMA_PRIORITY_LOW;          // 低优先级
        if (HAL_DMA_Init(&g_dma_adc_handle) != HAL_OK)
        {
            Error_Handler();
        }

        __HAL_LINKDMA(adcHandle,DMA_Handle,g_dma_adc_handle);                  // 将 DMA 句柄链接到 ADC
    }
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef* adcHandle)
{
    if(adcHandle->Instance==ADC1)
    {
        __HAL_RCC_ADC1_CLK_DISABLE();
        HAL_GPIO_DeInit(GPIOA, LIGHT_PIN|NTC_PIN);
        HAL_DMA_DeInit(adcHandle->DMA_Handle);
    }
}
