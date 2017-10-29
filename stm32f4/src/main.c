#include "main.h"

static volatile bool capture_cam = true;
char str[40];
bool CONTINUOUS_MODE = true;
u8 switch_motor_running = 0;
u8 switch_pump_running = 1;
static volatile u8 current_time = 0;
STEPPER_DIRECTION current_motor_direction = STEPPER_CW;
PUMP_DIRECTION current_pump_direction = CW;
u16 pump_time_stamp;
u16 motor_time_stamp;

void begin_rotate_motor(void){
    TM_ILI9341_Puts(180, 20, "            ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
    while ((get_seconds() - motor_time_stamp) < PUMP_DURATION){
        pump(400, CW);
        TM_ILI9341_Puts(180, 20, "ROTATE", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
    }
    // After some second, stop
    pump(0, CW);
}

void begin_pump(void){
    // Pump for some seconds
    TM_ILI9341_Puts(180, 20, "            ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
    while ((get_seconds() - pump_time_stamp) < PUMP_DURATION){
        pump(400, CW);
        TM_ILI9341_Puts(180, 20, "PUMPING", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
    }
    // After some second, stop
    pump(0, CW);
}

void init(){
    SystemInit();
    led_init();			//Initiate LED
    pump_init();
    stepper_init();
    // Stepper motor's speed does not depend on duty cycle of the pwm
	ticks_init();		//Ticks initialization
    TM_ILI9341_Init();
    TM_ILI9341_Fill(ILI9341_COLOR_WHITE);
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
    TM_ILI9341_Puts(180, 0, "Process: ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
    while(true){
        // TM_ILI9341_Puts(180, 0, "O: ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
        if(button_pressed(BUTTON_K1)){
            while(button_pressed(BUTTON_K1));
            // TURN PUMP ON / OFF
            pump_time_stamp = get_seconds();
            begin_pump();
            TM_ILI9341_Puts(180, 20, "DONE PUMPING", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
        }
        
        if(button_pressed(BUTTON_K0)){
            current_time = get_full_ticks();
            while(button_pressed(BUTTON_K0));
            motor_time_stamp = get_seconds();
            TM_ILI9341_Puts(180, 20, "DONE ROTATE", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
        }
        
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
            TM_ILI9341_DisplayImage((u16 *) frame_buffer);
            capture_segment();
            display_color_average((u16 *)segmentation, SEGMENT_COLUMNS * SEGMENT_ROWS, RGB565);
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




