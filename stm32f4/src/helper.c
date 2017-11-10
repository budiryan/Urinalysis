#include "helper.h"
volatile u16 segmentation[SEGMENT_ROWS * SEGMENT_COLUMNS];
extern volatile u16 frame_buffer[CAMERA_ROWS * CAMERA_COLUMNS];

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
void display_color_average(u16 image[], u16 array_length, COLOR_TYPE color){
    uint64_t r = 0;
    uint64_t g = 0;
    uint64_t b = 0;
    uint64_t temp = 0;
    uint64_t overall = 0;
    char str[40];
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
            overall = r  << 11;
            overall = overall | (g << 5);
            overall = overall | b;
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
            overall = r  << 11;
            overall = overall | (g << 5);
            overall = overall | b;
        break;
    }
    
    TM_ILI9341_Puts(0, 120, "               ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
    TM_ILI9341_Puts(0, 140, "               ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
    TM_ILI9341_Puts(0, 160, "               ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
    TM_ILI9341_Puts(0, 120, "R: ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
    TM_ILI9341_Puts(0, 140, "G: ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
    TM_ILI9341_Puts(0, 160, "B: ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
    
    TM_ILI9341_Puts(20, 120, itoa(r, str, 10), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
    TM_ILI9341_Puts(20, 140, itoa(g, str, 10), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
    TM_ILI9341_Puts(20, 160, itoa(b, str, 10), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
    //FOR DEBUGGING PURPOSE:
    /*
    TM_ILI9341_Puts(20, 120, itoa(frame_buffer[1000], str, 2), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
    TM_ILI9341_Puts(20, 140, itoa(DCMI_SingleRandomRead(OV9655_DEVICE_WRITE_ADDRESS, OV9655_COM7), str, 16), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
    TM_ILI9341_Puts(20, 160, itoa(DCMI_SingleRandomRead(OV9655_DEVICE_WRITE_ADDRESS, OV9655_COM15), str, 16), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
    TM_ILI9341_Puts(20, 180, itoa(DCMI_SingleRandomRead(OV9655_DEVICE_WRITE_ADDRESS, OV9655_COM1), str, 16), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
    TM_ILI9341_Puts(20, 120, itoa(DCMI_SingleRandomRead(OV9655_DEVICE_WRITE_ADDRESS, 0x74), str, 16), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
    TM_ILI9341_Puts(60, 120, itoa(DCMI_SingleRandomRead(OV9655_DEVICE_WRITE_ADDRESS, 0x5f), str, 16), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
    TM_ILI9341_Puts(100, 120, itoa(DCMI_SingleRandomRead(OV9655_DEVICE_WRITE_ADDRESS, 0x60), str, 16), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
    TM_ILI9341_Puts(140, 120, itoa(DCMI_SingleRandomRead(OV9655_DEVICE_WRITE_ADDRESS, 0x61), str, 16), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
    TM_ILI9341_Puts(180, 120, itoa(DCMI_SingleRandomRead(OV9655_DEVICE_WRITE_ADDRESS, 0x13), str, 16), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
    TM_ILI9341_Puts(220, 120, itoa(DCMI_SingleRandomRead(OV9655_DEVICE_WRITE_ADDRESS, 0xa7), str, 16), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
    TM_ILI9341_Puts(260, 120, itoa(DCMI_SingleRandomRead(OV9655_DEVICE_WRITE_ADDRESS, OV9655_COM14), str, 16), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
    TM_ILI9341_Puts(300, 120, itoa(DCMI_SingleRandomRead(OV9655_DEVICE_WRITE_ADDRESS, OV9655_COM9), str, 16), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
    TM_ILI9341_Puts(20, 140, itoa(DCMI_SingleRandomRead(OV9655_DEVICE_WRITE_ADDRESS, 0x75), str, 16), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
    TM_ILI9341_Puts(20, 160, itoa(DCMI_SingleRandomRead(OV9655_DEVICE_WRITE_ADDRESS, OV9655_GAIN), str, 16), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
    TM_ILI9341_Puts(20, 180, itoa(DCMI_SingleRandomRead(OV9655_DEVICE_WRITE_ADDRESS, OV9655_BLUE), str, 16), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
    TM_ILI9341_Puts(20, 200, itoa(DCMI_SingleRandomRead(OV9655_DEVICE_WRITE_ADDRESS, OV9655_RED), str, 16), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
    TM_ILI9341_Puts(20, 220, itoa(DCMI_SingleRandomRead(OV9655_DEVICE_WRITE_ADDRESS, 0xa6), str, 16), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
    TM_ILI9341_Puts(60, 220, itoa(DCMI_SingleRandomRead(OV9655_DEVICE_WRITE_ADDRESS, 0x2b), str, 16), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
    TM_ILI9341_Puts(100, 220, itoa(DCMI_SingleRandomRead(OV9655_DEVICE_WRITE_ADDRESS, 0x37), str, 16), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
    TM_ILI9341_Puts(140, 220, itoa(DCMI_SingleRandomRead(OV9655_DEVICE_WRITE_ADDRESS, 0x3a), str, 16), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
    TM_ILI9341_Puts(180, 220, itoa(DCMI_SingleRandomRead(OV9655_DEVICE_WRITE_ADDRESS, 0x3d), str, 16), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
    TM_ILI9341_Puts(220, 220, itoa(DCMI_SingleRandomRead(OV9655_DEVICE_WRITE_ADDRESS, 0x3e), str, 16), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
    TM_ILI9341_Puts(260, 220, itoa(DCMI_SingleRandomRead(OV9655_DEVICE_WRITE_ADDRESS, 0x55), str, 16), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
    
    
    TM_ILI9341_Puts(0, 180, itoa((image[100] & 0xFF0000) >> 16, str, 10), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
    TM_ILI9341_Puts(0, 200, itoa((image[100] & 0x00FF00) >> 8, str, 10), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
    TM_ILI9341_Puts(0, 220, itoa((image[100] & 0x0000FF), str, 10), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
    */
    
    TM_ILI9341_Puts(0, 180, "Analyzed paper color:", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
    TM_ILI9341_DrawFilledRectangle(0, 200, 20, 220, overall);
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

void capture_one_time(void){
    DCMI_CaptureCmd(ENABLE);
    TM_ILI9341_DisplayImage((uint16_t*) frame_buffer);
    _delay_ms(500);
    DCMI_CaptureCmd(DISABLE);
}
