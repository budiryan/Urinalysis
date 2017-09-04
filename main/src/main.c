#include "main.h"


int main() {
    //system_init();
    /*Test run pump */
    // Step pin: PA8 P11
    
    servo_init();
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); //enable the clock
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Speed = GPIO_Medium_Speed;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
    // Dir pin: PC2 P14
    GPIO_SetBits(GPIOC, GPIO_Pin_2);
    
    // Init step pin Servo 1
    servo_control(SERVO1, 300);
    
    led_init();			//Initiate LED
    ticks_init();		//Ticks initialization
    while(true){
        // Test LED ON OFF
        if(get_full_ticks() % 1000 == 0){
            LED_ON();
            GPIO_SetBits(GPIOC, GPIO_Pin_3);
        }
        else if(get_full_ticks() % 1000 == 500){
            LED_OFF();
            GPIO_ResetBits(GPIOC, GPIO_Pin_3);
        }
        
        /*
        LED_ON();
        GPIO_SetBits(GPIOC, GPIO_Pin_3);
        //servo_control(SERVO1, 15000);
        _delay_us(1500);
        LED_OFF();
        GPIO_ResetBits(GPIOC, GPIO_Pin_3);
        //servo_control(SERVO1, 0);
        _delay_us(1500);
        */
    }
    return 0;
} 
