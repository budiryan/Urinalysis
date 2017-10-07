#ifndef	_BUTTON_H
#define	_BUTTON_H

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include <stdbool.h>
#include "tft_display.h"
#include "main.h"

#define BUTTON_GPIO     GPIOE
#define BUTTON_K0_GPIO_PIN GPIO_Pin_4
#define BUTTON_K1_GPIO_PIN GPIO_Pin_3
#define BUTTON_RCC RCC_AHB1Periph_GPIOE


typedef enum{
    BUTTON_K0 = BUTTON_K0_GPIO_PIN,
    BUTTON_K1 = BUTTON_K1_GPIO_PIN
}BUTTON;

void button_init(void);
u8 button_pressed(BUTTON button);

#endif
