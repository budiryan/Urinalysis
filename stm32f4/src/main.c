#include "main.h"


static bool capture_cam = false;
extern float interpolation_score;
char str[40];
uint32_t pump_time_stamp;
URINALYSIS_PROCESS process = IDLE;

// Constant Definition
#define PUMP_DURATION 105
#define MOTOR_DURATION_US 762500
#define MINI_PUMP_DURATION 2100
#define ROTATION_COUNT 1
#define WAIT_DURATION 6000

PUMP_STATUS pump_status = PUMP_STOP;
PUMP_STATUS pump_reverse_status = PUMP_STOP;
STEPPER_STATUS stepper_status = STEPPER_STOP;
STEPPER_STATUS stepper_status_2 = STEPPER_STOP;


/*
*
* Bluetooth Listener to receive data from Android Phone / USART
*/
void receiver(const uint8_t byte) {
    // Sending back the received data example for confirmation
    uart_tx(COM3, &byte);
    uart_tx(COM3, "\r\n");
    switch(byte){
        case 'm':
            switch(stepper_status){
                case STEPPER_STOP:
                    stepper_spin(1400, STEPPER_CW, 1);
                    break;
                case STEPPER_SPIN:
                    stepper_spin(1400, STEPPER_CW, 0);
                    break;
            }
        case 'n':
            switch(stepper_status_2){
                case STEPPER_STOP:
                    stepper_spin(1400, STEPPER_CCW, 1);
                    break;
                case STEPPER_SPIN:
                    stepper_spin(1400, STEPPER_CCW, 0);
                    break;
            }   
        case 'p':
            switch(pump_status){
                case PUMP_STOP:
                    pump(50, PUMP_CCW, 1); // Sucking into the system
                    pump_status = PUMP_PUMP;
                break;
                case PUMP_PUMP:
                    pump(50, PUMP_CW, 1); // Throwing the water away
                    pump_status = PUMP_STOP;
                break;
            }
            break;
        case 's':
            stepper_spin(1000, STEPPER_CW, 0);
            pump(0, PUMP_CW, 0);
            break;
        case 'c':
            analyze_dipstick_paper();
            break;
    }

}

on_receive_listener * listener = &receiver;


int main() {
    init_system();
    TM_ILI9341_Puts(0, 0, "Urinalysis", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    TM_ILI9341_Puts(180, 0, "STATUS: ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    
    uart_tx(COM3, "Welcome to FYP Urinalysis System!\r\n");
    
    // Initialize bluetooth listener
    uart_interrupt_init(COM3, listener);
    
    while(true){
        if (capture_cam == true) {
             capture_cam = false;
             TM_ILI9341_DisplayImage((u16 *) frame_buffer);
        }
        
        switch(process){
            case PUMP_URINE:
                clear_counter();
                TM_ILI9341_Puts(180, 20, "PUMP URINE", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
                sprintf(str, "%d", get_seconds() - pump_time_stamp);
                TM_ILI9341_Puts(180, 40, str, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
                if(get_seconds() - pump_time_stamp < PUMP_DURATION)
                    pump(50, PUMP_CCW, 1);
                else{
                    // process = ROTATE_MOTOR;
                    process = ROTATE_MOTOR;
                    pump(50, PUMP_CW, 0);
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
                    pump(400, PUMP_CCW, 1);
                    stepper_spin(100, STEPPER_CW, 1);
                    delay_us(3 * MOTOR_DURATION_US);
                    // After some second, stop
                    stepper_spin(100, STEPPER_CW, 0);
                    pump(400, PUMP_CCW, 0);
                    delay_ms(MINI_PUMP_DURATION);
                }
                // wait 4 sec to let the urine flows
                pump(0, PUMP_CCW, 0);
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
                    pump(50, PUMP_CW, 1);
                else{
                    process = IDLE;
                    pump(50, PUMP_CW, 0);
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








