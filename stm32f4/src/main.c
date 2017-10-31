#include "main.h"

static volatile bool capture_cam = true;
char str[40];
bool CONTINUOUS_MODE = true;
u8 switch_motor_running = 0;
u8 switch_pump_running = 1;
static volatile u8 current_time = 0;
STEPPER_DIRECTION current_motor_direction = STEPPER_CW;
PUMP_DIRECTION current_pump_direction = CW;
uint32_t pump_time_stamp;
uint32_t motor_time_stamp;
#define PUMP_DURATION 97
#define MOTOR_DURATION_MS 3250
#define ROTATION_COUNT 8

void begin_rotate_motor(void){
}

void begin_pump(void){
    // Pump for some seconds
    pump_time_stamp = get_seconds();
    TM_ILI9341_Puts(180, 20, "           ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
    TM_ILI9341_Puts(180, 20, "PUMPING", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
    while ((get_seconds() - pump_time_stamp) < PUMP_DURATION){
        TM_ILI9341_Puts(180, 80, itoa(get_seconds(), str, 10), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
        pump(400, CCW);
    }
    // After some second, stop
    pump(0, CCW);
}

void clean_pump(void){
    // Pump for some seconds
    pump_time_stamp = get_seconds();
    TM_ILI9341_Puts(180, 20, "           ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
    TM_ILI9341_Puts(180, 20, "CLEAN PUMP", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
    while ((get_seconds() - pump_time_stamp) < PUMP_DURATION){
        TM_ILI9341_Puts(180, 80, itoa(get_seconds(), str, 10), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
        pump(400, CW); // only direction is different now...
    }
    // After some second, stop
    pump(0, CW);
}

void rotate_all_section(void){
    motor_time_stamp = get_full_ticks();
    TM_ILI9341_Puts(180, 20, "            ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
    TM_ILI9341_Puts(180, 20, "ROTATING", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
    pump(0, CW);
    while ((get_full_ticks() - motor_time_stamp) < MOTOR_DURATION_MS){
        TM_ILI9341_Puts(180, 80, itoa(get_seconds(), str, 10), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
        stepper_spin(STEPPER_CW, 300);
    }
    // After some second, stop
    stepper_spin(STEPPER_CW, 0);
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
    DCMI_CaptureCmd(DISABLE);
    TM_ILI9341_Puts(180, 0, "STATUS: ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
    TM_ILI9341_Puts(180, 20, "Idle ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
    TM_ILI9341_Puts(0, 0, "Urinalysis", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
    TM_ILI9341_Puts(0, 40, "Press K0 to", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
    TM_ILI9341_Puts(0, 60, "begin operation", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
    while(true){
        TM_ILI9341_Puts(180, 60, "Time: ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
        TM_ILI9341_Puts(180, 80, itoa(get_seconds(), str, 10), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
        if(button_pressed(BUTTON_K0)){
            // Analyze the image in 1 press of a button
            while(button_pressed(BUTTON_K0));
            TM_ILI9341_Puts(0, 40, "                ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
            TM_ILI9341_Puts(0, 60, "                ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
            begin_pump();
            TM_ILI9341_Puts(180, 20, "            ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
            TM_ILI9341_Puts(180, 20, "DONE PUMPING", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
            // Rotate to distribute the urine evenly
            for (int i  = 0 ; i < ROTATION_COUNT ; i++){
                rotate_all_section();
                pump(400, CCW);
                _delay_ms(5100);
            }
            clean_pump();
            TM_ILI9341_Puts(180, 20, "            ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
            TM_ILI9341_Puts(180, 20, "PUMP CLEAN", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
            _delay_ms(1000);
            TM_ILI9341_Puts(180, 20, "            ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
            DCMI_CaptureCmd(ENABLE);
            TM_ILI9341_Puts(180, 20, "Analyzing", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
            _delay_ms(4000);
            TM_ILI9341_DisplayImage((u16 *) frame_buffer);
            capture_segment();
            display_color_average((u16 *)segmentation, SEGMENT_COLUMNS * SEGMENT_ROWS, RGB565);
        }
        
        if(button_pressed(BUTTON_K1)){
            // Capture one time and display analysis
            while(button_pressed(BUTTON_K1));
            DCMI_CaptureCmd(ENABLE);
            TM_ILI9341_Puts(180, 20, "Analyzing", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
            _delay_ms(4000);
            TM_ILI9341_DisplayImage((u16 *) frame_buffer);
            capture_segment();
            display_color_average((u16 *)segmentation, SEGMENT_COLUMNS * SEGMENT_ROWS, RGB565);
        }
    }
}



void DMA2_Stream1_IRQHandler(void){
	// DMA complete
	if(DMA_GetITStatus(DMA2_Stream1,DMA_IT_TCIF1) != RESET){
        // Controlling the camera's gain
        DCMI_SingleRandomWrite(OV9655_DEVICE_WRITE_ADDRESS, OV9655_RED, 0x20);
        DCMI_SingleRandomWrite(OV9655_DEVICE_WRITE_ADDRESS, OV9655_GREEN, 0x20);
        DCMI_SingleRandomWrite(OV9655_DEVICE_WRITE_ADDRESS, OV9655_BLUE, 0x20);
		DMA_ClearITPendingBit(DMA2_Stream1,DMA_IT_TCIF1);
        capture_cam = true;
    }
}




