#include "adc_ir.h"

void ir_init(){
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed;
	GPIO_InitStruct.GPIO_Pin = IR_1_Pin | IR_2_Pin;
	GPIO_Init(IR_GPIO, &GPIO_InitStruct);
}

bool readIR(u8 id){
	if (id == 0){
		return !GPIO_ReadInputDataBit(IR_GPIO, IR_1_Pin);
	}else{
		return !GPIO_ReadInputDataBit(IR_GPIO, IR_2_Pin);
	}
}


s16 get_ir_dis(u8 i){
	switch(i){
		case 0:
			return (s16)roundf(1/(adc_get(i)*(ADC_IR_MIN - ADC_IR_MAX)/(ADC_IR_1_MAX - ADC_IR_1_MIN)));
		case 1:
			return (s16)roundf(1/(adc_get(i)*(ADC_IR_MIN - ADC_IR_MAX)/(ADC_IR_2_MAX - ADC_IR_2_MIN)));
		default:
			return 0;
	}
}
