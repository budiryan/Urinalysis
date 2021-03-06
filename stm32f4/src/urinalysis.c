#include "urinalysis.h"

// Extern variables from other files
extern float interpolation_score;
extern char str[40];


//Fatfs objects for MicroSD card
FATFS FatFs;
FIL fil;
FRESULT fres;

/***************************************************
 *  Returns: Nothing
 *
 *  Parameters:  Nothing
 *
 *  Description: Inits all devices on the system
 *  
 *  List of devices: LED, pump, button, stepper motor,
 *                   ticks, delay, LCD monitor, Bluetooth,
 *                   Camera
 *
 ***************************************************/
void init_system(void){
    SystemInit();
    pump_init();
    stepper_init();
    button_init();
    ticks_init();
    delay_init();
    TM_ILI9341_Init();
    TM_ILI9341_Fill(ILI9341_COLOR_WHITE);
    TM_ILI9341_Rotate(TM_ILI9341_Orientation_Landscape_1);
    TM_ILI9341_Puts(0, 0, "Urinalysis", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    TM_ILI9341_Puts(180, 0, "STATUS: ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    uart_init(COM3, 9600);
    OV9655_Configuration();
    DCMI_CaptureCmd(ENABLE);
}


/***************************************************
 *  Returns: FRESULT Object 
 *
 *  Parameters:  Address of variable fil (&fil), File name
 *
 *  Description: Opens a file stream so texts can be appended to the file
 *  
 *  List of devices: LED, pump, button, stepper motor,
 *                   ticks, delay, LCD monitor, Bluetooth,
 *                   Camera
 *
 ***************************************************/
FRESULT open_append (FIL* fp, const char* path)
{
    FRESULT fr;

    // Opens an existing file. If not exist, creates a new file.
    fr = f_open(fp, path, FA_WRITE | FA_OPEN_ALWAYS);
    if (fr == FR_OK) {
        // Seek to end of the file to append data
        fr = f_lseek(fp, f_size(fp));
        if (fr != FR_OK)
            f_close(fp);
    }
    return fr;
}

/* Converts an integer to char array, similar to sprintf() */
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

// Transfers interpolation score to SD card
void sd_transfer_data(float interpolation_score){
    TM_ILI9341_Puts(180, 200, "                ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    fres = f_mount(&FatFs, "", 1);
    TM_ILI9341_Puts(180, 200, "SD:", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    TM_ILI9341_Puts(210, 200, itoa(fres, str, 10), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    
    // Open or create a log file and ready to append
    fres = open_append(&fil, "result.txt");
    TM_ILI9341_Puts(225, 200, itoa(fres, str, 10), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);

    // Append a line
    sprintf(str, "Glucose: %.3f mg/dL\n", interpolation_score);
    f_printf(&fil, str);
    TM_ILI9341_Puts(240, 200, itoa(fres, str, 10), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    
    // Close the file
    fres = f_close(&fil);
    TM_ILI9341_Puts(255, 200, itoa(fres, str, 10), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);

    fres = f_mount(0, "", 1);
}

// Utility function to clear certain area of the LCD screen
void clear_counter(){
    TM_ILI9341_Puts(180, 20, "            ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    TM_ILI9341_Puts(180, 40, "            ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    TM_ILI9341_Puts(180, 60, "            ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
}
