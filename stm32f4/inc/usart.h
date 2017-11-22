#ifndef __USART_H
#define __USART_H

#include "stm32f4xx_usart.h"

#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

typedef enum 
{
	COM_NULL = -1,		//disabled
    COM1 = 0,   		//usart1
    COM3 = 1,				//usart2
} COM_TypeDef;

#define COMn 3 

// Definition for USART1 
#define COM1_CLK                    RCC_APB2Periph_USART1
#define COM1_TX_PIN                 GPIO_Pin_9
#define COM1_TX_GPIO_PORT           GPIOA
#define COM1_TX_GPIO_CLK            RCC_AHB1Periph_GPIOA
#define COM1_RX_PIN                 GPIO_Pin_10
#define COM1_RX_GPIO_PORT           GPIOA
#define COM1_RX_GPIO_CLK            RCC_AHB1Periph_GPIOA
#define COM1_IRQn                   USART1_IRQn


// Definition for USART2
#define COM3_CLK                    RCC_APB1Periph_USART3
#define COM3_TX_PIN                 GPIO_Pin_10
#define COM3_TX_GPIO_PORT           GPIOB
#define COM3_TX_GPIO_CLK            RCC_AHB1Periph_GPIOB
#define COM3_RX_PIN                 GPIO_Pin_11
#define COM3_RX_GPIO_PORT           GPIOB
#define COM3_RX_GPIO_CLK            RCC_AHB1Periph_GPIOB
#define COM3_IRQn                   USART3_IRQn

extern USART_TypeDef* COM_USART[COMn];
extern COM_TypeDef printf_COMx;

// COM1=USART1, COM3=UART3
void uart_init(COM_TypeDef COM, u32 br);
void uart_interrupt(COM_TypeDef COM);

void uart_printf_enable(COM_TypeDef COM);
void uart_printf_disable(void);

void uart_tx_byte(COM_TypeDef COM, const uint8_t data);
void uart_tx(COM_TypeDef COM, const uint8_t * tx_buf, ...);

uint8_t uart_rx_byte(COM_TypeDef COM);

typedef void on_receive_listener(const uint8_t byte);
void uart_interrupt_init(COM_TypeDef COM, on_receive_listener *listener);

#endif		/* __UART_H */
