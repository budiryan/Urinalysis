#ifndef __TICKS_H
#define __TICKS_H

#include "stm32f4xx_tim.h"

#define TICKS_TIM							TIM4
#define TICKS_RCC							RCC_APB1Periph_TIM4
#define TICKS_CLKFreq					84000000
#define TICKS_IRQn						TIM4_IRQn
#define TICKS_IRQHandler			void TIM4_IRQHandler(void)

//extern volatile u16 ticks;
//extern volatile u16 seconds;

u16 get_ticks(void);
u16 get_seconds(void);
u32 get_full_ticks(void);
void ticks_init(void);

void _delay_us(u32 nus);
void _delay_ms(u16 nms);

void simple_delay1_ms(void);
void simple_delay10_us(void);

#endif		/*  __TICKS_H */
