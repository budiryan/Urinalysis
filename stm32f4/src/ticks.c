#include "ticks.h"

volatile u16 ticks = 0;
volatile u16 seconds = 0;

/**
  * @brief  Get the ticks passed from 0-999
  * @param  None
  * @retval ticks passed
  */
u16 get_ticks(void) {
	return ticks;
}

/**
  * @brief  Get the seconds passed from
  * @param  seconds
  * @retval ticks passed
  */
u16 get_seconds(void) {
	return seconds;
}

u32 get_full_ticks(void)
{
	return seconds * 1000 + ticks;
}

/**
  * @brief  Initialization of ticks timer
  * @param  None
  * @retval None
  */
void ticks_init(void) {
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;      									// TimeBase is for timer setting   > refer to P. 344 of library

	RCC_APB1PeriphClockCmd(TICKS_RCC , ENABLE);
	
//	TICKS_TIM->PSC = SystemCoreClock / 1000000 - 1;		// Prescaler
//	TICKS_TIM->ARR = 1000;
//	TICKS_TIM->EGR = 1;
//	TICKS_TIM->SR = 0;
//	TICKS_TIM->DIER = 1;
//	TICKS_TIM->CR1 = 1;
	
	TIM_TimeBaseStructure.TIM_Period = 1000;	                 				       // Timer period, 1000 ticks in one second
	TIM_TimeBaseStructure.TIM_Prescaler = TICKS_CLKFreq / 1000000 - 1;     // 84M/1M - 1 = 83
	TIM_TimeBaseInit(TICKS_TIM, &TIM_TimeBaseStructure);      							 // this part feeds the parameter we set above
	
	TIM_ClearITPendingBit(TICKS_TIM, TIM_IT_Update);												 // Clear Interrupt bits
	TIM_ITConfig(TICKS_TIM, TIM_IT_Update, ENABLE);													 // Enable TIM Interrupt
	TIM_Cmd(TICKS_TIM, ENABLE);																							 // Counter Enable

	
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannel = TICKS_IRQn;
	NVIC_Init(&NVIC_InitStructure);
	
	//SysTick_Config(SystemCoreClock/1000);
	ticks = seconds = 0;
}

/**
  * @brief  Timer for ticks
  * @param  None
  * @retval None
  */
TICKS_IRQHandler
{
  if (TIM_GetITStatus(TICKS_TIM, TIM_IT_Update) != RESET) {
    TIM_ClearFlag(TICKS_TIM, TIM_FLAG_Update);
    //TIM_ClearITPendingBit(TICKS_TIM, TIM_IT_Update);

    if (ticks >= 999) {
      ticks = 0;
      seconds++;
    } else {
      ticks++;
    }
  }
	
}

static __IO uint32_t TimingDelay;
u8 using_delay = 0 ;

/**
  * @brief  Generate a delay (in us)
  * @param  nus: us to be delayed
  * @retval None
  */
void _delay_us( u32 nus)
{
	u32 temp;
	if( using_delay == 0 ){
		using_delay = 1;
		SysTick->LOAD = 9*nus;
		SysTick->VAL = 0x00;
		SysTick->CTRL = 0x01;
		do
		{
			temp=SysTick->CTRL;
		}while((temp&0x01)&&(!(temp&(1<<16))));
		SysTick->CTRL = 0x00;
		SysTick->VAL = 0x00;
		using_delay = 0;
	}
	else{
		nus = nus / 10;
		while( nus -- ){
			simple_delay10_us();
		}
	}
}

/**
  * @brief  Generate a delay (in ms)
  * @param  nms: ms to be delayed
  * @retval None
  */
void _delay_ms( u16 nms )
{
	u32 temp;
	u16 ms ; 
	if( using_delay == 0 ){
		using_delay = 1;
		while( nms ){
		
		ms = ( nms > 1000 ) ? 1000 : nms;
		
		SysTick->LOAD = 9000*ms;
		SysTick->VAL = 0x00;
		SysTick->CTRL = 0x01;
		do
		{
			temp = SysTick->CTRL;
		}while((temp&0x01)&&(!(temp&(1<<16))));
		SysTick->CTRL=0x00;
		SysTick->VAL=0x00;
		
		nms -= ms;
		}
		using_delay = 0;
	}
	else{
		while( nms -- ){
			simple_delay1_ms();
		}
			
	}
}

/**
  * @brief  A single 10us delay written in assembly
  * @param  None
  * @retval None
  */
__asm void simple_delay10_us(){
	MOV		R0, #115
loop
    SUB     R0, R0, #1
    CMP     R0, #0
    BNE        loop
    BX     LR
}

/**
  * @brief  A single 1ms delay
  * @param  None
  * @retval None
  */
void simple_delay1_ms(){
	u8 i = 0 ; 
	for( i = 0 ; i < 100 ; i ++ )
		simple_delay10_us();
}

