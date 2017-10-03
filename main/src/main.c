#include "main.h"

#define I2C_Port GPIOB
#define I2C_SDA  GPIO_Pin_9
#define I2C_SCL  GPIO_Pin_8
#define DCMI_DR_ADDRESS     0x50050028
#define FSMC_LCD_ADDRESS    0x60020000

u8 switch_motor_running = 0;
u8 switch_pump_running = 1;
STEPPER_DIRECTION current_motor_direction = STEPPER_CW;
PUMP_DIRECTION current_pump_direction = CW;


void Soft_I2C_Configuration(void){
  GPIO_InitTypeDef GPIO_InitStructure;
	/* Enable GPIOB clocks */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	/*SDA Pin Configuration*/
    GPIO_InitStructure.GPIO_Pin = I2C_SDA; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(I2C_Port, &GPIO_InitStructure);
	/*SCL Pin Configuration*/
	GPIO_InitStructure.GPIO_Pin = I2C_SCL; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(I2C_Port, &GPIO_InitStructure);
}

/**
  * @brief  Configures the MCO Module
  * @param  None
  * @retval None
  */
void OV9655_MCO_Configuration(void){
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource9, GPIO_AF_MCO);
  
  RCC_PLLI2SCmd(DISABLE);
  
  RCC_PLLI2SConfig(240, 5); // 192..432, 2..7, ie range of 27.429 Mhz to 192 MHz
  /* PLLI2SSCLK = 240 / 5 = 48Mhz */
	/* MCO2 Output Freq = 48 / 2 = 24Mhz*/
  RCC_MCO2Config(RCC_MCO2Source_PLLI2SCLK, RCC_MCO2Div_2); // 24 MHz with default PLL fComp
  
  RCC_PLLI2SCmd(ENABLE);
  
  while(RCC_GetFlagStatus(RCC_FLAG_PLLI2SRDY) == RESET);
}

/**
  * @brief  Configures the DCMI to interface with the OV9655 camera module.
  * @param  None
  * @retval None
  */
void OV9655_DCMI_Configuration(void)
{
  DCMI_InitTypeDef DCMI_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  DMA_InitTypeDef  DMA_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* Enable DCMI GPIOs clocks */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOE | 
                         RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOA, ENABLE); 

  /* Enable DCMI clock */
  RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_DCMI, ENABLE);

  /* Connect DCMI pins to AF13 ************************************************/
  /* PCLK */
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_DCMI);
  /* D0-D7 */
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_DCMI);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_DCMI);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource0, GPIO_AF_DCMI);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource1, GPIO_AF_DCMI);
  // GPIO_PinAFConfig(GPIOE, GPIO_PinSource4, GPIO_AF_DCMI);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_DCMI);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_DCMI);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource5, GPIO_AF_DCMI);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource6, GPIO_AF_DCMI);
  /* VSYNC */
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_DCMI);
  /* HSYNC */
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource4, GPIO_AF_DCMI);
  
  /* DCMI GPIO configuration **************************************************/
  /* D0 D1(PC6/7)  --> D0 D4(PC6/7/11) */ 
  // GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;  
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  /* D2..D4(PE0/1/4) D6/D7(PE5/6) */
  //GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 
  //	                              | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 
	                            | GPIO_Pin_5 | GPIO_Pin_6;
  GPIO_Init(GPIOE, &GPIO_InitStructure);

  /* D5(PB6), VSYNC(PB7) */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /* PCLK(PA6) HSYNC(PA4)*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  /* DCMI configuration *******************************************************/ 
  DCMI_InitStructure.DCMI_CaptureMode = DCMI_CaptureMode_Continuous;
  DCMI_InitStructure.DCMI_SynchroMode = DCMI_SynchroMode_Hardware;
  DCMI_InitStructure.DCMI_PCKPolarity = DCMI_PCKPolarity_Rising;
  DCMI_InitStructure.DCMI_VSPolarity = DCMI_VSPolarity_High;
  DCMI_InitStructure.DCMI_HSPolarity = DCMI_HSPolarity_High;
  DCMI_InitStructure.DCMI_CaptureRate = DCMI_CaptureRate_All_Frame;
  DCMI_InitStructure.DCMI_ExtendedDataMode = DCMI_ExtendedDataMode_8b;
  
  DCMI_Init(&DCMI_InitStructure);

  /* DCMI Interrupts config ***************************************************/
  //DCMI_ITConfig(DCMI_IT_VSYNC, ENABLE);
  //DCMI_ITConfig(DCMI_IT_LINE, ENABLE);
  DCMI_ITConfig(DCMI_IT_FRAME, ENABLE);
  //DCMI_ITConfig(DCMI_IT_ERR, ENABLE);
      
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  NVIC_InitStructure.NVIC_IRQChannel = DCMI_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
  NVIC_Init(&NVIC_InitStructure);
	
  /* Configures the DMA2 to transfer Data from DCMI to the LCD ****************/
  /* Enable DMA2 clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);  
  
  /* DMA2 Stream1 Configuration */  
  DMA_DeInit(DMA2_Stream1);

  
  DMA_InitStructure.DMA_Channel = DMA_Channel_1;  
  DMA_InitStructure.DMA_PeripheralBaseAddr = DCMI_DR_ADDRESS;	
  DMA_InitStructure.DMA_Memory0BaseAddr = FSMC_LCD_ADDRESS;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
  DMA_InitStructure.DMA_BufferSize = 1;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;         
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
     
  DMA_Init(DMA2_Stream1, &DMA_InitStructure);
  
}


