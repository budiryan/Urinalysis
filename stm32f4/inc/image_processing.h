#ifndef IMAGE_PROCESSING_H
#define IMAGE_PROCESSING_H

/*
* Contains all the code for image processing:
* - Camera + Segmentation array
* - Color formats (RGB + CIELAB)
* - Reference data from Dipstick paper
* - Interpolation
*/

#include "stm32f4xx.h"
#include "tm_stm32f4_ili9341.h"
#include "OV9655.h"
#include "ticks.h"
#include "approx_math.h"
#include <math.h>
#include <stdio.h>
#include "tm_stm32f4_ili9341.h"
#include "approx_math.h"

// Size of picture captured by OV9655 camera
#define CAMERA_COLUMNS          160
#define CAMERA_ROWS             120

// Point where segmentation of the camera starts
#define START_SEGMENT_ROWS      50
#define START_SEGMENT_COLUMNS   80

// Width & Height of the segmentation area
#define SEGMENT_ROWS            18
#define SEGMENT_COLUMNS         18

// Total number of pixels. Basically CAMERA_COLUMNS * CAMERA_ROWS
#define CAMERA_PIXEL            19200


typedef enum {
  RGB565, RGB555
} COLOR_TYPE;

typedef struct {
    int R;
    int G;
    int B;
    int score;
} COLOR_OBJECT;


s16 calculate_angle(float R1, float G1, float B1, float R2, float G2, float  B2);
float interpolate(COLOR_OBJECT test_data);
void convertRGBtoXYZ(int inR, int inG, int inB, float * outX, float * outY, float * outZ);
void convertXYZtoLab(float inX, float inY, float inZ, float * outL, float * outa, float * outb);
void convertLabtoXYZ( float inL, float ina, float  inb, float * outX, float * outY, float * outZ);
void convertXYZtoRGB(float inX, float inY, float inZ, int * outR, int * outG, int * outB);
float Lab_color_difference_CIE76( float inL1, float ina1, float  inb1, float inL2, float ina2, float  inb2);
float RGB_color_Lab_difference_CIE76(COLOR_OBJECT color1, COLOR_OBJECT color2);
float Lab_color_difference_CIE94( float inL1, float ina1, float  inb1, float inL2, float ina2, float  inb2);
float RGB_color_Lab_difference_CIE94( int R1, int G1, int B1, int R2, int G2, int B2);

void display_analysis(u16 image[], u16 array_length, COLOR_TYPE color);

#endif
