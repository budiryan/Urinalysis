#ifndef IMU_MATH_H
#define IMU_MATH_H

#include "stm32f4xx.h"
#include "ticks.h"
#include <stdbool.h>

s16 abs_diff(s16 minuend, s16 subtrahend);
float fabs_diff(float minuend, float subtrahend);
float range_remap(float to_be_mapped);

#endif
