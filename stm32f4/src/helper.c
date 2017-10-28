#include "helper.h"
volatile uint16_t segmentation[SEGMENT_ROWS * SEGMENT_COLUMNS];
extern volatile uint16_t frame_buffer[CAMERA_ROWS * CAMERA_COLUMNS];

/* Converts an integer to char array */
char * itoa (int value, char *result, int base)
{
    // check that the base if valid
    if (base < 2 || base > 36) { *result = '\0'; return result; }

    char* ptr = result, *ptr1 = result, tmp_char;
    int tmp_value;

    do {
        tmp_value = value;
        value /= base;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
    } while ( value );

    // Apply negative sign
    if (tmp_value < 0) *ptr++ = '-';
    *ptr-- = '\0';
    while (ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmp_char;
    }
    return result;
}

// Displays the average of each color channel, given an image array
void display_color_average(uint16_t image[], uint16_t array_length, COLOR_TYPE color){
    uint64_t r = 0;
    uint64_t g = 0;
    uint64_t b = 0;
    uint64_t temp = 0;
    char str[20];
    switch(color){
        case RGB555:
            for(int i = 0 ; i < array_length; i++){
                r += ((image[i] & 0x7C00) >> 10);
                g += ((image[i] & 0x3E0) >> 5);
                b += (image[i] & 0x1F);
            }
            r /= ((float)array_length);
            g /= ((float)array_length);
            b /= ((float)array_length);
            
        break;
        case RGB565:
            for(int i = 0 ; i < array_length; i++){
                r += ((image[i] & 0xF800) >> 11);
                g += ((image[i] & 0x7E0) >> 5);
                b += (image[i] & 0x1F);
            }
            r /= ((float)array_length);
            temp = (float)g / (array_length);
            g = (float)temp / (float)64.0 * (float)32.0;
            b /= ((float)array_length);
        break;
    }
    TM_ILI9341_Puts(0, 120, "              ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
    TM_ILI9341_Puts(0, 140, "              ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
    TM_ILI9341_Puts(0, 160, "              ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
    TM_ILI9341_Puts(0, 120, "R: ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
    TM_ILI9341_Puts(0, 140, "G: ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
    TM_ILI9341_Puts(0, 160, "B: ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
    TM_ILI9341_Puts(20, 120, itoa((float)r, str, 10), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
    TM_ILI9341_Puts(20, 140, itoa((float)g, str, 10), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
    TM_ILI9341_Puts(20, 160, itoa((float)b, str, 10), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
}

/* Move the array from frame buffer to the segmented array, defined on the top of this file */
void capture_segment(void){
    int n_rows, n_columns, n;
    int n_segment = 0;
	for (n_rows = 0; n_rows < CAMERA_ROWS; n_rows++) {
        for (n_columns = 0; n_columns < CAMERA_COLUMNS; n_columns++){
            // Draw a target cursor
            if ((n_columns >= START_SEGMENT_COLUMNS) && (n_columns < START_SEGMENT_COLUMNS + SEGMENT_COLUMNS)
                && (n_rows >= START_SEGMENT_ROWS) && (n_rows < START_SEGMENT_ROWS + SEGMENT_ROWS)){
                segmentation[n_segment] = frame_buffer[n];
                n_segment++;    
            }
            n++;
        }  
	}
}
