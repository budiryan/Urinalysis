#ifndef	_BUTTON_H
#define	_BUTTON_H

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include <stdbool.h>
#include "main.h"

#define BUTTON_GPIO GPIOD
#define BUTTON_0_GPIO_PIN GPIO_Pin_0
#define BUTTON_1_GPIO_PIN GPIO_Pin_1
#define BUTTON_2_GPIO_PIN GPIO_Pin_2
#define BUTTON_RCC RCC_AHB1Periph_GPIOD


typedef enum{
    BUTTON_0 = BUTTON_0_GPIO_PIN,
    BUTTON_1 = BUTTON_1_GPIO_PIN,
    BUTTON_2 = BUTTON_2_GPIO_PIN,
}BUTTON;

void button_init(void);
u8 button_pressed(BUTTON button);
void button_1_interrupt_config(void);
void button_2_interrupt_config(void);
void button_3_interrupt_config(void);

#endif
