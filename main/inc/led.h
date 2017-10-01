#ifndef LED_H
#define LED_H

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include <stdint.h>

#define LED_GPIO_PIN_1 GPIO_Pin_6
#define LED_GPIO_PIN_2 GPIO_Pin_7
#define LED_GPIO GPIOA

typedef enum {
  LED_1,
  LED_2
} LED;

void led_init(void);
void LED_OFF(void);
void LED_ON(void);

#endif
