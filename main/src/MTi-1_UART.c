#include "MTi-1_UART.h"
/**
** MTi-1_UART Library for STM32F4 Eco robot
** @Author James Mok, Simon Tam
** @Note   We think no body will read the code so its kinda messy but nevermind :)
**/

float MTi_ang[3] = {0, 0, 0};
float MTi_acc[3] = {0, 0, 0};

u8 MTi_msg[8] = {0, 0, 0, 0, 0, 0, 0, 0};
u8 raw_buffer[4];

/**
**@descript Init function
**/
void MTi_1_UART_init(void)
{
	uart_init(COM2, MTi_1_default_BR);
	uart_interrupt_init(COM2, MTi_1_UART_Rx);
	RCC_AHB1PeriphClockCmd(MTi_1_UART_GPIO_RCC, ENABLE);
	
	GPIO_InitTypeDef GPIOInitStructure;
	GPIOInitStructure.GPIO_Pin = MTi_1_UART_RTS_Pin;
	GPIOInitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIOInitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIOInitStructure.GPIO_OType = GPIO_OType_PP;
	GPIOInitStructure.GPIO_Speed = GPIO_Fast_Speed;
	GPIO_Init(MTi_1_UART_GPIO, &GPIOInitStructure);
	
	
	clear_buffer();
	send_MTi_1_UART_msg(NULL, GoToMeasurement, 0);
	//Setup the x,y,z axis
	MTi_msg[1] = 3;
	send_MTi_1_UART_msg(&MTi_msg[0], ResetOrientation,2);
	MTi_msg[1] = 1;
	send_MTi_1_UART_msg(&MTi_msg[0], ResetOrientation,2);
	MTi_msg[1] = 0;
}



/**
**	@descript Construct the message to the IMU. See MT Datasheet for details
**	@para 		*data: the data going the send
**  	@para			*MID: the message header
**	@para			*data_length: no of bytes of the data
**	
**/
void send_MTi_1_UART_msg(u8 *data, u8 mid, u16 data_length)
{
	while(GPIO_ReadInputDataBit(MTi_1_UART_GPIO, MTi_1_UART_RTS_Pin) != Bit_RESET);
		
	u8 temp[data_length + 5];
	u8 checksum = 0;
	
	temp[0] = MTi_1_Preamble;
	temp[1] = MTi_1_MasterDevice;
	temp[2] = mid;
	temp[3] = data_length;
	
	for(u8 i=4; i<4+data_length; i++)
	{
		temp[i] = *(data + i - 4);
	}
	
	for(u8 i=1; i<data_length + 4; i++)
		checksum += temp[i];
	
	checksum = 0xFF - checksum + 0x01;
	temp[data_length + 4] = checksum;
	
	for(u8 k=0; k<(data_length + 5); k++)
	{
		uart_tx_byte(COM2, temp[k]);
	}
}


/**
**@descript the UART listener used for the interrupt
**@para 		*data: the Rx received data
**/
void MTi_1_UART_Rx(u8 data)
{
	static u16 data_length 	= 5;	
	static u8  ANG_COUNT 		= 0;	//counter for receiving Pdata
	static u8  ACC_COUNT 		= 0;
	static u8  MID = 0;
	static u16 header_count = 0;	//count the header
  static u16 rx_count 		= 0;	//incremented by one when RX_interrupt occoured
	rx_count++;	
	
	if(data == MTi_1_Preamble && rx_count == 1) //The 1st byte == MTi_1_Preamble
		header_count++;
	else if(data == MTi_1_MasterDevice && rx_count == 2) //The 2nd byte == MTi_1_MasterDevice
		header_count++;
	else if(rx_count == 3 && header_count == 2) //The 3rd byte == MID
	{
		header_count++;
		MID = data;
	}
	else if(header_count == 3 && MID == MTData2)	//The receive msg is data
	{
		if(rx_count == 4) //The 4th byte = data length
			data_length = data;
		
		if(rx_count == 7 && data == 0x0C)
		{
			ANG_COUNT = 12;
		}
		
		if(ANG_COUNT && rx_count >= 8) //The 8-19th byte = x,y,z angle
		{
			if(ANG_COUNT > 8)
			{
				raw_buffer[(12-ANG_COUNT)%4] = data; 
				ANG_COUNT--;
				if(ANG_COUNT == 8)
				{
					MTi_ang[0] = flt_cal(raw_buffer);
					clear_buffer();
				}
			}
			else if(ANG_COUNT > 4 && ANG_COUNT <= 8)
			{
				raw_buffer[(12-ANG_COUNT)%4] = data;
				ANG_COUNT--;
				if(ANG_COUNT == 4)
				{
					MTi_ang[1] = flt_cal(raw_buffer);
					clear_buffer();
				}
			}
			else if(ANG_COUNT>0 && ANG_COUNT<=4)
			{
				raw_buffer[(12-ANG_COUNT)%4] = data;
				ANG_COUNT--;
				if(!ANG_COUNT)
				{
					ACC_COUNT = 12;
					MTi_ang[2] = flt_cal(raw_buffer);
					clear_buffer();
				}
			}
		}
		if(rx_count >= 23 && ACC_COUNT) //The 23-34th byte = x,y,z acceleration
		{
			if(ACC_COUNT > 8)
			{
				raw_buffer[(12-ACC_COUNT)%4] = data;
				ACC_COUNT--;
				if(ACC_COUNT == 8)
				{
					MTi_acc[0] = flt_cal(raw_buffer);
					clear_buffer();
				}
			}
			else if(ACC_COUNT > 4 && ACC_COUNT <= 8)
			{
				raw_buffer[(12-ACC_COUNT)%4] = data;
				ACC_COUNT--;
				if(ACC_COUNT == 4)
				{
					MTi_acc[1] = flt_cal(raw_buffer);
					clear_buffer();
				}
			}
			else if(ACC_COUNT > 0 && ACC_COUNT <= 4)
			{
				raw_buffer[(12-ACC_COUNT)%4] = data;
				ACC_COUNT--;
				if(!ACC_COUNT)
				{
					MTi_acc[2] = flt_cal(raw_buffer);
					clear_buffer();
				}
			}
		}
	}
	else
	{
		rx_count = 0;
		header_count = 0;
	}
	
	if(rx_count == data_length + 4)
	{
		rx_count = 0;
		header_count = 0;
	}
	
}	
	
/**
**@descript Reset the buffer to zero
**/
void clear_buffer(void){
	raw_buffer[0] = 0;
	raw_buffer[1] = 0;
	raw_buffer[2] = 0;
	raw_buffer[3] = 0;
}

/**
**@descript Translate a 4-byte array to a floating point number 
**@para  		*data[4]: the 4-byte array
**@retval 	float value of the array according to the IEEE-745 standard
**/
float flt_cal(u8 data[4]){
	u8 e;
	int temp;
	int i;
	float result;
	e = (data[0]<<1)|(data[1]>>7);
	temp = (0x800000|data[1]<<16)|(data[2]<<8)|data[3];
	result = (float)temp;
	temp = 127-e+23;
	if(temp>0)for(i=0;i<temp;i++)result= result/ (double)2.0;
	else for(i=0;i>temp;i--)result = result * (double)2.0;
	return data[0]&0x80?-result:result;
}


float get_MTi_ang(u8 index)
{
	return MTi_ang[index];
}

float get_MTi_acc(u8 index)
{
	return MTi_acc[index];
}
