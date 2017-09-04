#ifndef _ADC_H
#define _ADC_H

#include "stm32f4xx_tim.h"

void adc_init(void);
u16 adc_get(u8 i);

#endif




