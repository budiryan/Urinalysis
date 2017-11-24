#ifndef _DELAY_H
#define _DELAY_H

#include "delay.h"
#include "stm32f4xx.h"

// For store tick counts in us
static __IO uint32_t usTicks;
void delay_init(void);

void delay_us(uint32_t us);

void delay_ms(uint32_t ms);


#endif
