#include "main.h"


int main() {
    led_init();			//Initiate LED
    ticks_init();		//Ticks initialization
    pump_init();
    pump(300, CCW);
    while(true){
        // Test LED ON OF F
        if(get_full_ticks() % 4000 == 0){
            LED_ON();
        }
        else if(get_full_ticks() % 4000 == 2000){
            LED_OFF();
        }
    }
} 
