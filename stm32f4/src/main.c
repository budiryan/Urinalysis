#include "main.h"

// Constant Definition
#define PUMP_DURATION 20000
#define MINI_PUMP_DURATION 6000
#define CLEAN_PUMP_DURATION 20000
const int MOTOR_DURATION_SECTION = 2800;

static bool capture_cam = false;
extern float interpolation_score;
char str[40];
int pump_time_stamp;
URINALYSIS_PROCESS process = IDLE;
int motor_time_stamp;

PUMP_STATUS pump_status = PUMP_STOP;
PUMP_STATUS pump_reverse_status = PUMP_STOP;
STEPPER_STATUS stepper_status = STEPPER_STOP;
STEPPER_STATUS stepper_status_2 = STEPPER_STOP;

// For test result
USER current_user = USER1;
float glucose_score = 2.345;
char glucose_score_string[5];
float color_score = 3.45;
char color_score_string[5];


/*
*
* Bluetooth Listener to receive data / send data from USART
*/
void receiver(const uint8_t byte) {
    // Sending back the received data example for confirmation
    switch(byte){
        case 'n':
            uart_tx(COM3, "Motor spinning CCW\r\n");
            switch(stepper_status_2){
                case STEPPER_STOP:
                    stepper_status_2 = STEPPER_SPIN;
                    stepper_spin(1400, STEPPER_CCW, 1);
                    break;
                case STEPPER_SPIN:
                    stepper_status_2 = STEPPER_STOP;
                    stepper_spin(1400, STEPPER_CCW, 0);
                    break;
            }
            break;
        case 'm':
            uart_tx(COM3, "Motor spinning CW\r\n");
            switch(stepper_status){
                case STEPPER_STOP:
                    stepper_status = STEPPER_SPIN;
                    stepper_spin(1400, STEPPER_CW, 1);
                    break;
                case STEPPER_SPIN:
                    stepper_status = STEPPER_STOP;
                    stepper_spin(1400, STEPPER_CW, 0);
                    break;
            }
            break;
        case 'v':
            // Move stepper motor one section CCW
            uart_tx(COM3, "Motor move one section CCW\r\n");
            motor_time_stamp = get_full_ticks();
            stepper_spin(1400, STEPPER_CCW, 1);
            process = MOVE_ONE_SECTION_CCW;
            break;
        case 'b':
            // Move stepper motor one section CW
            uart_tx(COM3, "Motor move one section CW\r\n");
            motor_time_stamp = get_full_ticks();
            stepper_spin(1400, STEPPER_CW, 1);
            process = MOVE_ONE_SECTION_CW;
            break;
        case 'p':
            switch(pump_status){
                case PUMP_STOP:
                    uart_tx(COM3, " Pump sucking \r\n");
                    pump(50, PUMP_CCW, 1); // Sucking into the system
                    pump_status = PUMP_PUMP;
                break;
                case PUMP_PUMP:
                    uart_tx(COM3, " Pump release \r \n");
                    pump(50, PUMP_CW, 1); // Throwing the water away
                    pump_status = PUMP_STOP;
                break;
            }
            break;
        case 's':
            uart_tx(COM3, "Stopping the whole system \r\n");
            stepper_spin(1000, STEPPER_CW, 0);
            pump(0, PUMP_CW, 0);
            break;
        case 'c':
            uart_tx(COM3, "Analyzing segmented area \r\n");
            display_color_info((u16 *)segmentation, SEGMENT_ROWS * SEGMENT_COLUMNS, RGB565);
            display_analysis(GLUCOSE);
            break;
    }

}

on_receive_listener * listener = &receiver;


