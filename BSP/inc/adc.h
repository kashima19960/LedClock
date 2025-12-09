#ifndef __ADC_H
#define __ADC_H
#include "bsp_config.h"

extern ADC_HandleTypeDef g_adc_handle;
extern DMA_HandleTypeDef g_dma_adc_handle;
void dma_init(void);
void adc_init(void);
#endif
