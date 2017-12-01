/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Templates/main.h 
  * @author  MCD Application Team
  * @version V1.6.1
  * @date    21-October-2015
  * @brief   Header for main.c module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* FOR COMPLETE PIN MAPPING INFORMATION: GO TO 'doc/pin_mapping.txt'----------*/
/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include "stm32f4xx.h"
#include "led.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_dma2d.h"
#include "usart.h"
#include "approx_math.h"
#include "ticks.h"
#include "delay.h"
#include "button.h"
#include "pump.h"
#include "stepper_motor.h"
#include "tm_stm32f4_gpio.h"
#include "tm_stm32f4_fonts.h"
#include "tm_stm32f4_dma.h"
#include "tm_stm32f4_ili9341.h"
#include "tm_stm32f4_spi.h"
#include "tm_stm32f4_spi_dma.h"
#include "tm_stm32f4_fatfs.h"
#include "I2C.h"
#include "OV9655.h"
#include "image_processing.h"
#include "usart.h"
#include "urinalysis.h"
#include <stdio.h>

extern volatile uint16_t frame_buffer[CAMERA_ROWS * CAMERA_COLUMNS];
extern volatile int segmentation[SEGMENT_ROWS * SEGMENT_COLUMNS];

void init(void);

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
