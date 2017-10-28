#include "main.h"

static volatile bool capture_cam = true;
char str[40];
bool CONTINUOUS_MODE = true;

void init(){
    SystemInit();
    u8 switch_motor_running = 0;
    u8 switch_pump_running = 1;
    static volatile u8 current_time = 0;
    STEPPER_DIRECTION current_motor_direction = STEPPER_CW;
    PUMP_DIRECTION current_pump_direction = CW;   
    led_init();			//Initiate LED
    pump_init();
    stepper_init();
    // Stepper motor's speed does not depend on duty cycle of the pwm
	ticks_init();		//Ticks initialization
    TM_ILI9341_Init();
    TM_ILI9341_Fill(ILI9341_COLOR_MAGENTA);
    TM_ILI9341_Rotate(TM_ILI9341_Orientation_Landscape_2);
    button_init();
    OV9655_Configuration();
}


int main() {
    init();
    /* FOR COMPLETE PIN MAPPING INFORMATION: GO TO 'doc/pin_mapping.txt'----------*/
    // if(CONTINUOUS_MODE)
    //    DCMI_CaptureCmd(ENABLE);
    // else
    //    DCMI_CaptureCmd(DISABLE);
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
        /*
        if(button_pressed(BUTTON_K1)){
            // Just capture the picture one time
            while(button_pressed(BUTTON_K1));
            DCMI_CaptureCmd(ENABLE);
            TM_ILI9341_DisplayImage((uint16_t*) frame_buffer);
            LED_ON(LED_2);
            _delay_ms(500);
            DCMI_CaptureCmd(DISABLE);
        }*/
        
        if(capture_cam == true){
            capture_cam = false;
            TM_ILI9341_DisplayImage((uint16_t*) frame_buffer);
            capture_segment();
            display_color_average((uint16_t*)segmentation, SEGMENT_COLUMNS * SEGMENT_ROWS, RGB565);
        }
        
        if(button_pressed(BUTTON_K0)){
            while(button_pressed(BUTTON_K0));
        }
    }
}



void DMA2_Stream1_IRQHandler(void){
	// DMA complete
	if(DMA_GetITStatus(DMA2_Stream1,DMA_IT_TCIF1) != RESET){
		DMA_ClearITPendingBit(DMA2_Stream1,DMA_IT_TCIF1);
        capture_cam = true;
    }
}




