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
    PUMP_URINE_COLOR,
    CLEAN_PUMP,
    PERFORM_ANALYSIS_GLUCOSE,
    PERFORM_ANALYSIS_COLOR,
    MOVE_ONE_SECTION_CW_2,
    MOVE_ONE_SECTION_CW,
    SEND_DATA,
    DO_NOTHING,
    IDLE
}URINALYSIS_PROCESS;

typedef enum{
    USER1,
    USER2,
    USER3
}USER;

void init_system(void);
char * itoa (int value, char *result, int base);
void sd_transfer_data(float interpolation_score);
void clear_counter(void);
#endif
