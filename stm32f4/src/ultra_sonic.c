/** Ultrasonic library
*	@Author James Mok
*	
**/

#include "ultra_sonic.h"

static u32 rising_ticks1 = 0;
static u32 falling_ticks1 = 1;
static u32 result1 = 0;
static u16 result2 = 0;
struct US_TypeDef US_struct[US_COUNT] = { {TRIG_PIN_1, ECHO_PIN_1}, {TRIG_PIN_2, ECHO_PIN_2} };

void us_init()
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(US_RCC , ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;
	
	GPIO_InitTypeDef GPIO_InitStructure;
	for(u8 i=0; i<US_COUNT; i++)
	{
		GPIO_InitStructure.GPIO_Pin = US_struct[i].TRIG_Pin;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_Speed = GPIO_Fast_Speed;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_Init(US_PORT, &GPIO_InitStructure);
		
	
		GPIO_InitStructure.GPIO_Pin = US_struct[i].ECHO_Pin;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStructure.GPIO_Speed = GPIO_Fast_Speed;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
		GPIO_Init(US_PORT, &GPIO_InitStructure);
	}
	
												// TimeBase is for timer setting   > refer to P. 344 of library
	TIM_TimeBaseStructure.TIM_Prescaler = US_TIM_CLK / 1000000 - 1;     // 84M/1M - 1 = 83
	TIM_TimeBaseStructure.TIM_Period = US_RESET_TIME - 1;	
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInit(US_TIM, &TIM_TimeBaseStructure);      							 // this part feeds the parameter we set above
	
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;   		// set "high" to be effective output
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCPolarity_High;   		// set "high" to be effective output
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	           		// produce output when counter < CCR
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;		// Reset OC Idle state
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;	// Reset OC NIdle state
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;  	// this part enable the output
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputState_Disable; // this part disable the Nstate
	TIM_OCInitStructure.TIM_Pulse = US_TRIG_PULSE;									// this part sets the initial CCR value
	
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannel = US_IRQn;
	NVIC_Init(&NVIC_InitStructure);
	
	if(US_COUNT>=1)
	{
		GPIO_PinAFConfig(US_PORT, GPIO_PinSource6, GPIO_AF_TIM3);			//GPIO AF Config for using PWM (do it for every GPIO AF!)
		
		TIM_OC1Init(US_TIM, &TIM_OCInitStructure);
		TIM_OC1PreloadConfig(US_TIM, ENABLE);
		
		//EXTI setting
		SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource9);
		EXTI_InitStructure.EXTI_Line = EXTI_Line9;
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
		EXTI_Init(&EXTI_InitStructure);
		
		//Interrupt seting
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
		NVIC_Init(&NVIC_InitStructure);
		
		EXTI_ClearITPendingBit(EXTI_Line9);													// Clear Interrupt bits
	}
	if(US_COUNT==2)
	{
		GPIO_PinAFConfig(US_PORT, GPIO_PinSource7, GPIO_AF_TIM3);
		
		TIM_OC2Init(US_TIM, &TIM_OCInitStructure);
		TIM_OC2PreloadConfig(US_TIM, ENABLE);
		
		//EXTI setting
		SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource10);
		EXTI_InitStructure.EXTI_Line = EXTI_Line10;
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
		EXTI_Init(&EXTI_InitStructure);
		
		//Interrupt seting
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
		NVIC_Init(&NVIC_InitStructure);
	
		EXTI_ClearITPendingBit(EXTI_Line10);												// Clear Interrupt bits
	}	
	
	TIM_ARRPreloadConfig(US_TIM, ENABLE);													
	TIM_Cmd(US_TIM, ENABLE);	
	TIM_CtrlPWMOutputs(US_TIM, ENABLE);
	TIM_ITConfig(US_TIM, TIM_IT_Update, ENABLE);
	TIM_ITConfig(US_TIM, TIM_IT_CC1, ENABLE);
	
	TIM_ClearITPendingBit(US_TIM, TIM_IT_Update);									// Clear Interrupt bits
	TIM_ClearITPendingBit(US_TIM, TIM_IT_CC1);										// Clear Interrupt bits
}


/**	@brief		Function for getting the raw data (pulse width in us)
*		@param 		The us_device id(0,1)
*		@retval		The u32 raw data  
**/
u16 get_result(u8 device)
{
	return US_struct[device].result;
}

/**	@brief		Function for getting the distance
*		@param 		The us_device id(0,1)
*		@retval		The u16 distance  
**/
u16 get_distance(u8 device)
{
	u16 temp;
	temp = US_struct[device].result * 166/1000;
	
	if(temp < 30)
		return 0;
    else if(temp > 2500) 
        return 2500;
	else
		return temp;	
}

void EXTI9_5_IRQHandler()
{
	if(EXTI_GetITStatus(EXTI_Line9)!=RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line9);
		if(GPIO_ReadInputDataBit(US_PORT, ECHO_PIN_1))
		{
			US_struct[0].rising_ticks = TIM_GetCounter(US_TIM);
		}
		else
		{
			US_struct[0].falling_ticks = TIM_GetCounter(US_TIM);
			if(US_struct[0].falling_ticks > US_struct[0].rising_ticks)
				US_struct[0].result = US_struct[0].falling_ticks - US_struct[0].rising_ticks;
			else
				US_struct[0].result = 0;
		}
	}
}

void EXTI15_10_IRQHandler()
{
	if(EXTI_GetITStatus(EXTI_Line10)!=RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line10);
		if(GPIO_ReadInputDataBit(US_PORT, ECHO_PIN_2))
		{
			US_struct[1].rising_ticks = TIM_GetCounter(US_TIM);
		}
		else
		{
			US_struct[1].falling_ticks = TIM_GetCounter(US_TIM);
			if(US_struct[1].falling_ticks > US_struct[1].rising_ticks)
				US_struct[1].result = US_struct[1].falling_ticks - US_struct[1].rising_ticks;
			else
				US_struct[1].result = 0;
		}
	}
}

US_IRQHandler
{
  if (TIM_GetITStatus(US_TIM, TIM_IT_Update) != RESET) 
	{   
		// RESET
		TIM_ClearITPendingBit(US_TIM, TIM_IT_Update);
	}

	if(TIM_GetITStatus(US_TIM, TIM_IT_CC1) != RESET)
	{
		TIM_ClearITPendingBit(US_TIM, TIM_IT_CC1);
	}
}