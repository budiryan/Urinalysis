#ifndef __INFRARED_SENSOR
#define __INFRARED_SENSOR

#include "stm32f4xx_gpio.h"

#define INFRARED_SENSOR_GPIO GPIOC
#define INFRARED_LEFT_PIN GPIO_Pin_0
#define INFRARED_RIGHT_PIN GPIO_Pin_1
#define INFRARED_UPPER_LEFT_PIN GPIO_Pin_2
#define INFRARED_UPPER_RIGHT_PIN GPIO_Pin_3


typedef enum{
	INFRARED_SENSOR_LEFT = 0,
	INFRARED_SENSOR_RIGHT = 1,
    INFRARED_SENSOR_UPPER_LEFT = 2,
    INFRARED_SENSOR_UPPER_RIGHT = 3
} INFRARED_SENSOR;


void infrared_sensor_init(void);
u8 read_infrared_sensor(INFRARED_SENSOR sensor);


#endif
