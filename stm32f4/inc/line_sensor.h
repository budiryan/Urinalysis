#ifndef _LINE_SENSOR_H
#define _LINE_SENSOR_H

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"

#define LINE_SENSOR_1_PIN			GPIO_Pin_10
#define LINE_SENSOR_2_PIN			GPIO_Pin_11
#define LINE_SENSOR_3_PIN			GPIO_Pin_12
#define LINE_SENSOR_GPIO			GPIOE

void line_sensor_init(void);

typedef enum{
	LINE_SENSOR_1 = 0,
	LINE_SENSOR_2 = 1,
	LINE_SENSOR_3 = 2
} LINE_SENSOR;

//Left Digit = Left sensor
typedef enum{
	track_turn_left_90 = 1, 			//001
	track_turn_left_smooth = 3, 	//011
	track_turn_right_90 = 4, 			//100
	track_turn_right_smooth = 6, 	//110
	track_turn_leave = 7, 				//111
	track_turn_happy = 5					//101 
} TRACK_LINE_STATE;

u8 read_line_sensor(LINE_SENSOR);
u8 get_line_sensor_all(void);

#endif
