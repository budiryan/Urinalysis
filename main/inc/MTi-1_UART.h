#ifndef MTI_1_UART__H
#define	MTI_1_UART__H

#include "stm32f4xx.h"
#include "usart.h"
#include "stm32f4xx_gpio.h"
#include "ticks.h"

//UART setting
#define MTi_1_UART								USART2
#define MTi_1_UART_Tx_Pin					GPIO_Pin_2
#define	MTi_1_UART_Rx_Pin					GPIO_Pin_3
#define MTi_1_UART_RTS_Pin				    GPIO_Pin_4
#define MTi_1_UART_GPIO						GPIOA
#define MTi_1_UART_RCC						RCC_APB1Periph_USART2
#define MTi_1_UART_GPIO_RCC				    RCC_AHB1Periph_GPIOA
#define MTi_1_default_BR					(uint32_t)115200


//Op_code
#define ProtocolInfo					    (uint8_t)0x01
#define ConfigureProtocol					(uint8_t)0x02
#define ControlPipe							(uint8_t)0x03
#define PipeStatus							(uint8_t)0x04
#define NotificationPipe					(uint8_t)0x05
#define MeasurementPipe 					(uint8_t)0x06

#define	MTi_1_Preamble						(uint8_t)0xFA
#define MTi_1_MasterDevice				(uint8_t)0xFF

//MID
#define	WakeUp										(uint8_t)0x3E
#define WakeUpAck									(uint8_t)0x3F
#define	Reset											(uint8_t)0x40
#define InitMT										(uint8_t)0x02
#define InitMTResults 						(uint8_t)0x03
#define	GoToConfig								(uint8_t)0x30
#define	GoToMeasurement						(uint8_t)0x10
#define ReqDataLength							(uint8_t)0x0A
#define	DataLength								(uint8_t)0x0B
#define Error											(uint8_t)0x42
#define ReqOutputMode							(uint8_t)0xD0
#define	SetOutputMode							(uint8_t)0xD0
#define	ReqOutputSettings					(uint8_t)0xD2
#define SetOutputSettings					(uint8_t)0xD2
#define	ReqData										(uint8_t)0x34
#define MTData										(uint8_t)0x32
#define MTData2										(uint8_t)0x36
#define ReqConfiguration					(uint8_t)0x0C
#define SetOptionFlags 						(uint8_t)0x48
#define ResetOrientation					(uint8_t)0xA4

typedef enum
{
	MTi_Data_Ready 			= 1,
	MTi_Data_Not_Ready 	= 0
}MTi_Data_Statue;
	
typedef enum
{
	rx_NA 			= 0,
	rx_Preamble = 1,
	rx_BusID 		= 2,
	rx_MsgID 		= 3,
	rx_Length 	= 4,
	rx_Payload 	= 5,
	rx_Checksum = 6
}MTi_Rx_Statue;

//Functions
void MTi_1_UART_init(void);
void send_MTi_1_UART_msg(u8 *data, u8 MID, u16 data_length);
void MTi_1_UART_Rx(u8 data);
float get_MTi_acc(u8 index);
float get_MTi_ang(u8 index);
u8 get_ebuffer(u8 index);
void clear_buffer(void);
float flt_cal(u8 data[4]);
#endif
