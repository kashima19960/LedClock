#ifndef __ADC_H
#define __ADC_H
#include "app_config.h"
extern ADC_HandleTypeDef g_adc_handle;
extern DMA_HandleTypeDef g_dma_adc_handle;
void adc_init(void);
#endif
