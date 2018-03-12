#include "main.h"

// Constant Definition
#define PUMP_DURATION 105
#define MOTOR_DURATION_US 762500
#define MINI_PUMP_DURATION 2100
#define ROTATION_COUNT 1
#define WAIT_DURATION 6000
const int MOTOR_DURATION_SECTION = 550;

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
USER current_user = USER1;


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
            analyze_dipstick_paper();
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
        }
        sprintf(str, "%d %d %d", button_pressed(BUTTON_0), button_pressed(BUTTON_1), button_pressed(BUTTON_2));
        
        // User presses one of the button depending on who they are
        if(button_pressed(BUTTON_0)){
            current_user = USER1;
            process = SEND_DATA;
        }
        
        if(button_pressed(BUTTON_1)){
            current_user = USER2;
            process = SEND_DATA;
        }
        
        if(button_pressed(BUTTON_2)){
            current_user = USER3;
            process = SEND_DATA;
        }
        
        TM_ILI9341_Puts(180, 80, str, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
        
        
        switch(process){
            case MOVE_ONE_SECTION_CCW:
                if((get_full_ticks() - motor_time_stamp) > MOTOR_DURATION_SECTION){
                    stepper_spin(1400, STEPPER_CCW, 0);
                    process = IDLE;
                }
                break;
            case MOVE_ONE_SECTION_CW:
                if((get_full_ticks() - motor_time_stamp) > MOTOR_DURATION_SECTION){
                    stepper_spin(1400, STEPPER_CW, 0);
                    process = IDLE;
                }
                break;
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
            case SEND_DATA:
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
                strcat(str, "300");
                strcat(str, "|");
                strcat(str, "8");
                uart_tx(COM3, str);
                process = IDLE;
                break;
            case IDLE:
                // do nothing, rofl
                clear_counter();
                TM_ILI9341_Puts(180, 20, "IDLE", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
                if(get_full_ticks() % 10 == 0)
                    uart_tx(COM3, "Connected, Waiting for data!");
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








