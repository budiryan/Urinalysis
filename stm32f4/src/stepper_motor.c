#include "stepper_motor.h"
TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
TIM_OCInitTypeDef  TIM_OCInitStructure;

void stepper_init(void){
    _stepper_step_init();
    _stepper_dir_init();
}

void stepper_spin(uint32_t speed , STEPPER_DIRECTION direction, u8 spinning){
    // Set dir pin
    switch(direction){
        case STEPPER_CW:
            GPIO_ResetBits(STEPPER_DIR_GPIO, STEPPER_DIR_GPIO_PIN);
            break;
        case STEPPER_CCW:
            GPIO_SetBits(STEPPER_DIR_GPIO, STEPPER_DIR_GPIO_PIN);
            break;
    }
    
    // Set spin / stop
    switch(spinning){
        case 1:
            TIM_SetCompare3(STEPPER_STEP_TIM, 1);
            break;
        case 0:
            TIM_SetCompare3(STEPPER_STEP_TIM, 0);
            break;
    }
    
    // Adjust the speed of the stepper motor
    TIM_TimeBaseStructure.TIM_Period = speed;
    TIM_TimeBaseInit(STEPPER_STEP_TIM, &TIM_TimeBaseStructure);
}


// Default PWM frequency for Step pin is: 2500 hz
void _stepper_step_init(void){
    GPIO_InitTypeDef STEP_GPIO_InitStructure;


	RCC_APB2PeriphClockCmd(STEPPER_STEP_TIM_RCC, ENABLE);
    // Enable bus
	RCC_AHB1PeriphClockCmd(STEPPER_STEP_GPIO_RCC, ENABLE);
    
    // Push-Pull Output Alternate-function
	STEP_GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 
	STEP_GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;

	STEP_GPIO_InitStructure.GPIO_Pin = STEPPER_STEP_GPIO_PIN; 
	GPIO_Init(STEPPER_STEP_GPIO , &STEP_GPIO_InitStructure);	
	GPIO_PinAFConfig(STEPPER_STEP_GPIO, STEPPER_STEP_GPIO_PINSOURCE, GPIO_AF_TIM1);
	
	//-------------TimeBase Initialization-----------//
    
    // counter will count up (from 0 to FFFF)
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    //timer clock = dead-time and sampling clock 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV2;	
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

    //clk=168M/(167+1)=1 MHz, Freq = 1000000 / 400 = 2500Hz
	TIM_TimeBaseStructure.TIM_Prescaler = 399;
    //pulse cycle= 400 
	TIM_TimeBaseStructure.TIM_Period = 100;

	TIM_TimeBaseInit(STEPPER_STEP_TIM, &TIM_TimeBaseStructure);
	
	// ------------OC Init Configuration------------//
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputState_Disable; 
	TIM_OCInitStructure.TIM_Pulse = 0;
	
	// OC Init
	TIM_OC3Init(STEPPER_STEP_TIM, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(STEPPER_STEP_TIM, ENABLE);
	
	TIM_ARRPreloadConfig(STEPPER_STEP_TIM, ENABLE);
	TIM_Cmd(STEPPER_STEP_TIM, ENABLE);	
	TIM_CtrlPWMOutputs(STEPPER_STEP_TIM, ENABLE);
}

void _stepper_dir_init(void){
    RCC_AHB1PeriphClockCmd(STEPPER_DIR_GPIO_RCC, ENABLE); //enable the clock
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = STEPPER_DIR_GPIO_PIN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(STEPPER_DIR_GPIO, &GPIO_InitStructure);
    GPIO_ResetBits(STEPPER_DIR_GPIO, STEPPER_DIR_GPIO_PIN);
}
