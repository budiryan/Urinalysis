/********************************************************************************
  * @file    I2C.h
  * @author  Ferhat YOL
  * @version V0.0.3
  * @date    10/15/2010
  * @brief   Software I2C Program
  *          
  ******************************************************************************/
#ifndef I2C_H
#define I2C_H
/* Includes ------------------------------------------------------------------*/
   #include "stm32f4xx.h"
	 
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/ 
	 #define I2C_Delay_Time 500
     #define I2C_Port GPIOB
	 #define I2C_SDA  GPIO_Pin_9
	 #define I2C_SCL  GPIO_Pin_8
/* Bits definitions ----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void Soft_I2C_Configuration(void);
void I2C_Delay(void);
uint8_t I2C_Start(void);
void I2C_Stop(void);
void I2C_Ack(void);
void I2C_NoAck(void);
uint8_t I2C_WaitAck(void);
void I2C_SendByte(unsigned char SendByte);
unsigned char I2C_ReceiveByte(void);
unsigned char I2C_ReadByte(unsigned char DeviceAddress,unsigned int ReadAddress);

#endif 
