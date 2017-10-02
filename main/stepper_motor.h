#ifndef STEPPER_MOTOR_H
#define STEPPER_MOTOR_H

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_tim.h"
#include "ticks.h"
#include <stdint.h>

// Dir pin: PC9
#define STEPPER_DIR_GPIO_PIN GPIO_Pin_9
#define STEPPER_DIR_GPIO GPIOC
#define STEPPER_DIR_GPIO_RCC RCC_AHB1Periph_GPIOC

// Step pin: Step pin: PA8
#define STEPPER_STEP_TIM			   TIM1
#define STEPPER_STEP_TIM_CHANNEL       TIM_Channel_1
#define STEPPER_STEP_AF_TIM 		   GPIO_AF_TIM1
#define STEPPER_STEP_TIM_RCC		   RCC_APB2Periph_TIM1
#define STEPPER_STEP_GPIO              GPIOA
#define STEPPER_STEP_GPIO_PIN          GPIO_Pin_8
#define STEPPER_STEP_GPIO_PINSOURCE    GPIO_PinSource8
#define STEPPER_STEP_GPIO_RCC	       RCC_AHB1Periph_GPIOA

typedef enum {
  STEPPER_CW, STEPPER_CCW
} STEPPER_DIRECTION;


void stepper_init(void);
void stepper_spin(STEPPER_DIRECTION direction, u8 speed);
void _stepper_step_init(void);
void _stepper_dir_init(void);

#endif
