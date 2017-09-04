#ifndef _ENCODER_H_
#define _ENCODER_H_

// include ic library
#include "stm32f4xx.h"
#include "stm32f4xx_tim.h"
#include "ticks.h"
#include "tft_display.h"

typedef struct {
	TIM_TypeDef* timer;
	u32 clock_source;
	u32 GPIO_clock_source;
	u16 port1;
	u16 port2;
	GPIO_TypeDef* GPIOx;
} Encoder_Typedef[];


//Encoder 1: PC6 TIM8_CH1
//           PC7 TIM8_CH2
//           PORT 1 IS A PORT 2 IS B
//Encoder 2: PB6 TIM4_CH1
//           PB7 TIM4_CH2
// define
#define ENCODER_NO												2
// Encoder 1 details
#define ENCODER_TIMER1										TIM8
#define ENCODER_TIMER1_AF                       GPIO_AF_TIM8
#define ENCODER_TIMER1_CLOCK_SOURCE				RCC_APB2Periph_TIM8						// in APB 1
#define ENCODER_TIMER1_GPIO_CLOCK_SOURCE	    RCC_AHB1Periph_GPIOC					// in APB 2
#define ENCODER_TIMER1_PORT1							GPIO_Pin_6
#define ENCODER_TIMER1_PORT2							GPIO_Pin_7
#define ENCODER_TIMER1_GPIOx							GPIOC
#define ENCODER_TIMER1_GPIO_PINSOURCE1          GPIO_PinSource6
#define ENCODER_TIMER1_GPIO_PINSOURCE2          GPIO_PinSource7

// Encoder 2 details
#define ENCODER_TIMER2							TIM4
#define ENCODER_TIMER2_AF                       GPIO_AF_TIM4
#define ENCODER_TIMER2_CLOCK_SOURCE				RCC_APB1Periph_TIM4						// in APB 1
#define ENCODER_TIMER2_GPIO_CLOCK_SOURCE	    RCC_AHB1Periph_GPIOB					// in APB 2
#define ENCODER_TIMER2_PORT1							GPIO_Pin_6
#define ENCODER_TIMER2_PORT2							GPIO_Pin_7
#define ENCODER_TIMER2_GPIOx							GPIOB
#define ENCODER_TIMER2_GPIO_PINSOURCE1          GPIO_PinSource6
#define ENCODER_TIMER2_GPIO_PINSOURCE2          GPIO_PinSource7

#define ENCODER_MAX_CHANGE								20000

typedef enum {
	ENCODER1 = 0,
	ENCODER2 = 1
} ENCODER;

// function declaration
void encoder_init(void);
long get_minimize_count(ENCODER ENCODERx);
u32 get_full_count(ENCODER ENCODERx);
u32 get_count(ENCODER ENCODERx);
float determine_velocity(ENCODER ENCODERx);
void reset_all_encoder(void);
void reset_encoder_1(void);
void reset_encoder_2(void);

#endif
