#include "led.h"


void led_init(){
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); //enable the clock
	
	/* Fill in the init Structure */
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = LED_GPIO_PIN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(LED_GPIO, &GPIO_InitStructure);
	LED_OFF();
}


 
void LED_OFF(){
	GPIO_ResetBits(LED_GPIO,LED_GPIO_PIN);
}

void LED_ON(){
	GPIO_SetBits(LED_GPIO,LED_GPIO_PIN);
}

