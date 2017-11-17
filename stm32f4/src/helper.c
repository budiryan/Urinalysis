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
    
    // Reference colors
    //uint16_t glucose_neg = 0xFFAF;
     uint16_t glucose_neg = 0x8645;
    // uint64_t glucose_normal = 0x8645;
    // uint64_t glucose_50 = 0x056B;
    // uint64_t glucose_150 = 0x044E;
    // uint64_t glucose_500 = 0x12EB;
    uint16_t glucose_1000 = 0x2228;
    
    uint16_t glucose_neg_r = (glucose_neg & 0xF800) >> 11;
    uint16_t glucose_neg_g = (glucose_neg & 0x7E0) >> 5;
    glucose_neg_g = glucose_neg_g / (float)64.0 * (float)32.0;
    uint16_t glucose_neg_b = glucose_neg & 0x1F;
    
    uint16_t glucose_1000_r = (glucose_1000 & 0xF800) >> 11;
    uint16_t glucose_1000_g = (glucose_1000 & 0x7E0) >> 5;
    glucose_1000_g = glucose_1000_g / (float)64.0 * (float)32.0;
    uint16_t glucose_1000_b = glucose_1000 & 0x1F;
    
    int32_t diff_neg_r;
    int32_t diff_neg_g;
    int32_t diff_neg_b;
    int32_t diff_neg;
    
    int32_t diff_1000_r;
    int32_t diff_1000_g;
    int32_t diff_1000_b;
    int32_t diff_1000;
    int32_t glucose_detected = 0;
    
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
            temp = (float)g  / (float)32.0 * (float)64.0;
            overall = overall | (temp << 5);
            overall = overall | b;
        break;
    }
    
    // Calculate the euclidean distance for negative glucose
    diff_neg_r = r - glucose_neg_r;
    diff_neg_g = g - glucose_neg_g;
    diff_neg_b = b - glucose_neg_b;
    diff_neg = Sqrt(Sqr(diff_neg_r) + Sqr(diff_neg_g) + Sqr(diff_neg_b));
    
    // Calculate the euclidean distance for positive glucose
    diff_1000_r = r - glucose_1000_r;
    diff_1000_g = g - glucose_1000_g;
    diff_1000_b = b - glucose_1000_b;
    diff_1000 = Sqrt(Sqr(diff_1000_r) + Sqr(diff_1000_g) + Sqr(diff_1000_b));
    
    TM_ILI9341_Puts(0, 140, "               ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    TM_ILI9341_Puts(0, 160, "               ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    TM_ILI9341_Puts(0, 180, "               ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    TM_ILI9341_Puts(0, 140, "R: ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    TM_ILI9341_Puts(0, 160, "G: ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    TM_ILI9341_Puts(0, 180, "B: ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    
    TM_ILI9341_Puts(20, 140, itoa(r, str, 10), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    TM_ILI9341_Puts(20, 160, itoa(g, str, 10), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    TM_ILI9341_Puts(20, 180, itoa(b, str, 10), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    
    TM_ILI9341_Puts(0, 200, "Analyzed paper color:", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    TM_ILI9341_DrawFilledRectangle(0, 220, 20, 240, overall);
    
    TM_ILI9341_Puts(180, 100, "           ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    TM_ILI9341_Puts(180, 120, "           ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    
    /*
    TM_ILI9341_Puts(180, 0, itoa(diff_neg, str, 10), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    TM_ILI9341_Puts(180, 20, itoa(diff_1000, str, 10), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    
    
    TM_ILI9341_Puts(180, 40, itoa(glucose_neg_r, str, 10), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    TM_ILI9341_Puts(180, 60, itoa(glucose_neg_g, str, 10), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    TM_ILI9341_Puts(180, 80, itoa(glucose_neg_b, str, 10), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    
    TM_ILI9341_Puts(180, 120, itoa(glucose_1000_r, str, 10), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    TM_ILI9341_Puts(180, 140, itoa(glucose_1000_g, str, 10), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    TM_ILI9341_Puts(180, 160, itoa(glucose_1000_b, str, 10), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    */
    TM_ILI9341_Puts(180, 100, "Result: ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    if (diff_neg < diff_1000){
        TM_ILI9341_Puts(180, 120, "No glucose", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    }
    else if (diff_neg > diff_1000){
        TM_ILI9341_Puts(180, 120, "glucose", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    }
    
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
