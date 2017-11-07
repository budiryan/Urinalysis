/**
  ******************************************************************************
  * @file    I2C.c
  * @author  Ferhat YOL
  * @version V0.0.3
  * @date    10/15/2010
  * @brief   Software I2C Program
  *          
  ******************************************************************************/
	/* Includes ------------------------------------------------------------------*/
  #include "stm32f4xx.h"
	#include "I2C.h"
	#include "main.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Bits definitions ----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
   #define SDAH GPIO_SetBits(I2C_Port,I2C_SDA)
   #define SDAL GPIO_ResetBits(I2C_Port,I2C_SDA)
   #define SCLH GPIO_SetBits(I2C_Port,I2C_SCL)
   #define SCLL GPIO_ResetBits(I2C_Port,I2C_SCL)
   #define SDAR GPIO_ReadInputDataBit(I2C_Port,I2C_SDA)//GPIOB->IDR & I2C_SDA
   #define SCLR GPIO_ReadInputDataBit(I2C_Port,I2C_SCL)//GPIOB->IDR & I2C_SDA
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

void Soft_I2C_Configuration(void){
  GPIO_InitTypeDef GPIO_InitStructure;
  /* Enable GPIOB clocks */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	/*SDA Pin Configuration*/
  GPIO_InitStructure.GPIO_Pin = I2C_SDA; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_Init(I2C_Port, &GPIO_InitStructure);
	/*SCL Pin Configuration*/
  GPIO_InitStructure.GPIO_Pin = I2C_SCL; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(I2C_Port, &GPIO_InitStructure);
}


void I2C_Delay(void)
{
   unsigned int i=I2C_Delay_Time;
   while(i)
   {
     i--;
   }
}

uint8_t I2C_Start(void)
{
  SDAH;
	SCLH;
	I2C_Delay();
  SDAL;
	I2C_Delay();
	SCLL;
	I2C_Delay();
 return (0x00);
}

void I2C_Stop(void)
{
 I2C_Delay();
 SDAL;
 I2C_Delay();
 SCLH;
 I2C_Delay();
 SDAH;
 I2C_Delay();
}

void I2C_Ack(void)
{
 SCLL;
 I2C_Delay();
 SDAL;
 I2C_Delay();
 SCLH;
 I2C_Delay();
 SCLL;
 I2C_Delay();
}

void I2C_NoAck(void)
{
 SCLL;
 I2C_Delay();
 SDAH;
 I2C_Delay();
 SCLH;
 I2C_Delay();
 SCLL;
 I2C_Delay();
}
 
uint8_t I2C_WaitAck(void)  
{
	SDAH;
  I2C_Delay();
	SCLH;I2C_Delay();  //Clock Uygulaniyor
	if(SDAR) return (0xFF);
	;SCLL;I2C_Delay();
	return(0x00);
}

void I2C_SendByte(unsigned char SendByte)
{
    unsigned char i=0;
    for(i=0;i<8;i++)
		{
      if ((SendByte << i) & 0x80)
			 {
				 SDAH;
       }
			 else
			 {
         SDAL;
       }
			 I2C_Delay();
       SCLH;
			 I2C_Delay();
       SCLL;
			 I2C_Delay();
    }
}
 
unsigned char I2C_ReceiveByte(void)  
{
    unsigned char i=8;
    unsigned char ReceiveByte=0;
 
    SDAH;    
    while(i--)
    {
      ReceiveByte<<=1;      
      SCLL;
      I2C_Delay();
      SCLH;
      I2C_Delay();
      if(SDAR)
      {
        ReceiveByte|=0x01;
      }
    }
    SCLL;
    return ReceiveByte;
}

unsigned char I2C_ReadByte( unsigned char DeviceAddress,unsigned int ReadAddress)
{  
    unsigned char temp;
    if(!I2C_Start())return (0xFF);    
 
    I2C_SendByte((DeviceAddress & 0xFF));
    if(!I2C_WaitAck()){I2C_Stop(); return 0xFF;}
   
    I2C_SendByte((unsigned char)((ReadAddress>>8) & 0xFF));    
    I2C_WaitAck();
    I2C_SendByte((unsigned char)((ReadAddress) & 0xFF));        
    I2C_WaitAck();
    I2C_Start();
    I2C_SendByte((DeviceAddress & 0xFE)|0x01);    
    I2C_WaitAck();
   
    temp = I2C_ReceiveByte();
   
    I2C_NoAck();
     
    I2C_Stop();
    return temp;
}

