#include "main.h"

u8 switch_motor_running = 0;
u8 switch_pump_running = 1;
STEPPER_DIRECTION current_motor_direction = STEPPER_CW;
PUMP_DIRECTION current_pump_direction = CW;

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
    /* FOR COMPLETE PIN MAPPING INFORMATION: GO TO 'doc/pin_mapping.txt'----------*/
    char str[12];    
    led_init();			//Initiate LED
    pump_init();
    stepper_init();
    // Stepper motor's speed does not depend on duty cycle of the pwm
	ticks_init();		//Ticks initialization
    TM_ILI9341_Init();
    TM_ILI9341_Fill(ILI9341_COLOR_MAGENTA);
    TM_ILI9341_Rotate(TM_ILI9341_Orientation_Landscape_2);
    int camera_status;
    camera_status = OV9655_Configuration();
    button_init();
    TM_ILI9341_Puts(100, 100, itoa(camera_status, str, 16), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
    
    // test: READ CAMERA's MANUFACTURER ID
    OV9655_IDTypeDef OV9655_ID;
    DCMI_OV9655_ReadID(&OV9655_ID);
    int id1 = OV9655_ID.Manufacturer_ID1;
    int id2 = OV9655_ID.Manufacturer_ID2;
    char id1_str[4];
    char id2_str[4];
    TM_ILI9341_Puts(100, 120, itoa(id1, id1_str, 16), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
    TM_ILI9341_Puts(100, 140, itoa(id2, id2_str, 16), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
    
    while(true){
        /*
        TM_ILI9341_Puts(100, 100, itoa(get_seconds(), str, 10), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
        if(button_pressed(BUTTON_K0)){
            current_time = get_full_ticks();
            while(button_pressed(BUTTON_K0));
                // CHANGE BOTH DIRECTION AND SPEED
             switch(current_motor_direction){
                case STEPPER_CW:
                    stepper_spin(current_motor_direction, 200);
                    current_motor_direction = STEPPER_CCW;
                    break;
                case STEPPER_CCW:
                    stepper_spin(current_motor_direction, 0);
                    current_motor_direction = STEPPER_CW;
                    break;
            }
        }
        if(button_pressed(BUTTON_K1)){
            while(button_pressed(BUTTON_K1));
            // TURN PUMP ON / OFF
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
        */
    }
} 



