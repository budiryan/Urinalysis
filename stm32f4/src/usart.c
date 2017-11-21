#include "usart.h"

uint8_t rx_buffer[256] = { 0 };
uint8_t rx_full = 0;
on_receive_listener *uart1_rx_listener;
on_receive_listener *uart2_rx_listener;
uint8_t uart1_listener_empty = 1;
uint8_t uart2_listener_empty = 1;

#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

USART_TypeDef* COM_USART[COMn] = {USART1, USART2}; 
GPIO_TypeDef* COM_TX_PORT[COMn] = {COM1_TX_GPIO_PORT, COM2_TX_GPIO_PORT}; 
GPIO_TypeDef* COM_RX_PORT[COMn] = {COM1_RX_GPIO_PORT, COM2_RX_GPIO_PORT}; 
uc32 COM_USART_CLK[COMn] = {COM1_CLK, COM2_CLK};
uc32 COM_TX_PORT_CLK[COMn] = {COM1_TX_GPIO_CLK, COM2_TX_GPIO_CLK}; 
uc32 COM_RX_PORT_CLK[COMn] = {COM1_RX_GPIO_CLK, COM2_RX_GPIO_CLK};
uc16 COM_TX_PIN[COMn] = {COM1_TX_PIN, COM2_TX_PIN};
uc16 COM_RX_PIN[COMn] = {COM1_RX_PIN, COM2_RX_PIN};
uc16 COM_IRQ[COMn] = {USART1_IRQn, USART2_IRQn};

COM_TypeDef printf_COMx;

/**
  * @brief  Inintialization of USART
  * @param  COM: which USART to inialialize
  * @param  br: Baudrate used for USART
  * @retval None
  */
void uart_init(COM_TypeDef COM, u32 br)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_AHB1PeriphClockCmd(COM_TX_PORT_CLK[COM] | COM_RX_PORT_CLK[COM], ENABLE);
	if (COM == COM1)
	{
		RCC_APB2PeriphClockCmd(COM_USART_CLK[COM], ENABLE);
        GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);//Connect PA9 to USART1_Tx
        GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);//Connect PA10 to USART1_Rx
	}
	else
	{
		RCC_APB1PeriphClockCmd(COM_USART_CLK[COM], ENABLE);
        GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);//Connect PB10 to USART2_Tx
        GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);//Connect PB11 to USART2_Rx
	}

	/* Configure USART Tx & USART Rx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = COM_TX_PIN[COM]|COM_RX_PIN[COM];
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(COM_TX_PORT[COM] , &GPIO_InitStructure);

	/* USART configuration */
	USART_InitStructure.USART_BaudRate = br;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(COM_USART[COM], &USART_InitStructure);
	USART_Cmd(COM_USART[COM], ENABLE);
}

/**
  * @brief  Enable the interrupt of USART
  * @param  COM: which USART to enable interrupt
  * @retval None
  */
void uart_interrupt(COM_TypeDef COM)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	#ifdef VECT_TAB_RAM
	NVIC_SetVectorTable(NVIC_VectTab_RAM,0x0);
	#else
	NVIC_SetVectorTable(NVIC_VectTab_FLASH,0x0);
	#endif

	NVIC_InitStructure.NVIC_IRQChannel = COM_IRQ[COM];
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	/* Enables the USART receive interrupt */
	USART_ITConfig(COM_USART[COM],USART_IT_RXNE,ENABLE);
}

/**
  * @brief  Enable the function of sending data of Printf via USART
  * @param  COM: which USART to be used for Printf
  * @retval None
  */
void uart_printf_enable(COM_TypeDef COM)
{
	printf_COMx = COM;
}

/**
  * @brief  Disable the function of sending data of Printf via UART
  * @param  None
  * @retval None
  */
void uart_printf_disable(void)
{
	printf_COMx = COM_NULL;
}

/**
  * @brief  Sending one byte of data via USART
  * @param  COM: which USART to be used for sending data
  * @param  data: one byte data to be sent
  * @retval None
  */
void uart_tx_byte(COM_TypeDef COM, uc8 data)
{
	while (USART_GetFlagStatus(COM_USART[COM], USART_FLAG_TC) == RESET); 
	USART_SendData(COM_USART[COM], (uint16_t)data);
}

/**
  * @brief  Sending multiple bytes of data via USART
  * @param  COM: which USART to be used for sending data
  * @param  tx_buf: string to be sent
  * @retval None
  */
void uart_tx(COM_TypeDef COM, const uc8 * tx_buf, ...)
{
	va_list arglist;
	u8 buf[255], *fp;
	
	va_start(arglist, tx_buf);
	vsprintf((char*)buf, (const char*)tx_buf, arglist);
	va_end(arglist);
	
	fp = buf;
	while (*fp)
		uart_tx_byte(COM, (uint16_t)*fp++);
}
/**
  * @brief  Receiving one byte of data via USART
  * @param  COM: which USART to be used for receiving data
  * @retval One byte of data received
  */
u8 uart_rx_byte(COM_TypeDef COM)
{
	while (USART_GetFlagStatus(COM_USART[COM], USART_FLAG_TC) == RESET); 
	return (u8)USART_ReceiveData(COM_USART[COM]);
}

void uart_interrupt_init(COM_TypeDef COM, on_receive_listener *listener)
{
	if (COM == COM1)
	{
		uart1_rx_listener = listener;
		uart1_listener_empty = 0;
	}
	else if (COM == COM2)
	{
		uart2_rx_listener = listener;
		uart2_listener_empty = 0;
	}
	else
		assert(0);
	
	uart_interrupt(COM);
}
	
void USART1_IRQHandler(void)
{
	if(USART_GetITStatus(USART1,USART_IT_RXNE) != RESET)
	{ // check RX interrupt
		if (!uart1_listener_empty)
			(*uart1_rx_listener)(USART_ReceiveData(USART1));
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
}


/*
void USART2_IRQHandler(void)
{
	if(USART_GetITStatus(USART2,USART_IT_RXNE) != RESET)
	{ // check RX interrupt
		if (!uart2_listener_empty)
			(*uart2_rx_listener)(USART_ReceiveData(USART2));
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
	}
}
*/

/**
  * @brief  Binding of function Printf
  * @param  None
  * @retval None
  */
//PUTCHAR_PROTOTYPE
//{
//	if (printf_COMx == COM_NULL)
//		return ch;
//		
//	while (USART_GetFlagStatus(COM_USART[printf_COMx], USART_FLAG_TC) == RESET);
//	USART_SendData(COM_USART[printf_COMx], (uint8_t) ch);
//	return ch;
//}