int main() {
    /*
    Data format to be sent through Bluetooth:
    Flag to trigger | user | glucose value | urine value 
    */
    
    init_system();
    // Initialize bluetooth listener
    uart_interrupt_init(COM3, listener);
    uart_tx(COM3, "Welcome to FYP Urinalysis System!\r\n");
    
    while(true){
        if (capture_cam == true) {
            capture_cam = false;
            TM_ILI9341_DisplayImage((u16 *) frame_buffer);
            display_color_info((u16 *)segmentation, SEGMENT_ROWS * SEGMENT_COLUMNS, RGB565);
        }
        
        if(button_pressed(BUTTON_0)){
            current_user = USER1;
            process = PUMP_URINE;
            pump_time_stamp = get_full_ticks();
        }

        if(button_pressed(BUTTON_1)){
            current_user = USER2;
            process = PUMP_URINE;
            pump_time_stamp = get_full_ticks();
        }
        
        if(button_pressed(BUTTON_2)){
            current_user = USER3;
            process = PUMP_URINE;
            pump_time_stamp = get_full_ticks();
        }

        switch(process){
            case PUMP_URINE:
                clear_counter();
                TM_ILI9341_Puts(180, 20, "ANALYZE", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
                TM_ILI9341_Puts(180, 40, "GLUCOSE", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
                sprintf(str, "%d", (get_full_ticks() - pump_time_stamp) / 1000);
                TM_ILI9341_Puts(180, 60, str, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
                if(get_full_ticks() - pump_time_stamp < PUMP_DURATION)
                    pump(50, PUMP_CCW, 1);
                else{
                    pump(50, PUMP_CW, 0);
                    process = PERFORM_ANALYSIS_GLUCOSE;
                }
                break;
            case PERFORM_ANALYSIS_GLUCOSE:
                clear_counter();
                display_color_info((u16 *)segmentation, SEGMENT_ROWS * SEGMENT_COLUMNS, RGB565);
                glucose_score = display_analysis(GLUCOSE);
                motor_time_stamp = get_full_ticks();
                process = MOVE_ONE_SECTION_CW;
                break;
            case MOVE_ONE_SECTION_CCW:
                if((get_full_ticks() - motor_time_stamp) > MOTOR_DURATION_SECTION){
                    stepper_spin(1000, STEPPER_CCW, 0);
                    process = IDLE;
                }
                else{
                    stepper_spin(500, STEPPER_CCW, 1);
                }
                break;
            case MOVE_ONE_SECTION_CW:
                if((get_full_ticks() - motor_time_stamp) > MOTOR_DURATION_SECTION){
                    stepper_spin(1000, STEPPER_CW, 0);
                    pump_time_stamp = get_full_ticks();
                    process = PUMP_URINE_COLOR;
                }
                else{
                    stepper_spin(500, STEPPER_CW, 1);
                }
                break;
            case PUMP_URINE_COLOR:
                clear_counter();
                TM_ILI9341_Puts(180, 20, "ANALYZE", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
                TM_ILI9341_Puts(180, 40, "COLOR", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
                sprintf(str, "%d", (get_full_ticks() - pump_time_stamp) / 1000);
                TM_ILI9341_Puts(180, 60, str, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
                if(get_full_ticks() - pump_time_stamp < MINI_PUMP_DURATION)
                    pump(50, PUMP_CCW, 1);
                else{
                    motor_time_stamp = get_full_ticks();
                    pump(50, PUMP_CW, 0);
                    process = PERFORM_ANALYSIS_COLOR;
                }
                break;
            case PERFORM_ANALYSIS_COLOR:
                clear_counter();
                display_color_info((u16 *)segmentation, SEGMENT_ROWS * SEGMENT_COLUMNS, RGB565);
                color_score = display_analysis(COLOR);
                pump_time_stamp = get_full_ticks();
                process = CLEAN_PUMP;
                break;
            case CLEAN_PUMP:
                clear_counter();
                TM_ILI9341_Puts(180, 20, "CLEAN PUMP", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
                sprintf(str, "%d", (get_full_ticks() - pump_time_stamp) / 1000);
                TM_ILI9341_Puts(180, 60, str, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
                if(get_full_ticks() - pump_time_stamp < PUMP_DURATION)
                    pump(50, PUMP_CW, 1);
                else{
                    process = SEND_DATA;
                    pump(50, PUMP_CW, 0);
                }
                break;
            case SEND_DATA:
                TM_ILI9341_Puts(180, 20, "SENDING DATA", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
                strcpy(str, "Urine");
                strcat(str, "|");
                switch(current_user){
                    case USER1:
                        strcat(str, "0");
                        break;
                    case USER2:
                        strcat(str, "1");
                        break;
                    case USER3:
                        strcat(str, "2");
                        break;
                }
                strcat(str, "|");
                sprintf(glucose_score_string, "%.2f", glucose_score);
                strcat(str, glucose_score_string);
                strcat(str, "|");
                sprintf(color_score_string, "%.2f", color_score);
                strcat(str, color_score_string);
                uart_tx(COM3, "       ");
                uart_tx(COM3, str);
                process = IDLE;
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
