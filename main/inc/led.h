#ifndef LED_H
#define LED_H

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include <stdint.h>

// #define LED_GPIO_PIN_1 GPIO_Pin_6 --> PA6 is not available anymore
#define LED_GPIO_PIN_2 GPIO_Pin_7
#define LED_GPIO GPIOA

typedef enum {
  LED_2 = LED_GPIO_PIN_2
} LED;

void led_init(void);
void LED_OFF(LED led);
void LED_ON(LED led);

#endif
