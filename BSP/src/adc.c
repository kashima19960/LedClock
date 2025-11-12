#include "adc.h"

ADC_HandleTypeDef g_adc_handle;
DMA_HandleTypeDef g_dma_adc_handle;

void adc_init(void)
{
    ADC_ChannelConfTypeDef sConfig = {0};

    g_adc_handle.Instance = ADC1;
    g_adc_handle.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
    g_adc_handle.Init.Resolution = ADC_RESOLUTION_12B;
    g_adc_handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    g_adc_handle.Init.ScanConvMode = ADC_SCAN_DIRECTION_FORWARD;
    g_adc_handle.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    g_adc_handle.Init.LowPowerAutoWait = DISABLE;
    g_adc_handle.Init.LowPowerAutoPowerOff = DISABLE;
    g_adc_handle.Init.ContinuousConvMode = ENABLE;
    g_adc_handle.Init.DiscontinuousConvMode = DISABLE;
    g_adc_handle.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T3_TRGO;
    g_adc_handle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
    g_adc_handle.Init.DMAContinuousRequests = ENABLE;
    g_adc_handle.Init.Overrun = ADC_OVR_DATA_PRESERVED;
    if (HAL_ADC_Init(&g_adc_handle) != HAL_OK)
    {
        Error_Handler();
    }

    sConfig.Channel = ADC_CHANNEL_0;
    sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
    sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
    if (HAL_ADC_ConfigChannel(&g_adc_handle, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }

    sConfig.Channel = ADC_CHANNEL_1;
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
        GPIO_InitStruct.Pin = LIGHT_PIN|NTC_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        g_dma_adc_handle.Instance = DMA1_Channel1;
        g_dma_adc_handle.Init.Direction = DMA_PERIPH_TO_MEMORY;
        g_dma_adc_handle.Init.PeriphInc = DMA_PINC_DISABLE;
        g_dma_adc_handle.Init.MemInc = DMA_MINC_ENABLE;
        g_dma_adc_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
        g_dma_adc_handle.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
        g_dma_adc_handle.Init.Mode = DMA_CIRCULAR;
        g_dma_adc_handle.Init.Priority = DMA_PRIORITY_LOW;
        if (HAL_DMA_Init(&g_dma_adc_handle) != HAL_OK)
        {
            Error_Handler();
        }

        __HAL_LINKDMA(adcHandle,DMA_Handle,g_dma_adc_handle);
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
