#include "main.h"

static volatile int frame_flag = false;
static volatile bool capture_cam = false;
char str[12];
int counter = 0;
int counter_2 = 100;

//Enter capture mode: Take a single image
void camera_capture(){
    // Stop DCMI
    DCMI_CaptureCmd(DISABLE);
	while(DCMI->CR & 0x01);
	DMA_Cmd(DMA2_Stream1, DISABLE);
	DMA_DoubleBufferModeCmd(DMA2_Stream1, DISABLE);
	
    DMA_Cmd(DMA2_Stream1, DISABLE);
	while(DMA2D_GetFlagStatus(DMA2D_FLAG_TC) != RESET); //Wait until current transmission is ok
	//lcd_frame_transfer_in();
}

/* Converts an integer to char array */
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


void DisplayImage(uint16_t image[ILI9341_PIXEL]){
    uint32_t count = 0;
    
    for (int i = 0 ; i < IMG_COLUMNS; i++){
        for (int j = 0 ; j < IMG_ROWS; j++){
            TM_ILI9341_DrawPixel(i, j, frame_buffer[count]);
            count++;
        }
    }
}


int main() {
    /* FOR COMPLETE PIN MAPPING INFORMATION: GO TO 'doc/pin_mapping.txt'----------*/
    SystemInit();
    u8 switch_motor_running = 0;
    u8 switch_pump_running = 1;
    static volatile u8 current_time = 0;
    STEPPER_DIRECTION current_motor_direction = STEPPER_CW;
    PUMP_DIRECTION current_pump_direction = CW;   
    led_init();			//Initiate LED
    // pump_init();
    // stepper_init();
    // Stepper motor's speed does not depend on duty cycle of the pwm
	ticks_init();		//Ticks initialization
    TM_ILI9341_Init();
    // TM_ILI9341_Fill(ILI9341_COLOR_MAGENTA);
    TM_ILI9341_Rotate(TM_ILI9341_Orientation_Landscape_1);
    int camera_status;
    camera_status = OV9655_Configuration();
    // button_init();
    // TM_ILI9341_Puts(100, 100, itoa(camera_status, str, 16), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
    
    // test: READ CAMERA's MANUFACTURER ID
    OV9655_IDTypeDef OV9655_ID;
    DCMI_OV9655_ReadID(&OV9655_ID);
    int id1 = OV9655_ID.Manufacturer_ID1;
    int id2 = OV9655_ID.Manufacturer_ID2;
    char id1_str[4];
    char id2_str[4];
    // TM_ILI9341_Puts(0, 20, itoa(id1, id1_str, 16), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
    // TM_ILI9341_Puts(0, 40, itoa(id2, id2_str, 16), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);

    DCMI_CaptureCmd(ENABLE);
    while(true){
        /*
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
        if(button_pressed(BUTTON_K1)){
            while(button_pressed(BUTTON_K1));
            LED_ON(LED_2);
            counter++;
        }
        // TM_ILI9341_Puts(100, 100, itoa(frame_buffer[2000], str, 16), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
        if (capture_cam == true){
            TM_ILI9341_DisplayImage((uint16_t*) frame_buffer);
            //DisplayImage((uint16_t*) frame_buffer);
            capture_cam = false;
            // TM_ILI9341_Puts(100, 100, itoa(frame_buffer[600], str, 16), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
            //for (int i = 0 ; i < 100; i++){
             //   TM_ILI9341_DrawPixel(i, 30, frame_buffer[i]);
            // }
        }
    }
}


void DMA2_Stream1_IRQHandler(void){
	// DMA complete
	if(DMA_GetITStatus(DMA2_Stream1,DMA_IT_TCIF1) != RESET){
		DMA_ClearITPendingBit(DMA2_Stream1,DMA_IT_TCIF1);
        // TM_ILI9341_Puts(100, 100, itoa(frame_buffer[2000], str, 16), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
        
        /*
        TM_ILI9341_Puts(120, 200, itoa(counter_2, str, 16), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
        
        TM_ILI9341_Puts(120, 160, itoa(frame_buffer[counter_2], str, 16), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
        
        
        //TM_ILI9341_Puts(120, 180, "                 ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
        //TM_ILI9341_Puts(120, 200, itoa(counter_2, str, 10), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
        counter_2++;
        */
        // DCMI_CaptureCmd(ENABLE); 
        /*
        if(counter == 0){
            LED_ON(LED_2);
            counter = 1;
        }
        else{
            LED_OFF(LED_2);
            counter = 0;
        }
        */
        capture_cam = true;      
    }
}




