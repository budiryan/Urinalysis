#ifndef _URINALYSIS_H
#define _URINALYSIS_H
#include "stm32f4xx.h"
#include "main.h"

/*
* Contains all the helper functions + variables to assist main.c
*/

extern volatile u16 frame_buffer[CAMERA_ROWS * CAMERA_COLUMNS];
extern volatile u16 segmentation[SEGMENT_ROWS * SEGMENT_COLUMNS];

typedef enum{
    PUMP_URINE, 
    ROTATE_MOTOR, 
    CLEAN_PUMP,
    PERFORM_ANALYSIS,
    ALERT_BLUETOOTH,
    SAVE_TO_SD_CARD,
    MOVE_ONE_SECTION_CCW,
    MOVE_ONE_SECTION_CW,
    SEND_DATA,
    IDLE
}URINALYSIS_PROCESS;


typedef enum{
    USER1,
    USER2,
    USER3
}USER;

void init_system(void);
char * itoa (int value, char *result, int base);
void analyze_dipstick_paper(void);
void sd_transfer_data(float interpolation_score);
void send_bluetooth(void);
void clear_counter(void);
#endif
