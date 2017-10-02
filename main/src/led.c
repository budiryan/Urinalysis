#include "led.h"


void led_init(){
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); //enable the clock
	
	/* Fill in the init Structure */
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Pin = LED_GPIO_PIN_1 | LED_GPIO_PIN_2;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(LED_GPIO, &GPIO_InitStructure);
	LED_OFF(LED_1 | LED_2);
}


 
void LED_OFF(LED led){
	GPIO_SetBits(LED_GPIO, led);
}

void LED_ON(LED led){
	GPIO_ResetBits(LED_GPIO, led);
}

