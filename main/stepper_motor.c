#include "stepper_motor.h"

void stepper_init(void){
    _stepper_step_init();
    _stepper_dir_init();
}

void stepper_spin(STEPPER_DIRECTION direction, u8 speed){
    // Set dir pin
    switch(direction){
        case STEPPER_CW:
            GPIO_SetBits(STEPPER_DIR_GPIO, STEPPER_DIR_GPIO_PIN);
            break;
        case STEPPER_CCW:
            GPIO_ResetBits(STEPPER_DIR_GPIO, STEPPER_DIR_GPIO_PIN);
            break;
    }
    // Set step pwm
    TIM_SetCompare1(STEPPER_STEP_TIM, speed);
}

// Default PWM frequency for Step pin is: 2500 hz
void _stepper_step_init(void){
    GPIO_InitTypeDef STEP_GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	RCC_APB2PeriphClockCmd(STEPPER_STEP_TIM_RCC, ENABLE);
	RCC_AHB1PeriphClockCmd(STEPPER_STEP_GPIO_RCC, ENABLE);	// Enable bus

	STEP_GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; // Push-Pull Output Alternate-function
	STEP_GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;

	STEP_GPIO_InitStructure.GPIO_Pin = STEPPER_STEP_GPIO_PIN; 
	GPIO_Init(STEPPER_STEP_GPIO , &STEP_GPIO_InitStructure);	
	GPIO_PinAFConfig(STEPPER_STEP_GPIO, STEPPER_STEP_GPIO_PINSOURCE, GPIO_AF_TIM1);
	
	//-------------TimeBase Initialization-----------//
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // counter will count up (from 0 to FFFF)
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV2;		//timer clock = dead-time and sampling clock 	
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

	//------------------------------//
	TIM_TimeBaseStructure.TIM_Prescaler = 167;  //clk=168M/(167+1)=1 MHz, Freq = 1000000 / 400 = 2500Hz
	TIM_TimeBaseStructure.TIM_Period = 4000;     //pulse cycle= 400 
	//------------------------------//

	TIM_TimeBaseInit(STEPPER_STEP_TIM, &TIM_TimeBaseStructure);
	
	// ------------OC Init Configuration------------//
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;   		// set "high" to be effective output
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCPolarity_High;   		// set "high" to be effective output
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	           		// produce output when counter < CCR
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;		// Reset OC Idle state
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;	// Reset OC NIdle state
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;  	// this part enable the output
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputState_Disable; // this part disable the Nstate
	//------------------------------//
	TIM_OCInitStructure.TIM_Pulse = 250;    // this part sets the initial CCR value ,CCR = ExpPulseWidth * 1000
	//------------------------------//
	
	// OC Init
	TIM_OC1Init(STEPPER_STEP_TIM, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(STEPPER_STEP_TIM, ENABLE);
	
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
