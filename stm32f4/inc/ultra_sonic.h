#ifndef __ULTRA_SONIC_H
#define __ULTRA_SONIC_H

#include "stm32f4xx_gpio.h"
#include "ticks.h"

#define US_TIM 	TIM3
#define US_RCC	RCC_APB1Periph_TIM3
#define US_IRQn	TIM3_IRQn
#define US_IRQHandler	void TIM3_IRQHandler()

#define US_TRIG_PULSE         	10

#define US_TIM_CLK							84000000
#define	US_RESET_TIME						30000 //update rate of sensor and the maximum detect range, update rate is inverse proprotion to update rate
#define	US_DEVICE_COUNT					8

#define US_PORT 					GPIOA
#define TRIG_PIN_1 				GPIO_Pin_6 //TX
#define ECHO_PIN_1 				GPIO_Pin_9 //RX
#define TRIG_PIN_2 				GPIO_Pin_7 //TX
#define ECHO_PIN_2 				GPIO_Pin_10 //RX

#define US_COUNT		2

struct US_TypeDef
{
	uint16_t TRIG_Pin;
	uint16_t ECHO_Pin;	
	u16 rising_ticks;
	u16 falling_ticks;
	u16 result;
};

void us_init(void);
u16 get_result(u8 device);
u16 get_distance(u8 device);

#endif
