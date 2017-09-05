#include "main.h"


int main() {
    led_init();			//Initiate LED
    ticks_init();		//Ticks initialization
    pump_init();
    while(true){
        // Test LED ON OFF
        if(get_full_ticks() % 2000 == 0){
            LED_ON();
            pump(350, CCW);
        }
        else if(get_full_ticks() % 2000 == 1000){
            LED_OFF();
            pump(350, CW);
        }
    }
} 
