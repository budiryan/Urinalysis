#include "button.h"

void button_init(){
    // Initialize K0 and K1
    RCC_AHB1PeriphClockCmd(BUTTON_RCC, ENABLE); //enable the clock
    GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Speed = GPIO_Medium_Speed;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = BUTTON_K0_GPIO_PIN | BUTTON_K1_GPIO_PIN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(BUTTON_GPIO, &GPIO_InitStructure);
    
    // Initialize KUP
    RCC_AHB1PeriphClockCmd(BUTTON_RCC_2, ENABLE); //enable the clock
	GPIO_InitStructure.GPIO_Speed = GPIO_Medium_Speed;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = BUTTON_KUP_GPIO_PIN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(BUTTON_GPIO_2, &GPIO_InitStructure);
    
}

u8 button_pressed(BUTTON button){
    switch(button){
        case BUTTON_K0:
        case BUTTON_K1:
            return !GPIO_ReadInputDataBit(BUTTON_GPIO, button);
            break;
        case BUTTON_KUP:
            return !GPIO_ReadInputDataBit(BUTTON_GPIO_2, button);
            break;
    }   
}
