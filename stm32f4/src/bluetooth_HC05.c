#include "bluetooth_HC05.h"

/*Triggered upon RX*/
void USART2_IRQHandler(void)
{
	if(USART_GetITStatus(USART2,USART_IT_RXNE)==SET)
	{
		if(USART_GetFlagStatus(USART2,USART_IT_RXNE)==SET)
		{
			rcvBuffer[rcvIndex]=USART_ReceiveData(USART2);

			//Buffer full or end of current message
			if(rcvIndex+1>=RCV_BUFFER_SIZE || rcvBuffer[rcvIndex]=='\n')
			{
				//Placing message in message buffer - setting flag
				rcvMessageFlag=1;
				uint32_t i;
				for(i=0;i<RCV_BUFFER_SIZE;i++)
				{
					if(i<=rcvIndex)
					{
						rcvMessage[i]=rcvBuffer[i];
					}
					else
					{
						rcvMessage[i]=0x00;
					}
				}

				//Clear receive buffer
				for(i=0;i<RCV_BUFFER_SIZE;i++)
				{
					rcvBuffer[i]=0x00;
				}
				rcvMessageSize=rcvIndex;
				rcvIndex=0;
			}
			else
			{
				rcvIndex++;
			}
		}
		USART_ClearITPendingBit(USART2,USART_IT_RXNE);
	}
}

void bluetooth_init(uint32_t baudrate)
{
	/*Initialize clocks*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);

	/*Initialize UART Peripheral*/
	USART_InitTypeDef UsartStruct;
	UsartStruct.USART_BaudRate=baudrate;
	UsartStruct.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	UsartStruct.USART_Mode=USART_Mode_Tx | USART_Mode_Rx;
	UsartStruct.USART_Parity=USART_Parity_No;
	UsartStruct.USART_StopBits=USART_StopBits_1;
	UsartStruct.USART_WordLength=USART_WordLength_8b;
	USART_Init(USART2,&UsartStruct);
	USART_Cmd(USART2,ENABLE);

	// Initialize TX and RX Pins
	// USART1 : PB6 for TX and PB7 for RX
	// USART2 : PD5 for TX and PD6 for RX --> currently use
	GPIO_InitTypeDef GPIOStruct;
	GPIOStruct.GPIO_Mode=GPIO_Mode_AF;
	GPIOStruct.GPIO_OType=GPIO_OType_PP;
	GPIOStruct.GPIO_Pin=GPIO_Pin_5 | GPIO_Pin_6;
	GPIOStruct.GPIO_PuPd=GPIO_PuPd_NOPULL;
	GPIOStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOD,&GPIOStruct);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_USART2);;

	/*Initialize NVIC to interrupt on RX*/
	NVIC_InitTypeDef NVICStruct;
	NVICStruct.NVIC_IRQChannel=USART2_IRQn;
	NVICStruct.NVIC_IRQChannelCmd=ENABLE;
	NVICStruct.NVIC_IRQChannelPreemptionPriority=1;
	NVICStruct.NVIC_IRQChannelSubPriority=1;
	NVIC_Init(&NVICStruct);
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
}



void bluetooth_send_string(char* items)
{
	/*This works because strings end with \0 (null)*/
	while(*items)
	{
		bluetooth_send_char(*items);
		items++;
	}
}

void bluetooth_send_char(char item)
{
	while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET){}
	USART_SendData(USART2, (uint16_t)item);
}

