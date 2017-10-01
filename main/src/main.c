#include "main.h"


int main() {
    
    led_init();			//Initiate LED
    pump_init();
    stepper_init();
    button_init();
    pump(400, CW);
    // Stepper motor's speed does not depend on duty cycle of the pwm
    stepper_spin(STEPPER_CW);
	ticks_init();		//Ticks initialization
    TM_ILI9341_Init();
    TM_ILI9341_Fill(ILI9341_COLOR_MAGENTA);
    TM_ILI9341_Rotate(TM_ILI9341_Orientation_Landscape_2);
    LED_OFF();
    
    while(true){
        // Test LED ON OF F
        if(button_pressed(BUTTON_K0)){
            LED_ON();
        }
        if(button_pressed(BUTTON_K1)){
            LED_OFF();
        }
    }
     
} 
