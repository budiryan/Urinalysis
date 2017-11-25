#include "urinalysis.h"

//Fatfs object
FATFS FatFs;
FIL fil;
FRESULT fres;
extern float interpolation_score;
extern char str[40];

void init_system(void){
    SystemInit();
    led_init();
    pump_init();
    button_init();
    stepper_init();
	ticks_init();
    delay_init();
    TM_ILI9341_Init();
    TM_ILI9341_Fill(ILI9341_COLOR_WHITE);
    TM_ILI9341_Rotate(TM_ILI9341_Orientation_Landscape_2);
    uart_init(COM3, 9600);
    OV9655_Configuration();
    DCMI_CaptureCmd(ENABLE);
}

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

void analyze_dipstick_paper(){
    delay_ms(500);
    TM_ILI9341_DisplayImage((u16 *) frame_buffer);
    TM_ILI9341_Puts(180, 20, "Analyzing", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    display_analysis((u16 *)segmentation, 25, RGB565);
    delay_ms(500);
    TM_ILI9341_Puts(180, 20, "         ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
}

int sd_transfer_data(){
    
    fres = f_mount(&FatFs, "", 1);
    sprintf(str, "SD card 1: %d", fres);
    TM_ILI9341_Puts(180, 60, str, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);   
    TM_ILI9341_Puts(180, 0, itoa(fres, str, 10), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    fres = f_open(&fil, "result4.txt", FA_OPEN_ALWAYS | FA_READ | FA_WRITE);
    sprintf(str, "SD card 2: %d", fres);
    TM_ILI9341_Puts(180, 80, str, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);   
    TM_ILI9341_Puts(180, 20, itoa(fres, str, 10), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    fres = f_write(&fil, "Budi Ryan", 10, NULL);
    sprintf(str, "SD card 3: %d", fres);
    TM_ILI9341_Puts(180, 100, str, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);   
    fres = f_close(&fil);
    sprintf(str, "SD card 4: %d", fres);
    TM_ILI9341_Puts(180, 120, str, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);    
    f_mount(0, "", 1);
    return 0;
    
    /* Open or create a log file and ready to append */
    
    /*
    f_mount(&FatFs, "", 0);
    fres = open_append(&fil, "logfile.txt");
    if (fres != FR_OK) return 1;

    // Append a line
    f_printf(&fil, "Test append line");

    // Close the file
    f_close(&fil);
    return 0;
    */
}

void send_bluetooth(){
    if (interpolation_score > 20)
        uart_tx(COM3, "ALERT! Glucose level is high! it is at %.3f mg/dL \n", interpolation_score);
    else
        uart_tx(COM3, "Glucose level is normal at: %.3f mg/dL", interpolation_score);
}

void clear_counter(){
    TM_ILI9341_Puts(180, 20, "           ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    TM_ILI9341_Puts(180, 40, "    ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
}
