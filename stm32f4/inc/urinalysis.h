#ifndef _URINALYSIS_H
#define _URINALYSIS_H
#include "stm32f4xx.h"
#include "main.h"
extern volatile uint16_t frame_buffer[CAMERA_ROWS * CAMERA_COLUMNS];
extern volatile int segmentation[SEGMENT_ROWS * SEGMENT_COLUMNS];

typedef enum{
    PUMP_URINE, 
    ROTATE_MOTOR, 
    CLEAN_PUMP,
    PERFORM_ANALYSIS,
    ALERT_BLUETOOTH,
    SAVE_TO_SD_CARD,
    IDLE
}URINALYSIS_PROCESS;

void init_system(void);
char * itoa (int value, char *result, int base);
void analyze_dipstick_paper(void);
void sd_transfer_data(float interpolation_score);
void send_bluetooth(void);
void clear_counter(void);
void sd_test_init(void);
#endif
