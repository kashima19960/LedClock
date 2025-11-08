#ifndef __ADC_H__
#define __ADC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

extern ADC_HandleTypeDef hadc;

void adc_init(void);

#ifdef __cplusplus
}
#endif

#endif
