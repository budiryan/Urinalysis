#include "main.h"


int main() {
    
    led_init();			//Initiate LED
    //tft_init(0, RED, WHITE, BLUE);
    // pump_init();
    // stepper_init();
    // pump(300, CW);
    // button_init();
    // Stepper motor does not depend on duty cycle of the pwm
    // stepper_spin(STEPPER_CW);
	ticks_init();		//Ticks initialization
    TM_ILI9341_Init();
    TM_ILI9341_Fill(ILI9341_COLOR_MAGENTA);
    TM_ILI9341_Rotate(TM_ILI9341_Orientation_Landscape_2);
    LED_ON();
    
    while(true){
        // Test LED ON OF F
        if(get_full_ticks() % 2500 == 0){
            TM_ILI9341_Fill(ILI9341_COLOR_WHITE);
            LED_OFF();
            //LED_ON();
            // DEBUG
            // stepper_spin(400, STEPPER_CW);
            // pump(400, CCW);
        }
        else if(get_full_ticks() % 2500 == 1250){
            TM_ILI9341_Fill(ILI9341_COLOR_MAGENTA);
            TM_ILI9341_Puts(60, 150, "ILI9341 LCD Module", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
            LED_ON();
            //DEBUG
            //stepper_spin(0, STEPPER_CCW);
            //pump(0, CW);
        }
    }
     
} 
