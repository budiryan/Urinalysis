#include "main.h"


/*
ACKNOWLEDGEMENT:
I fully acknowledge third party sources I have used while developing the project:
- Tilen Majerle for controlling the LCD Monitor
- ChaN for the Micro SD card library
*/

static bool capture_cam = false;
extern float interpolation_score;
char str[40];
uint32_t pump_time_stamp;
URINALYSIS_PROCESS process = IDLE;
#define PUMP_DURATION 105
#define MOTOR_DURATION_US 762500
#define MINI_PUMP_DURATION 2100
#define ROTATION_COUNT 1
#define WAIT_DURATION 6000

int main() {
    init_system();
    TM_ILI9341_Puts(0, 0, "Live Feed:", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    TM_ILI9341_Puts(180, 0, "STATUS: ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    sd_test_init();
    int angle = calculate_angle(2,2,2,-2,-1,-1);
    
    while(true){
        if (capture_cam == true) {
             capture_cam = false;
             TM_ILI9341_DisplayImage((u16 *) frame_buffer);
        }
        analyze_dipstick_paper();
        if(button_pressed(BUTTON_K1)){
            // Capture one time and display analysis
            // while(button_pressed(BUTTON_K1));
            // sd_transfer_data(interpolation_score);
        }
        if(button_pressed(BUTTON_K0)){
            // process = PUMP_URINE;
            // pump_time_stamp = get_seconds();
        }
        
        switch(process){
            case PUMP_URINE:
                clear_counter();
                TM_ILI9341_Puts(180, 20, "PUMP URINE", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
                sprintf(str, "%d", get_seconds() - pump_time_stamp);
                TM_ILI9341_Puts(180, 40, str, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
                if(get_seconds() - pump_time_stamp < PUMP_DURATION)
                    pump(400, CCW);
                else{
                    // process = ROTATE_MOTOR;
                    process = ROTATE_MOTOR;
                    pump(0, CW);
                }
            break;
            case ROTATE_MOTOR:
                clear_counter();
                TM_ILI9341_Puts(180, 20, "            ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
                TM_ILI9341_Puts(180, 40, "    ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
                TM_ILI9341_Puts(180, 20, "ROTATE", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
                for(int i = 0; i < ROTATION_COUNT; i++){
                    sprintf(str, "%d", i + 1);
                    TM_ILI9341_Puts(180, 40, str, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
                    pump(0, CCW);
                    stepper_spin(STEPPER_CW, 300);
                    delay_us(3 * MOTOR_DURATION_US);
                    // After some second, stop
                    stepper_spin(STEPPER_CW, 0);
                    pump(400, CCW);
                    delay_ms(MINI_PUMP_DURATION);
                }
                // wait 4 sec to let the urine flows
                pump(0, CCW);
                TM_ILI9341_Puts(180, 20, "WAIT   ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
                delay_ms(WAIT_DURATION);
                process = PERFORM_ANALYSIS;
            break;
            case PERFORM_ANALYSIS:
                clear_counter();
                TM_ILI9341_Puts(100, 160, "             ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
                TM_ILI9341_Puts(180, 20, "ANALYZING", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
                analyze_dipstick_paper();
                process = ALERT_BLUETOOTH;
                
            break;
            case ALERT_BLUETOOTH:
                clear_counter();
                TM_ILI9341_Puts(180, 20, "BLUETOOTH", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
                send_bluetooth();
                process = SAVE_TO_SD_CARD;
            break;
            case SAVE_TO_SD_CARD:
                TM_ILI9341_Puts(180, 20, "SAVE SD", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
                sd_transfer_data(interpolation_score);
                process = CLEAN_PUMP;
                clear_counter();
                pump_time_stamp = get_seconds();
            break;
            case CLEAN_PUMP:
                clear_counter();
                TM_ILI9341_Puts(180, 20, "CLEAN PUMP", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
                sprintf(str, "%d", get_seconds() - pump_time_stamp);
                TM_ILI9341_Puts(180, 40, str, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
                if(get_seconds() - pump_time_stamp < PUMP_DURATION)
                    pump(400, CW);
                else{
                    process = IDLE;
                    pump(0, CW);
                }
            break;
            case IDLE:
                // do nothing, rofl
                clear_counter();
                TM_ILI9341_Puts(180, 20, "IDLE", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
            break;
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








