#include "button.h"

void button_init(){
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE); //enable the clock
    GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Speed = GPIO_Medium_Speed;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = BUTTON_K0_GPIO_PIN | BUTTON_K1_GPIO_PIN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(BUTTON_GPIO, &GPIO_InitStructure);
}

u8 button_pressed(BUTTON button){
	return !GPIO_ReadInputDataBit(BUTTON_GPIO, button);
}