/**
  * @brief  Configures the OV9655 DCMI
  * @param  None
  * @retval None
  */
uint8_t OV9655_Configuration(void){
	int i=0;
	OV9655_IDTypeDef OV9655_ID;
	/*OV9655 MCO Configuration*/
	OV9655_MCO_Configuration();
	/* I2C configuration */
    Soft_I2C_Configuration();

    for(i=0;i<10000;i++){}
    
        /* Reset and check the presence of the OV9655 camera module */
    if (DCMI_SingleRandomWrite(OV9655_DEVICE_WRITE_ADDRESS,0x12, 0x80)==0xFF)
    {
     return (0xFF); //Camera Failed!
    }
    /* Camera Ok!*/
    for(i=0;i<10000;i++){}

    DCMI_OV9655_ReadID(&OV9655_ID);	
        
    /* OV9655 Camera size setup */    
    #if defined (QQVGA_SIZE)
    DCMI_OV9655_QQVGASizeSetup();
    #elif defined (QVGA_SIZE)
    DCMI_OV9655_QVGASizeSetup();
    #endif 
    /* Set the RGB565 mode */
    DCMI_SingleRandomWrite(OV9655_DEVICE_WRITE_ADDRESS, OV9655_COM7, 0x63);
    DCMI_SingleRandomWrite(OV9655_DEVICE_WRITE_ADDRESS, OV9655_COM15, 0xd0);

    /* Invert the HRef signal*/
    DCMI_SingleRandomWrite(OV9655_DEVICE_WRITE_ADDRESS, OV9655_COM10, 0x08);
    /* OV9655 Camera DCMI setup */
    OV9655_DCMI_Configuration();	

    return (0x00);
}


/*Used for Debugging Purposes --> Converts an integer to char array*/
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

int main() {
    u8 current_time;
    char str[12];
    led_init();			//Initiate LED
    pump_init();
    stepper_init();
    // Stepper motor's speed does not depend on duty cycle of the pwm
    stepper_spin(STEPPER_CW, 10);
	ticks_init();		//Ticks initialization
    TM_ILI9341_Init();
    TM_ILI9341_Fill(ILI9341_COLOR_MAGENTA);
    TM_ILI9341_Rotate(TM_ILI9341_Orientation_Landscape_2);
    button_init();
    
    
    // Test if camera is working properly, if you cannot get through the while loop, camera init failed!
    if (OV9655_Configuration() == 0x00){ // Call for camera initialization
        TM_ILI9341_Puts(100, 100, "Camera init success!!", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
    }
    else {    
        //while(true){
        //    TM_ILI9341_Puts(100, 100, "Camera init failed", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
        //}
    }
    
    
    while(true){
        TM_ILI9341_Puts(100, 100, itoa(get_seconds(), str, 10), &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
        if(button_pressed(BUTTON_K0)){
            current_time = get_full_ticks();
            while(button_pressed(BUTTON_K0));
                // CHANGE BOTH DIRECTION AND SPEED
             switch(current_motor_direction){
                case STEPPER_CW:
                    stepper_spin(current_motor_direction, 200);
                    current_motor_direction = STEPPER_CCW;
                    break;
                case STEPPER_CCW:
                    stepper_spin(current_motor_direction, 0);
                    current_motor_direction = STEPPER_CW;
                    break;
            }
        }
        if(button_pressed(BUTTON_K1)){
            while(button_pressed(BUTTON_K1));
            // TURN PUMP ON / OFF
            switch(switch_pump_running){
                case 0:
                    pump(400, CW);
                    LED_ON(LED_2);
                    switch_pump_running = 1;
                    break;
                case 1:
                    pump(0, CW);
                    LED_OFF(LED_2);
                    switch_pump_running = 0;
                    break;
            }
        }
    }
} 



