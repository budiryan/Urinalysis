#include "main.h"

static bool capture_cam = false;
char str[40];
bool CONTINUOUS_MODE = true;
u8 switch_motor_running = 0;
u8 switch_pump_running = 1;
static volatile u8 current_time = 0;
STEPPER_DIRECTION current_motor_direction = STEPPER_CW;
PUMP_DIRECTION current_pump_direction = CW;
uint32_t pump_time_stamp;
uint32_t motor_time_stamp;
u8 count = 0;
#define PUMP_DURATION 57
#define MOTOR_DURATION_MS 1069
#define ROTATION_COUNT 8


void begin_rotate_motor(void){
}

void begin_pump(void){
    // Pump for some seconds
    pump_time_stamp = get_seconds();
    TM_ILI9341_Puts(180, 20, "           ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    TM_ILI9341_Puts(180, 20, "PUMPING", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    TM_ILI9341_Puts(180, 40, "   ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    while ((get_seconds() - pump_time_stamp) < PUMP_DURATION){
        TM_ILI9341_Puts(180, 40, itoa(get_seconds() - pump_time_stamp, str, 10), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
        pump(400, CCW);
    }
    // After some second, stop
    pump(0, CCW);
}

void clean_pump(void){
    // Pump for some seconds
    pump_time_stamp = get_seconds();
    TM_ILI9341_Puts(180, 20, "           ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    TM_ILI9341_Puts(180, 20, "CLEAN PUMP", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    TM_ILI9341_Puts(180, 40, "   ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    while ((get_seconds() - pump_time_stamp) < PUMP_DURATION){
        TM_ILI9341_Puts(180, 40, itoa(get_seconds() - pump_time_stamp, str, 10), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
        pump(400, CW); // only direction is different now...
    }
    // After some second, stop
    pump(0, CW);
}

void rotate_all_section(void){
    motor_time_stamp = get_full_ticks();
    TM_ILI9341_Puts(180, 20, "            ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    TM_ILI9341_Puts(180, 20, "ROTATING", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    pump(0, CW);
    /*
    while ((get_full_ticks() - motor_time_stamp) < MOTOR_DURATION_MS){
        // TM_ILI9341_Puts(180, 80, itoa(get_seconds(), str, 10), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
        stepper_spin(STEPPER_CW, 300);
    }
    */
    stepper_spin(STEPPER_CW, 300);
    Delayms(MOTOR_DURATION_MS);
    // After some second, stop
    stepper_spin(STEPPER_CW, 0);
}

void init(){
    SystemInit();
    led_init();			//Initiate LED
    pump_init();
    button_init();
    stepper_init();
    // Stepper motor's speed does not depend on duty cycle of the pwm
	ticks_init();		//Ticks initialization --> to get seconds etc
    TM_DELAY_Init();    // Special Library for Delays
    TM_ILI9341_Init();
    TM_ILI9341_Fill(ILI9341_COLOR_WHITE);
    TM_ILI9341_Rotate(TM_ILI9341_Orientation_Landscape_2);
    pump(0, CCW);
    stepper_spin(STEPPER_CW, 0);
    uart_init(COM1, 9600);
    uart_tx(COM1, "my name is buddy\n");
    
}

void analyze_dipstick_paper(){
    Delayms(1000);
    TM_ILI9341_DisplayImage((u16 *) frame_buffer);
    TM_ILI9341_Puts(180, 20, "Analyzing", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    display_color_average((u16 *)segmentation, 25, RGB565);
    Delayms(1500);
    TM_ILI9341_Puts(180, 20, "         ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
}


int main() {
    init();
    char c;
    /* FOR COMPLETE PIN MAPPING INFORMATION: GO TO 'doc/pin_mapping.txt'----------*/
    int camera_status = OV9655_Configuration();
    /*
    // int status = 0;
    TM_ILI9341_Puts(0, 0, "Live Feed:", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_ORANGE);
    
    TM_ILI9341_Puts(180, 0, "STATUS: ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    TM_ILI9341_Puts(180, 20, "Idle ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    float test_a = powf(9.345323, 2.1);
    DCMI_CaptureCmd(ENABLE);
    COLOR_OBJECT test = {146, 156, 50, 0};
    test.score = interpolate(test);
    sprintf(str, "%.2f", test.score);
    TM_ILI9341_Puts(0, 160, str, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    */
    // Test send some data
    TM_ILI9341_Puts(0, 0, "HELLO WORLD", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    TM_ILI9341_Puts(0, 20, "DATA IS SENT", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    while(true){
        c = uart_rx_byte(COM1);
        TM_ILI9341_Puts(0, 40, "    ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
        sprintf(str, "%c", c);
        TM_ILI9341_Puts(0, 40, str, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
        if (capture_cam == true) {
             capture_cam = false;
             TM_ILI9341_DisplayImage((u16 *) frame_buffer);
        }
        
        if(button_pressed(BUTTON_K0)){
            // Analyze the image in 1 press of a button
            while(button_pressed(BUTTON_K0));
            begin_pump();
            TM_ILI9341_Puts(180, 20, "            ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
            TM_ILI9341_Puts(180, 20, "DONE PUMPING", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
            TM_ILI9341_Puts(180, 40, "            ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
            
            // Rotate to distribute the urine evenly
            // for (int i  = 0 ; i < ROTATION_COUNT ; i++){
            //    rotate_all_section();
            //    pump(400, CCW);
            //}
            analyze_dipstick_paper();
            clean_pump();
            TM_ILI9341_Puts(180, 20, "            ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
            TM_ILI9341_Puts(180, 20, "PUMP CLEAN", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
            TM_ILI9341_Puts(180, 20, "            ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
            TM_ILI9341_Puts(180, 40, "            ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
            
        }
        
        else if(button_pressed(BUTTON_K1)){
            // Capture one time and display analysis
            while(button_pressed(BUTTON_K1));
            analyze_dipstick_paper();
        }
        
    }
}

// An interrupt used by the OV9655 camera
void DMA2_Stream1_IRQHandler(void){
	// DMA complete
	if(DMA_GetITStatus(DMA2_Stream1,DMA_IT_TCIF1) != RESET){
        // Controlling the camera's gain
		DMA_ClearITPendingBit(DMA2_Stream1,DMA_IT_TCIF1);
        capture_cam = true;
    }
}






