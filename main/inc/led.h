#ifndef LED_H
#define LED_H

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include <stdint.h>

#define LED_GPIO_PIN GPIO_Pin_2
#define LED_GPIO GPIOC

void led_init(void);
void LED_OFF(void);
void LED_ON(void);

#endif
