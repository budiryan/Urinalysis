#include "main.h"

static bool capture_cam = false;
char str[40];
uint32_t pump_time_stamp;
URINALYSIS_PROCESS process = IDLE;
#define PUMP_DURATION 96
// #define PUMP_DURATION 10
#define MOTOR_DURATION_US 763100
#define MINI_PUMP_DURATION 1600
#define ROTATION_COUNT 4

int main() {
    init_system();
    TM_ILI9341_Puts(0, 0, "Live Feed:", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    TM_ILI9341_Puts(180, 0, "STATUS: ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    TM_ILI9341_Puts(180, 20, "Idle ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    int sd_result;
    sd_result = sd_transfer_data();
    //if(sd_result == 0)
     //   TM_ILI9341_Puts(180, 60, "SD card save!", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    //else
     //   TM_ILI9341_Puts(180, 60, "SD card fail!", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    while(true){
        if (capture_cam == true) {
             capture_cam = false;
             TM_ILI9341_DisplayImage((u16 *) frame_buffer);
        }
        if(button_pressed(BUTTON_K1)){
            // Capture one time and display analysis
            while(button_pressed(BUTTON_K1));
            analyze_dipstick_paper();
        }
        if(button_pressed(BUTTON_K0)){
            process = PUMP_URINE;
            pump_time_stamp = get_seconds();
        }
        // pump(400, CW);
        
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
                    delay_us(2 * MOTOR_DURATION_US);
                    // After some second, stop
                    stepper_spin(STEPPER_CW, 0);
                    pump(400, CCW);
                    delay_ms(MINI_PUMP_DURATION);
                }
                process = PERFORM_ANALYSIS;
            break;
            case PERFORM_ANALYSIS:
                clear_counter();
                TM_ILI9341_Puts(100, 160, "             ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
                TM_ILI9341_Puts(180, 20, "ANALYZING", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
                analyze_dipstick_paper();
                process = CLEAN_PUMP;
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
                    process = ALERT_BLUETOOTH;
                    pump(0, CW);
                }
            break;
            case ALERT_BLUETOOTH:
                clear_counter();
                TM_ILI9341_Puts(180, 20, "BLUETOOTH", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
                send_bluetooth();
                process = SAVE_TO_SD_CARD;
                clear_counter();
            break;
            case SAVE_TO_SD_CARD:
                TM_ILI9341_Puts(180, 20, "SAVE SD", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
                TM_ILI9341_Puts(180, 60, "             ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
                sd_result = sd_transfer_data();
                if(sd_result == 0)
                    TM_ILI9341_Puts(180, 60, "SD card save!", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
                else
                    TM_ILI9341_Puts(180, 60, "SD card fail!", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
                process = IDLE;
                clear_counter();
            break;
            case IDLE:
                // do nothing, rofl
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








