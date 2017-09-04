#include "infrared_sensor.h"


uint16_t infrared_gpio_pins[4] = {INFRARED_LEFT_PIN, INFRARED_RIGHT_PIN, INFRARED_UPPER_LEFT_PIN, INFRARED_UPPER_RIGHT_PIN};


/**
* Init function
* @param void
* @return void
**/
void infrared_sensor_init(){
	//PA6
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Pin = INFRARED_LEFT_PIN | INFRARED_RIGHT_PIN | INFRARED_UPPER_LEFT_PIN | INFRARED_UPPER_RIGHT_PIN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(INFRARED_SENSOR_GPIO, &GPIO_InitStructure);
	
}

/**
* Return the state of the line sensor
* @param sensor: INFRARED_SENSOR_LEFT / INFRARED_SENSOR_RIGHT / INFRARED_SENSOR_UPPER_LEFT / INFRARED_SENSOR_UPPER_RIGHT
* @return 0 if white, 1 if not
**/
u8 read_infrared_sensor(INFRARED_SENSOR sensor){
    return GPIO_ReadInputDataBit(INFRARED_SENSOR_GPIO,infrared_gpio_pins[sensor]);
}
