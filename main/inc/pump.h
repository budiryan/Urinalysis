#ifndef PUMP_H
#define PUMP_H

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_tim.h"
#include "ticks.h"
#include <stdint.h>

// Dir pin: PC2 P14
#define DIR_GPIO_PIN GPIO_Pin_2
#define DIR_GPIO GPIOC
#define DIR_GPIO_RCC RCC_AHB1Periph_GPIOC

// Step pin: Step pin: PA8 P11
#define STEP_TIM			   TIM1
#define STEP_TIM_CHANNEL       TIM_Channel_1
#define STEP_AF_TIM 		   GPIO_AF_TIM1
#define STEP_TIM_RCC		   RCC_APB2Periph_TIM1
#define STEP_GPIO              GPIOA
#define STEP_GPIO_PIN          GPIO_Pin_8
#define STEP_GPIO_PINSOURCE    GPIO_PinSource8
#define STEP_GPIO_RCC	       RCC_AHB1Periph_GPIOA

typedef enum {
  CW, CCW
} PUMP_DIRECTION;


void pump_init(void);
void pump(u16 speed, PUMP_DIRECTION direction);
void _step_init(void);
void _dir_init(void);

#endif
