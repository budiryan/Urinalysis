#include "main.h"

u8 switch_motor_running = 0;
u8 switch_pump_running = 1;

/*Used for Debugging Purposes --> Converts an integer to char array*/
char * itoa (int value, char *result, int base)
{
    // check that the base if valid
    if (base < 2 || base > 36) { *result = '\0'; return result; }

    char* ptr = result, *ptr1 = result, tmp_char;
    int tmp_value;

    do {
        tmp_value = value;
        value /= base;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
    } while ( value );

    // Apply negative sign
    if (tmp_value < 0) *ptr++ = '-';
    *ptr-- = '\0';
    while (ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmp_char;
    }
    return result;
}

int main() {
    
    led_init();			//Initiate LED
    pump_init();
    stepper_init();
    pump(400, CW);
    // Stepper motor's speed does not depend on duty cycle of the pwm
    stepper_spin(STEPPER_CW, 10);
	ticks_init();		//Ticks initialization
    TM_ILI9341_Init();
    TM_ILI9341_Fill(ILI9341_COLOR_MAGENTA);
    TM_ILI9341_Rotate(TM_ILI9341_Orientation_Landscape_2);
    button_init();
    while(true){
        if(button_pressed(BUTTON_K0)){
            while(button_pressed(BUTTON_K0));
            switch(switch_motor_running){
                case 0:
                    stepper_spin(STEPPER_CW, 0);
                    LED_OFF(LED_1);
                    switch_motor_running = 1;
                    break;
                case 1:
                    stepper_spin(STEPPER_CW, 200);
                    LED_ON(LED_1);
                    switch_motor_running = 0;
                    break;
            }
        }
        if(button_pressed(BUTTON_K1)){
            while(button_pressed(BUTTON_K1));
            switch(switch_pump_running){
                case 0:
                    pump(400, CW);
                    LED_ON(LED_2);
                    switch_pump_running = 1;
                    break;
                case 1:
                    pump(0, CW);
                    LED_OFF(LED_2);
                    switch_pump_running = 0;
                    break;
            }
        }
    }
} 



