#ifndef __ADC_H__
#define __ADC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "app_config.h"

extern ADC_HandleTypeDef g_adc_handle;
extern DMA_HandleTypeDef g_dma_adc_handle;
void adc_init(void);

#ifdef __cplusplus
}
#endif

#endif
