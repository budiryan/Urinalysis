#ifndef _CONFIG_H
#define _CONFIG_H

#include <stdio.h>
#include "stm32f4xx.h"

/**
 * @brief Definition for COM port1, connected to USART1
 */ 
#define Open207Z_USART                        USART1
#define Open207Z_USART_CLK                    RCC_APB2Periph_USART1
#define Open207Z_USART_TX_PIN                 GPIO_Pin_9
#define Open207Z_USART_TX_GPIO_PORT           GPIOA
#define Open207Z_USART_TX_GPIO_CLK            RCC_AHB1Periph_GPIOA
#define Open207Z_USART_TX_SOURCE              GPIO_PinSource9
#define Open207Z_USART_TX_AF                  GPIO_AF_USART1
#define Open207Z_USART_RX_PIN                 GPIO_Pin_10
#define Open207Z_USART_RX_GPIO_PORT           GPIOA
#define Open207Z_USART_RX_GPIO_CLK            RCC_AHB1Periph_GPIOA
#define Open207Z_USART_RX_SOURCE              GPIO_PinSource10
#define Open207Z_USART_RX_AF                  GPIO_AF_USART1
#define Open207Z_USART_IRQn                   USART1_IRQn

/**
 * @brief Definition for COM port1, connected to I2C1
 */
 /* Configure I2C1 pins: PB6->SCL and PB7->SDA */ 
#define Open207Z_I2C                        	I2C1
#define Open207Z_I2C_CLK                    	RCC_APB1Periph_I2C1

#define Open207Z_I2C_SDA_PIN                 	GPIO_Pin_7
#define Open207Z_I2C_SDA_GPIO_PORT           	GPIOB
#define Open207Z_I2C_SDA_GPIO_CLK            	RCC_AHB1Periph_GPIOB
#define Open207Z_I2C_SDA_SOURCE              	GPIO_PinSource7
#define Open207Z_I2C_SDA_AF                  	GPIO_AF_I2C1

#define Open207Z_I2C_SCL_PIN                 	GPIO_Pin_6
#define Open207Z_I2C_SCL_GPIO_PORT           	GPIOB
#define Open207Z_I2C_SCL_GPIO_CLK            	RCC_AHB1Periph_GPIOB
#define Open207Z_I2C_SCL_SOURCE              	GPIO_PinSource6
#define Open207Z_I2C_SCL_AF                  	GPIO_AF_I2C1

#define I2C_SPEED               100000
#define I2C_SLAVE_ADDRESS7      0x30

/* SPIx Communication boards Interface */
// SPI1 MISO (PA6)	MOSI(PA7)  CLK(PA5)  NSS(PA4)

#define Open207Z_RCC_APB2Periph_SPIx   	       RCC_APB2Periph_SPI1
#define Open207Z_GPIO_AF_SPIx 				   GPIO_AF_SPI1

#define Open207Z_SPIx                           SPI1
#define Open207Z_SPIx_CLK                       RCC_APB2Periph_SPI1
#define Open207Z_SPIx_CLK_INIT                  RCC_APB1PeriphClockCmd
#define Open207Z_SPIx_IRQn                      SPI1_IRQn
#define Open207Z_SPIx_IRQHANDLER                SPI1_IRQHandler

#define Open207Z_SPIx_SCK_PIN                   GPIO_Pin_5
#define Open207Z_SPIx_SCK_GPIO_PORT             GPIOA
#define Open207Z_SPIx_SCK_GPIO_CLK              RCC_AHB1Periph_GPIOA
#define Open207Z_SPIx_SCK_SOURCE                GPIO_PinSource5
#define Open207Z_SPIx_SCK_AF                    GPIO_AF_SPI1

#define Open207Z_SPIx_MISO_PIN                  GPIO_Pin_6
#define Open207Z_SPIx_MISO_GPIO_PORT            GPIOA
#define Open207Z_SPIx_MISO_GPIO_CLK             RCC_AHB1Periph_GPIOA
#define Open207Z_SPIx_MISO_SOURCE               GPIO_PinSource6
#define Open207Z_SPIx_MISO_AF                   GPIO_AF_SPI1

#define Open207Z_SPIx_MOSI_PIN                  GPIO_Pin_7
#define Open207Z_SPIx_MOSI_GPIO_PORT            GPIOA
#define Open207Z_SPIx_MOSI_GPIO_CLK             RCC_AHB1Periph_GPIOA
#define Open207Z_SPIx_MOSI_SOURCE               GPIO_PinSource7
#define Open207Z_SPIx_MOSI_AF                   GPIO_AF_SPI1

/**
 * @brief Definition for LCD
 */
 /* Configure LCD pins: PB1->Reset and PB0->Back Light Control */
#define Open207Z_LCD_BackLightControl_PIN		GPIO_Pin_0
#define Open207Z_LCD_BackLightControl_PORT		GPIOB
#define Open207Z_LCD_BackLightControl_CLK		RCC_AHB1Periph_GPIOB

#define Open207Z_LCD_Reset_PIN					GPIO_Pin_1
#define Open207Z_LCD_Reset_PORT					GPIOB
#define Open207Z_LCD_Reset_CLK					RCC_AHB1Periph_GPIOB	

/* DCMI Communication boards Interface */

/**
 * @connected to I2C2
 */
 /* Configure I2C1 pins: PB10->SIOC and PB11->SIOD */ 
#define Open207Z_SCCB                        	I2C1
#define Open207Z_SCCB_CLK                    	RCC_APB1Periph_I2C1

#define Open207Z_SCCB_SDA_PIN                 	GPIO_Pin_9
#define Open207Z_SCCB_SDA_GPIO_PORT           	GPIOB
#define Open207Z_SCCB_SDA_GPIO_CLK            	RCC_AHB1Periph_GPIOB
#define Open207Z_SCCB_SDA_SOURCE              	GPIO_PinSource9
#define Open207Z_SCCB_SDA_AF                  	GPIO_AF_I2C1

#define Open207Z_SCCB_SCL_PIN                 	GPIO_Pin_8
#define Open207Z_SCCB_SCL_GPIO_PORT           	GPIOB
#define Open207Z_SCCB_SCL_GPIO_CLK            	RCC_AHB1Periph_GPIOB
#define Open207Z_SCCB_SCL_SOURCE              	GPIO_PinSource8
#define Open207Z_SCCB_SCL_AF                  	GPIO_AF_I2C1

#endif	  /*_CONFIG_H*/

