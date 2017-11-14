#ifndef HELPER_H
#define HELPER_H

#include "stm32f4xx.h"
#include "tm_stm32f4_ili9341.h"
#include "OV9655.h"
#include "ticks.h"

#define CAMERA_COLUMNS          160
#define CAMERA_ROWS             120
#define START_SEGMENT_ROWS      61
#define START_SEGMENT_COLUMNS   57
#define SEGMENT_ROWS            5
#define SEGMENT_COLUMNS         5
#define CAMERA_PIXEL            19200


typedef enum {
  RGB565, RGB555
} COLOR_TYPE;

char * itoa (int value, char *result, int base);
void capture_segment(void);
void display_color_average(u16 image[], u16 array_length, COLOR_TYPE color);
void capture_one_time(void);

#endif
