#ifndef PUMP_H
#define PUMP_H

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_tim.h"
#include "ticks.h"
#include <stdint.h>

// Dir pin: PA2 P5
#define DIR_GPIO_PIN GPIO_Pin_2
#define DIR_GPIO GPIOA
#define DIR_GPIO_RCC RCC_AHB1Periph_GPIOA

// Step pin: Step pin: PA3 P5 TIM2 Channel 4
#define STEP_TIM			   TIM2
#define STEP_TIM_CHANNEL       TIM_Channel_4
#define STEP_AF_TIM 		   GPIO_AF_TIM2
#define STEP_TIM_RCC		   RCC_APB1Periph_TIM2
#define STEP_GPIO              GPIOA
#define STEP_GPIO_PIN          GPIO_Pin_3
#define STEP_GPIO_PINSOURCE    GPIO_PinSource3
#define STEP_GPIO_RCC	       RCC_AHB1Periph_GPIOA

typedef enum {
  CW, CCW
} PUMP_DIRECTION;

void pump_init(void);
void pump(u16 speed, PUMP_DIRECTION direction);
void _step_init(void);
void _dir_init(void);

#endif
