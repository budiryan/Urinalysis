#ifndef _ARDU_IMU_H
#define _ARDU_IMU_H

#include "stm32f4xx.h"
#include "usart.h"
#include <stdbool.h>
#include "imu_math.h"

#define ARDU_IMU_UART COM2

typedef union {
	u8 chars[4];
	float f;
} byte2float;

extern bool ardu_imu_synced;
extern bool ardu_imu_staged;
extern bool ardu_imu_pre_staged;
extern bool ardu_imu_calibrated;
extern float ardu_out_ypr[3]; 
extern float ardu_cal_ypr[3]; 

#define IMU_USE_CONTINUOUS_MODE
#define SYNC_TIMEOUT 200
#define SAMPLE_SIZE 75

void ardu_imu_init(void);
void ardu_imu_value_update(void);

#endif
