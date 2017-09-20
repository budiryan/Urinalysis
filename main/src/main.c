#include "main.h"


int main() {
    led_init();			//Initiate LED
    ticks_init();		//Ticks initialization
    pump_init();
    stepper_init();
    pump(300, CW);
    button_init();
    // Stepper motor does not depend on duty cycle of the pwm
    stepper_spin(STEPPER_CW);
    while(true){
        // Test LED ON OF F
        if(get_full_ticks() % 4000 == 0){
            LED_ON();
            // DEBUG
            // stepper_spin(400, STEPPER_CW);
            // pump(400, CCW);
        }
        else if(get_full_ticks() % 4000 == 2000){
            LED_OFF();
            //DEBUG
            //stepper_spin(0, STEPPER_CCW);
            //pump(0, CW);
        }
    }
} 
