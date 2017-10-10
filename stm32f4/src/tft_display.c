#include "tft_display.h"

//private data
u16 curr_bg_color = BLACK;
u16 curr_text_color = BLACK;
u16 curr_text_color_sp = BLACK;

u8 tft_orientation = 0, tft_enabled = 1;


char text[CHAR_MAX_X][CHAR_MAX_Y];
char text_prev[CHAR_MAX_X][CHAR_MAX_Y];
u16 text_color[CHAR_MAX_X][CHAR_MAX_Y];
u16 text_color_prev[CHAR_MAX_X][CHAR_MAX_Y];
u16 bg_color[CHAR_MAX_X][CHAR_MAX_Y];
u16 bg_color_prev[CHAR_MAX_X][CHAR_MAX_Y];
u8 text_bg_color_prev[CHAR_MAX_X][CHAR_MAX_Y];/*for transmit for xbc, msb 4bits: text color, lsb 4bits: bg color*/

u16 print_pos = 0;

u8 prints_enabled = 1;

void tft_prints_enable(u8 i) 
{
	prints_enabled = i;
}


/**
  * @brief  Initialization of SPI for TFT
  * @param  None
  * @retval None
  */
void tft_spi_init(void)
{
   SPI_InitTypeDef   	SPI_InitStructure;
   GPIO_InitTypeDef 	GPIO_InitStructure;

   /* Enable TFT_SPI and GPIO clocks */
   //RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1 | RCC_APB2Periph_GPIOA, ENABLE);
	
   //Clock and Pins mapping to SPI2
   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);
   RCC_APB1PeriphClockLPModeCmd(RCC_APB1Periph_SPI2,ENABLE);
   GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_SPI2);
	 //GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_SPI2);
   GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_SPI2);
	
   /* Enable GPIOD for RST pin */
   GPIO_InitStructure.GPIO_Pin = TFT_RST_PIN;
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
   GPIO_InitStructure.GPIO_Speed = GPIO_Medium_Speed;
   GPIO_Init(TFT_RST_PORT, &GPIO_InitStructure);

   /* Enable GPIOB for DC Pin */
   GPIO_InitStructure.GPIO_Pin = TFT_DC_PIN;
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
   GPIO_InitStructure.GPIO_Speed = GPIO_Medium_Speed;
   GPIO_Init(TFT_DC_PORT, &GPIO_InitStructure);

   /* Configure TFT_SPI Pin: SCK, MISO and MOSI */
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 |GPIO_Pin_15;
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
   GPIO_InitStructure.GPIO_Speed = GPIO_Fast_Speed;
   GPIO_Init(GPIOB, &GPIO_InitStructure);

   /* Configure TFT_SPI Pin: CS */
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_CS;
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
   GPIO_InitStructure.GPIO_Speed = GPIO_Fast_Speed;
   GPIO_Init(GPIO_CS, &GPIO_InitStructure);
	 

   /* TFT_SPI configuration */
   SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
   SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
   SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
   SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
   SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
   SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
   SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
   SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB; 
   SPI_Init(TFT_SPI, &SPI_InitStructure);
	 
   /* Enable TFT_SPI   */
   SPI_Cmd(TFT_SPI, ENABLE);
   SPI_CalculateCRC( TFT_SPI , DISABLE );		// Disable the CRC checking
   SPI_SSOutputCmd( TFT_SPI , DISABLE );
}

/**
  * @brief  Sending a command
  * @param  command: one byte command to be sent
  * @retval None
  */
void tft_write_command(u8 command)
{
	GPIO_ResetBits(GPIO_CS, GPIO_Pin_CS);
	GPIO_ResetBits(TFT_DC_PORT, TFT_DC_PIN);
	/* Loop while DR register in not emplty */
   while (SPI_I2S_GetFlagStatus(TFT_SPI, SPI_I2S_FLAG_TXE) == RESET);
   /* Send byte through the TFT_SPI peripheral */
   SPI_I2S_SendData(TFT_SPI, command);
   while (SPI_I2S_GetFlagStatus(TFT_SPI, SPI_I2S_FLAG_RXNE) == RESET);
   SPI_I2S_ReceiveData(TFT_SPI);

   GPIO_SetBits(GPIO_CS, GPIO_Pin_CS);
}

/**
  * @brief  Sending a data
  * @param  data: one byte data to be sent
  * @retval None
  */
void tft_write_data(u8 data)
{
	GPIO_ResetBits(GPIO_CS, GPIO_Pin_CS);
	GPIO_SetBits(TFT_DC_PORT, TFT_DC_PIN);

   /* Loop while DR register in not emplty */
   while (SPI_I2S_GetFlagStatus(TFT_SPI, SPI_I2S_FLAG_TXE) == RESET);
   /* Send byte through the TFT_SPI peripheral */
   SPI_I2S_SendData(TFT_SPI, data);
   while (SPI_I2S_GetFlagStatus(TFT_SPI, SPI_I2S_FLAG_RXNE) == RESET);
   SPI_I2S_ReceiveData(TFT_SPI);

   GPIO_SetBits(GPIO_CS, GPIO_Pin_CS);
}

/**
  * @brief  Configuration of TFT
  * @param  None
  * @retval None
  */
void tft_config(void)
{
	tft_write_command(0x01);   //Sofeware setting
	//_delay_ms(0);
	tft_write_command(0x11);//Sleep out
	//_delay_ms(120);
	
	//ST7735R Frame Rate
	tft_write_command(0xB1);
	tft_write_data(0x01);
	tft_write_data(0x2C);
	tft_write_data(0x2D);
	tft_write_command(0xB2);
	tft_write_data(0x01);
	tft_write_data(0x2C);
	tft_write_data(0x2D);
	tft_write_command(0xB3);
	tft_write_data(0x01);
	tft_write_data(0x2C);
	tft_write_data(0x2D);
	tft_write_data(0x01);
	tft_write_data(0x2C);
	tft_write_data(0x2D);
	//------------------------------------End ST7735R Frame Rate-----------------------------------------//
	tft_write_command(0xB4);//Column inversion
	tft_write_data(0x07);
	//------------------------------------ST7735R Power Sequence-----------------------------------------//
	tft_write_command(0xC0);
	tft_write_data(0xA2);
	tft_write_data(0x02);
	tft_write_data(0x84);
	tft_write_command(0xC1);
	tft_write_data(0xC5);
	tft_write_command(0xC2);
	tft_write_data(0x0A);
	tft_write_data(0x00);
	tft_write_command(0xC3);
	tft_write_data(0x8A);
	tft_write_data(0x2A);
	tft_write_command(0xC4);
	tft_write_data(0x8A);
	tft_write_data(0xEE);
	//---------------------------------End ST7735R Power Sequence-------------------------------------//
	tft_write_command(0xC5);//VCOM
	tft_write_data(0x0E);
	tft_write_command(0x36);//MX, MY, RGB mode
	tft_write_data(0xC8);
	//------------------------------------ST7735R Gamma Sequence-----------------------------------------//
	tft_write_command(0xe0);
	tft_write_data(0x02);
	tft_write_data(0x1c);
	tft_write_data(0x07);
	tft_write_data(0x12);
	tft_write_data(0x37);
	tft_write_data(0x32);
	tft_write_data(0x29);
	tft_write_data(0x2d);
	tft_write_data(0x29);
	tft_write_data(0x25);
	tft_write_data(0x2b);
	tft_write_data(0x39);
	tft_write_data(0x00);
	tft_write_data(0x01);
	tft_write_data(0x03);
	tft_write_data(0x10);
	tft_write_command(0xe1);
	tft_write_data(0x03);
	tft_write_data(0x1d);
	tft_write_data(0x07);
	tft_write_data(0x06);
	tft_write_data(0x2e);
	tft_write_data(0x2c);
	tft_write_data(0x29);
	tft_write_data(0x2d);
	tft_write_data(0x2e);
	tft_write_data(0x2e);
	tft_write_data(0x37);
	tft_write_data(0x3f);
	tft_write_data(0x00);
	tft_write_data(0x00);
	tft_write_data(0x02);
	tft_write_data(0x10);
	tft_write_command(0x2A);
	tft_write_data(0x00);
	tft_write_data(0x00);
	tft_write_data(0x00);
	tft_write_data(0x7f);

	tft_write_command(0x2B);
	tft_write_data(0x00);
	tft_write_data(0x00);
	tft_write_data(0x00);
	tft_write_data(0x9f);
	//------------------------------------End ST7735R Gamma Sequence-----------------------------------------//

	tft_write_command(0x3A);
	tft_write_data(0x05);  
	tft_write_command(0x29);//Display on

//	_delay_ms(10);
}

/**
  * @brief  Hardware reset for TFT
  * @param  None
  * @retval None
  */
void tft_reset(void)
{
 	GPIO_ResetBits(TFT_RST_PORT, TFT_RST_PIN);
	_delay_ms(1);
	GPIO_SetBits(TFT_RST_PORT, TFT_RST_PIN);
	_delay_ms(1);
}

/**
  * @brief  Initialization of TFT
  * @param  orientation: default orientation (0, 1, 2, 3)
  * @param  in_bg_color: default background color
  * @param  in_text_color: default text color
  * @param  in_text_color_sp: default special text color
  * @retval None
  */
void tft_init(u8 orientation, u16 in_bg_color, u16 in_text_color, u16 in_text_color_sp)
{
	u8 x, y;
	tft_spi_init();
	tft_reset();
	tft_config();
    tft_write_command(0x2C);
	tft_set_bg_color(in_bg_color);
	tft_set_text_color(in_text_color);
	tft_set_special_color(in_text_color_sp);
	tft_fill_color(in_bg_color);
	tft_orientation = orientation;
	

	for (x = 0; x < CHAR_MAX_X; x++) {
		for (y = 0; y < CHAR_MAX_Y; y++) {
			text[x][y] = ' ';
			text_color[x][y] = in_text_color;
			bg_color[x][y] = in_bg_color;

			text_prev[x][y] = ' ';
			text_color_prev[x][y] = in_text_color;
			bg_color_prev[x][y] = in_bg_color;
		}
	}
}

/**
  * @brief  Enable using TFT
  * @param  None
  * @retval None
  */
void tft_enable(void)
{
	tft_enabled = 1;
}

/**
  * @brief  Disable using TFT
  * @param  None
  * @retval None
  */
void tft_disable(void)
{
	tft_enabled = 0;
}

/**
  * @brief  Set the current background color in RGB565
  * @param  None
  * @retval None
  */
void tft_set_bg_color(u16 in_bg_color)
{
	curr_bg_color = in_bg_color;
}

/**
	* @brief Get the current background color
	* @param None
	* @retval Current background color in RGB565
	*/
u16 tft_get_bg_color(void)
{
	return curr_bg_color;
}

/**
  * @brief  Set the current text color
  * @param  None
  * @retval None
  */
void tft_set_text_color(u16 in_text_color)
{
	curr_text_color = in_text_color;
}

/**
	* @brief Set the current text color in RGB565
	* @param None
	* @retval Current text color in RGB565
	*/
u16 tft_get_text_color(void)
{
	return curr_text_color;
}


/**
  * @brief  Set the current special text color
  * @param  None
  * @retval None
  */
void tft_set_special_color(u16 text_color_sp)
{
	curr_text_color_sp = text_color_sp;
}

/**
	* @brief Set the current text color in RGB565
	* @param None
	* @retval Current text color in RGB565
	*/
u16 tft_get_special_text_color(void)
{
	return curr_text_color_sp;
}

/**
  * @brief Get the current orientation of the TFT monitor
  * @param None
  * @retval The current orientation (0 - 3)
  */
u8 tft_get_orientation(void)
{
	return tft_orientation;
}

/**
  * @brief Set the orientation of the TFT monitor
  * @param o: orientation (0 - 3)
  * @retval None
  */
void tft_set_orientation(u8 o)
{
	tft_orientation = o % 4;
	tft_force_clear();
}

/**
  * @brief Get the max character per row exclusively (based on the current orientation)
  * @param None
  * @retval The maximum character per row exclusively (if n is returned, range of display is 0..n-1)
  */
u8 tft_get_max_x_char(void)
{
  return (tft_orientation % 2) ? CHAR_MAX_X_HORIZONTAL : CHAR_MAX_X_VERTICAL;
}

/**
  * @brief Get the max character per column exclusively (based on the current orientation)
  * @param None
  * @retval The maximum character per column exclusively (if n is returned, range of display is 0..n-1)
  */
u8 tft_get_max_y_char(void)
{
  return (tft_orientation % 2) ? CHAR_MAX_Y_HORIZONTAL : CHAR_MAX_Y_VERTICAL;
}

/**
  * @brief  Set the position of one pixel
  * @param  None
  * @retval None
  */
void tft_set_pixel_pos(u8 x, u8 y)
{
	tft_write_command(0x2a);		// Column addr set
	tft_write_data(0x00);
	tft_write_data(x); 				// X START
	tft_write_data(0x00);
	tft_write_data(x+1); 			// X END

	tft_write_command(0x2b);		// Row addr set
	tft_write_data(0x00);
	tft_write_data(y);				// Y START
	tft_write_data(0x00);
	tft_write_data(y+1);			// Y END

	tft_write_command(0x2c); 		// write to RAM
}

/**
  * @brief  Set the position of some characters
  * @param  None
  * @retval None
  */
void tft_set_char_pos(u8 x1, u8 y1, u8 x2, u8 y2)
{
	tft_write_command(0x2a);		// Column addr set
	tft_write_data(0x00);
	tft_write_data(x1); 			//X START
	tft_write_data(0x00);
	tft_write_data(x2); 			//X END
	
	tft_write_command(0x2b);		//Row addr set
	tft_write_data(0x00);
	tft_write_data(y1);			//Y START
	tft_write_data(0x00);
	tft_write_data(y2);		//Y END

	tft_write_command(0x2c); 		// write to RAM
}

/**
  * @brief  Clear every piexl on screen
  * @param  None
  * @retval None
  */
void tft_force_clear(void)
{
	u8 x, y;
	for (x = 0; x < CHAR_MAX_X; x++) {
		for (y = 0; y < CHAR_MAX_Y; y++) {
			text_prev[x][y] = ' ';
			text_color_prev[x][y] = curr_text_color;
			bg_color_prev[x][y] = curr_bg_color;
		}
	}
	tft_fill_color(curr_bg_color);
}

/**
  * @brief  Clear one line on screen
  * @param  line: the line to be cleared
  * @retval None
  */
void tft_clear_line(u8 line)
{
	u8 x;
	for (x = 0; x < CHAR_MAX_X; x++) {
		text[x][line] = ' ';
		text_color[x][line] = curr_text_color;
		bg_color[x][line] = curr_bg_color;
	}
}

/**
  * @brief  Clear all lines on screen
  * @param  None
  * @retval None
  */
void tft_clear(void)
{
	u8 y;
	for(y = 0; y < CHAR_MAX_Y; y++)
		tft_clear_line(y);
}

/**
  * @brief  Switch the orientation of screen
  * @param  None
  * @retval None
  */
void tft_toggle(void)
{
	tft_force_clear();
	tft_clear();
	tft_orientation = (tft_orientation+1) % 4;
}

/**
  * @brief  Print a single pixel on screen
  * @param  x: x-coordinate
  * @param  y: y-coordinate
  * @param  color: color of the pixel
  * @retval None
  */
void tft_put_pixel(u8 x, u8 y, u16 color)
{
	switch (tft_orientation) {
		case 0:
			tft_set_pixel_pos(x, y);
			break;
		case 1:
			tft_set_pixel_pos(MAX_WIDTH-y-1, x);
			break;
		case 2:
			tft_set_pixel_pos(MAX_WIDTH-x-1, MAX_HEIGHT-y-1);
			break;
		case 3:
			tft_set_pixel_pos(y, MAX_HEIGHT-x-1);
			break;
	}
    tft_write_data(color >> 8);
    tft_write_data(color);
}

/**
  * @brief  Fill the whole screen with a color
  * @param  color: color to be filled with
  * @retval None
  */
void tft_fill_color(u16 color)
{
	u16 i;					//160*128
	
	tft_write_command(0x2a);		// Column addr set
	tft_write_data(0x00);
	tft_write_data(0x00); 				// X START
	tft_write_data(0x00);
	tft_write_data(0x7f); 			// X END

	tft_write_command(0x2b);		// Row addr set
	tft_write_data(0x00);
	tft_write_data(0x00);				// Y START
	tft_write_data(0x00);
	tft_write_data(0x9f);			// Y END

	tft_write_command(0x2c); 		// write to RAM
	
	for (i = 0; i < MAX_WIDTH*MAX_HEIGHT; i++) {
		tft_write_data(color >> 8);
		tft_write_data(color);
	}
}

u8 tft_char_is_changed(u8 x, u8 y)
{
	u8 re = (text_prev[x][y] != text[x][y] || text_color_prev[x][y] != text_color[x][y] || bg_color_prev[x][y] != bg_color[x][y]);
	text_prev[x][y] = text[x][y];
	text_color_prev[x][y] = text_color[x][y];
	bg_color_prev[x][y] = bg_color[x][y];
	return re;
}

void tft_clear_row(u8 y){
	tft_prints(0,y,"                ");
}

/**
  * @brief  Print a string at certain position ("[...]" as special-color character)
  * @param  x: starting x-coordinate
  * @param  y: starting y-coordinate
  * @param  pstr: string to be printed
  * @retval None
  */
void tft_prints(u8 x, u8 y, const char * pstr, ...)
{
	u8 buf[256], is_special = 0;
	u8* fp = NULL;
	
	va_list arglist;
	va_start(arglist, pstr);
	vsprintf((char*)buf, (const char*)pstr, arglist);
	va_end(arglist);
	
	
	fp = buf;
	while (*fp)	{
		if (*fp == '[' && *(fp - 1) != '\\') {
			is_special = 1;
			fp++;
		} else if (*fp == ']' && *(fp - 1) != '\\') {
			is_special = 0;
			fp++;
		} else if (*fp == '\r' || *fp == '\n') {		  				 
			fp++;
		} else {
			if (x > CHAR_MAX_X || y > CHAR_MAX_Y) {
				fp++;
				continue;
			}
      if (*fp == '\\' && (*(fp+1) == '[' || *(fp+1) == ']')) {
        fp++;
      }
			text[x][y] = *fp++;
			text_color[x][y] = is_special ? curr_text_color_sp : curr_text_color;
			bg_color[x][y] = curr_bg_color;			
			if (x >= CHAR_MAX_X) {
				x = 0;
				y++;
			} else {
				x++;
			}
			if (y >= CHAR_MAX_Y)
				y = 0;
		}
	}
}

/**
  * @brief  Refresh the whole screen
  * @param  None
  * @retval None
  */
void tft_update(void)
{
	s16 x, y, x2, y2, px, py;
	s16 char_n = 0;
	u16 clr;
	
	if (!tft_enabled)
		return;
	
	for (y = 0; y < 10; y++) {
		for (x = 0; x < 16; x++) {
			text_bg_color_prev[x][y] = (text_color[x][y] & 0x8000 ? 0x40 : 0) | (text_color[x][y] & 0x0400 ? 0x20 : 0) | (text_color[x][y] & 0x0010 ? 0x10 : 0); //text:red green blue
			text_bg_color_prev[x][y] = text_bg_color_prev[x][y] | (bg_color[x][y] & 0x8000 ? 0x04 : 0) | (bg_color[x][y] & 0x0400 ? 0x02 : 0) | (bg_color[x][y] & 0x0010 ? 0x01 : 0); //bg :red green blue
		}
	}

	switch (tft_orientation) {
		case 0:
			for (y = 0; y < CHAR_MAX_Y_VERTICAL; y++) {
				for (x = 0; x < CHAR_MAX_X_VERTICAL; x++) {
					if (tft_char_is_changed(x, y)) {
						char_n = 1;
						while (x+char_n < CHAR_MAX_X_VERTICAL && tft_char_is_changed(x+char_n, y)) {
							if (x+char_n >= CHAR_MAX_X_VERTICAL) {
								break;
							} else {
								char_n++;
							}
						}
						tft_set_char_pos(x*CHAR_WIDTH, y*CHAR_HEIGHT, (x+char_n)*CHAR_WIDTH-1, (y+1)*CHAR_HEIGHT-1);
						y2 = y;
						for (py = 0; py < CHAR_HEIGHT; py++) {
							for (px = 0; px < char_n*CHAR_WIDTH; px++) {
								x2 = x+px/CHAR_WIDTH;
								clr = ascii_8x16[((text[x2][y2] - STARTING_ASCII) * CHAR_HEIGHT) + py] & (0x80 >> (px % CHAR_WIDTH)) ? text_color[x2][y2] : bg_color[x2][y2];
								tft_write_data(clr >> 8);
								tft_write_data(clr);
							}
						}
						x += char_n-1;
					}
				}
			}
			break;
		case 1:
			for (x = 0; x < CHAR_MAX_X_HORIZONTAL; x++) {
				for (y = CHAR_MAX_Y_HORIZONTAL-1; y >= 0; y--) {
					if (tft_char_is_changed(x, y)) {
						char_n = 1;
						while (y-char_n > -1 && tft_char_is_changed(x, y-char_n)) {
							if (y-char_n <= -1) {
								break;
							} else {
								char_n++;
							}
						}
						tft_set_char_pos((CHAR_MAX_Y_HORIZONTAL-y-1)*CHAR_HEIGHT, x*CHAR_WIDTH, (CHAR_MAX_Y_HORIZONTAL-y-1+char_n)*CHAR_HEIGHT-1, (x+1)*CHAR_WIDTH-1);
						x2 = x;
						for (px = 0; px < CHAR_WIDTH; px++) {
							for (py = 0; py < char_n*CHAR_HEIGHT; py++) {
								y2 = y-py/CHAR_HEIGHT;
								clr = ascii_8x16[((text[x2][y2] - STARTING_ASCII) * CHAR_HEIGHT) + CHAR_HEIGHT-(py % CHAR_HEIGHT)-1] & (0x80 >> px) ? text_color[x2][y2] : bg_color[x2][y2];
								tft_write_data(clr >> 8);
								tft_write_data(clr);
							}
						}
						y -= char_n-1;
					}
				}
			}
			break;
		case 2:
			for (y = CHAR_MAX_Y_VERTICAL-1; y >= 0; y--) {
				for (x = CHAR_MAX_X_VERTICAL-1; x >= 0; x--) {
					if (tft_char_is_changed(x, y)) {
						char_n = 1;
						while (x-char_n > -1 && tft_char_is_changed(x-char_n, y)) {
							if (x-char_n <= -1) {
								break;
							} else {
								char_n++;
							}
						}
						tft_set_char_pos((CHAR_MAX_X_VERTICAL-x-1)*CHAR_WIDTH, (CHAR_MAX_Y_VERTICAL-y-1)*CHAR_HEIGHT, (CHAR_MAX_X_VERTICAL-x-1+char_n)*CHAR_WIDTH-1, (CHAR_MAX_Y_VERTICAL-y)*CHAR_HEIGHT-1);
						y2 = y;
						for (py = 0; py < CHAR_HEIGHT; py++) {
							for (px = 0; px < char_n*CHAR_WIDTH; px++) {
								x2 = x-px/CHAR_WIDTH;
								clr = ascii_8x16[((text[x2][y2] - STARTING_ASCII) * CHAR_HEIGHT) + (CHAR_HEIGHT-py-1)] & (0x80 >> (CHAR_WIDTH-(px % CHAR_WIDTH)-1)) ? text_color[x2][y2] : bg_color[x2][y2];
								tft_write_data(clr >> 8);
								tft_write_data(clr);
							}
						}
						x -= char_n-1;
					}
				}
			}
			break;
		case 3:
			for (x = CHAR_MAX_X_HORIZONTAL-1; x >= 0; x--) {
				for (y = 0; y < CHAR_MAX_Y_HORIZONTAL; y++) {
					if (tft_char_is_changed(x, y)) {
						char_n = 1;
						while (y+char_n < CHAR_MAX_Y_HORIZONTAL && tft_char_is_changed(x, y+char_n)) {
							if (y+char_n >= CHAR_MAX_Y_HORIZONTAL) {
								break;
							} else {
								char_n++;
							}
						}
						tft_set_char_pos(y*CHAR_HEIGHT, (CHAR_MAX_X_HORIZONTAL-x-1)*CHAR_WIDTH, (y+char_n)*CHAR_HEIGHT-1, (CHAR_MAX_X_HORIZONTAL-x)*CHAR_WIDTH-1);
						x2 = x;
						for (px = 0; px < CHAR_WIDTH; px++) {
							for (py = 0; py < char_n*CHAR_HEIGHT; py++) {
								y2 = y+py/CHAR_HEIGHT;
								clr = ascii_8x16[((text[x2][y2] - STARTING_ASCII) * CHAR_HEIGHT) + (py % CHAR_HEIGHT)] & (0x80 >> (CHAR_WIDTH-px-1)) ? text_color[x2][y2] : bg_color[x2][y2];
								tft_write_data(clr >> 8);
								tft_write_data(clr);
							}
						}
						y += char_n-1;
					}
				}
			}
			break;
	}
}

void tft_put_logo(u8 x, u8 y){
	//Place black background
	for (u8 loopX = 0; loopX<47; loopX++){
		for (u8 loopY = 0; loopY<39; loopY++){
			tft_put_pixel(x+loopX, y+loopY, BLACK);
		}
	}

	//Place DARK_RED parallelogram
	for (u8 loopX = 15; loopX<45; loopX++){
		tft_put_pixel(x+loopX, y+1, DARK_RED);
	}
	for (u8 loopX = 14; loopX<44; loopX++){
		tft_put_pixel(x+loopX, y+2, DARK_RED);
		tft_put_pixel(x+loopX, y+3, DARK_RED);
	}
	for (u8 loopX = 13; loopX<43; loopX++){
		tft_put_pixel(x+loopX, y+4, DARK_RED);
		tft_put_pixel(x+loopX, y+5, DARK_RED);
		tft_put_pixel(x+loopX, y+6, DARK_RED);
	}
	for (u8 loopX = 12; loopX<42; loopX++){
		tft_put_pixel(x+loopX, y+7, DARK_RED);
		tft_put_pixel(x+loopX, y+8, DARK_RED);
		tft_put_pixel(x+loopX, y+9, DARK_RED);
	}
	for (u8 loopX = 11; loopX<41; loopX++){
		tft_put_pixel(x+loopX, y+10, DARK_RED);
		tft_put_pixel(x+loopX, y+11, DARK_RED);
	}
	for (u8 loopX = 10; loopX<40; loopX++){
		tft_put_pixel(x+loopX, y+12, DARK_RED);
		tft_put_pixel(x+loopX, y+13, DARK_RED);
		tft_put_pixel(x+loopX, y+14, DARK_RED);
		tft_put_pixel(x+loopX, y+15, DARK_RED);
	}
	for (u8 loopX = 9; loopX<39; loopX++){
		tft_put_pixel(x+loopX, y+16, DARK_RED);
		tft_put_pixel(x+loopX, y+17, DARK_RED);
		tft_put_pixel(x+loopX, y+18, DARK_RED);
	}
	for (u8 loopX = 8; loopX<38; loopX++){
		tft_put_pixel(x+loopX, y+19, DARK_RED);
		tft_put_pixel(x+loopX, y+20, DARK_RED);
	}
	for (u8 loopX = 7; loopX<37; loopX++){
		tft_put_pixel(x+loopX, y+21, DARK_RED);
		tft_put_pixel(x+loopX, y+22, DARK_RED);
		tft_put_pixel(x+loopX, y+23, DARK_RED);
		tft_put_pixel(x+loopX, y+24, DARK_RED);
	}

	//Put the R letter
	tft_put_pixel(x+16, y+6, GREY);
	for (u8 loopX = 17; loopX<36; loopX++){
		tft_put_pixel(x+loopX, y+6, WHITE);
	}

	for (u8 loopX = 15; loopX<37; loopX++){
		tft_put_pixel(x+loopX, y+7, WHITE);
	}

	tft_put_pixel(x+14, y+8, DARK_GREY);
	for (u8 loopX = 15; loopX<37; loopX++){
		tft_put_pixel(x+loopX, y+8, WHITE);
	}
	tft_put_pixel(x+37, y+8, GREY);

	for (u8 loopX = 14; loopX<38; loopX++){
		tft_put_pixel(x+loopX, y+9, WHITE);
	}

	for (u8 loopX = 14; loopX<20; loopX++){
		tft_put_pixel(x+loopX, y+10, WHITE);
	}
	tft_put_pixel(x+20, y+10, GREY);
	tft_put_pixel(x+28, y+10, GREY);
	for (u8 loopX = 31; loopX<38; loopX++){
		tft_put_pixel(x+loopX, y+10, WHITE);
	}

	for (u8 loopX = 14; loopX<20; loopX++){
		tft_put_pixel(x+loopX, y+11, WHITE);
	}
	tft_put_pixel(x+28, y+11, GREY);
	tft_put_pixel(x+29, y+11, DARK_GREY);
	for (u8 loopX = 32; loopX<38; loopX++){
		tft_put_pixel(x+loopX, y+11, WHITE);
	}

	tft_put_pixel(x+13, y+12, GREY);
	for (u8 loopX = 14; loopX<19; loopX++){
		tft_put_pixel(x+loopX, y+12, WHITE);
	}
	tft_put_pixel(x+19, y+12, DARK_GREY);
	tft_put_pixel(x+21, y+12, WHITE);
	for (u8 loopX = 30; loopX<38; loopX++){
		tft_put_pixel(x+loopX, y+12, WHITE);
	}

	for (u8 loopX = 13; loopX<19; loopX++){
		tft_put_pixel(x+loopX, y+13, WHITE);
	}
	tft_put_pixel(x+19, y+13, GREY);
	tft_put_pixel(x+28, y+13, WHITE);
	tft_put_pixel(x+29, y+13, WHITE);
	for (u8 loopX = 32; loopX<38; loopX++){
		tft_put_pixel(x+loopX, y+13, WHITE);
	}

	for (u8 loopX = 13; loopX<19; loopX++){
		tft_put_pixel(x+loopX, y+14, WHITE);
	}
	tft_put_pixel(x+19, y+14, DARK_GREY);
	tft_put_pixel(x+20, y+14, DARK_GREY);
	tft_put_pixel(x+28, y+14, GREY);
	tft_put_pixel(x+31, y+14, GREY);
	for (u8 loopX = 32; loopX<38; loopX++){
		tft_put_pixel(x+loopX, y+14, WHITE);
	}

	//LEFT LEG
	for (u8 loopX = 12; loopX<18; loopX++){
		tft_put_pixel(x+loopX, y+15, WHITE);
		tft_put_pixel(x+loopX, y+16, WHITE);
		tft_put_pixel(x+loopX, y+17, WHITE);
	}
	for (u8 loopX = 11; loopX<17; loopX++){
		tft_put_pixel(x+loopX, y+18, WHITE);
		tft_put_pixel(x+loopX, y+19, WHITE);
		tft_put_pixel(x+loopX, y+20, WHITE);
	}
	for (u8 loopX = 10; loopX<16; loopX++){
		tft_put_pixel(x+loopX, y+21, WHITE);
		tft_put_pixel(x+loopX, y+22, WHITE);
	}
	for (u8 loopX = 10; loopX<15; loopX++){
		tft_put_pixel(x+loopX, y+23, WHITE);
	}
	for (u8 loopX = 9; loopX<15; loopX++){
		tft_put_pixel(x+loopX, y+24, WHITE);
	}
	for (u8 loopX = 9; loopX<14; loopX++){
		tft_put_pixel(x+loopX, y+25, WHITE);
		tft_put_pixel(x+loopX, y+26, WHITE);
	}

	//RIGHT LEG
	for (u8 loopX = 21; loopX<38; loopX++){
		tft_put_pixel(x+loopX, y+15, WHITE);
	}
	for (u8 loopX = 20; loopX<38; loopX++){
		tft_put_pixel(x+loopX, y+16, WHITE);
	}
	for (u8 loopX = 20; loopX<37; loopX++){
		tft_put_pixel(x+loopX, y+17, WHITE);
	}
	for (u8 loopX = 21; loopX<36; loopX++){
		tft_put_pixel(x+loopX, y+18, WHITE);
	}
	for (u8 loopX = 21; loopX<29; loopX++){
		tft_put_pixel(x+loopX, y+19, WHITE);
	}
	for (u8 loopX = 21; loopX<30; loopX++){
		tft_put_pixel(x+loopX, y+20, WHITE);
		tft_put_pixel(x+loopX+1, y+21, WHITE);
		tft_put_pixel(x+loopX+2, y+22, WHITE);
		tft_put_pixel(x+loopX+3, y+23, WHITE);
	}
	for (u8 loopX = 25; loopX<33; loopX++){
		tft_put_pixel(x+loopX, y+24, WHITE);
	}
	for (u8 loopX = 26; loopX<35; loopX++){
		tft_put_pixel(x+loopX, y+25, WHITE);
	}
	for (u8 loopX = 27; loopX<35; loopX++){
		tft_put_pixel(x+loopX, y+26, WHITE);
	}

	//Misc in R letter
	tft_put_pixel(x+11, y+17, DARK_GREY);
	tft_put_pixel(x+21, y+17, GREY);
	tft_put_pixel(x+17, y+18, DARK_GREY);
	tft_put_pixel(x+36, y+18, GREY);
	for (u8 loopX = 29; loopX<34; loopX++){
		tft_put_pixel(x+loopX, y+19, GREY);
	}
	tft_put_pixel(x+34, y+19, DARK_GREY);
	tft_put_pixel(x+10, y+20, GREY);
	tft_put_pixel(x+18, y+20, WHITE);
	tft_put_pixel(x+16, y+21, GREY);
	tft_put_pixel(x+20, y+21, WHITE);
	tft_put_pixel(x+18, y+22, WHITE);
	tft_put_pixel(x+18, y+22, GREY);
	tft_put_pixel(x+9, y+23, DARK_GREY);
	tft_put_pixel(x+33, y+24, GREY);
	tft_put_pixel(x+14, y+25, DARK_GREY);
	tft_put_pixel(x+8, y+26, GREY);
	tft_put_pixel(x+35, y+26, DARK_GREY);

	//Bright Red in red region
	tft_put_pixel(x+15, y+3, RED);
	tft_put_pixel(x+17, y+4, RED);
	tft_put_pixel(x+16, y+5, RED);
	tft_put_pixel(x+35, y+5, RED);
	tft_put_pixel(x+38, y+6, RED);
	tft_put_pixel(x+49, y+6, RED);
	tft_put_pixel(x+39, y+7, RED);
	tft_put_pixel(x+11, y+14, RED);
	tft_put_pixel(x+20, y+13, RED);
	tft_put_pixel(x+39, y+14, RED);
	tft_put_pixel(x+39, y+15, RED);
	tft_put_pixel(x+20, y+18, RED);
	tft_put_pixel(x+37, y+18, RED);
	tft_put_pixel(x+38, y+19, RED);
	tft_put_pixel(x+34, y+20, RED);
	tft_put_pixel(x+36, y+20, RED);
	tft_put_pixel(x+33, y+22, RED);
	tft_put_pixel(x+8, y+23, RED);
	tft_put_pixel(x+20, y+23, RED);
	tft_put_pixel(x+21, y+23, RED);
	tft_put_pixel(x+33, y+23, RED);
	tft_put_pixel(x+8, y+24, RED);
	tft_put_pixel(x+17, y+24, RED);
	tft_put_pixel(x+22, y+24, RED);
	tft_put_pixel(x+36, y+24, RED);
	tft_put_pixel(x+7, y+25, RED);
	tft_put_pixel(x+24, y+25, RED);
	tft_put_pixel(x+7, y+26, RED);
	tft_put_pixel(x+15, y+26, RED);
	tft_put_pixel(x+18, y+26, RED);
	tft_put_pixel(x+19, y+26, RED);
	tft_put_pixel(x+22, y+26, RED);
	tft_put_pixel(x+23, y+26, RED);
	tft_put_pixel(x+24, y+26, RED);

	//Put the HKUST word
	tft_put_pixel(x+6, y+28, WHITE);
	tft_put_pixel(x+10, y+28, WHITE);
	tft_put_pixel(x+11, y+28, WHITE);
	tft_put_pixel(x+13, y+28, WHITE);
	tft_put_pixel(x+17, y+28, WHITE);
	tft_put_pixel(x+20, y+28, WHITE);
	tft_put_pixel(x+23, y+28, WHITE);
	tft_put_pixel(x+24, y+28, WHITE);
	for (u8 loopX=27;loopX<36;loopX++){
		tft_put_pixel(x+loopX, y+28, WHITE);
	}

	tft_put_pixel(x+5, y+29, WHITE);
	tft_put_pixel(x+6, y+29, WHITE);
	tft_put_pixel(x+9, y+29, WHITE);
	tft_put_pixel(x+12, y+29, WHITE);
	tft_put_pixel(x+13, y+29, WHITE);
	tft_put_pixel(x+16, y+29, WHITE);
	tft_put_pixel(x+17, y+29, WHITE);
	tft_put_pixel(x+19, y+29, WHITE);
	tft_put_pixel(x+20, y+29, WHITE);
	tft_put_pixel(x+22, y+29, WHITE);
	tft_put_pixel(x+23, y+29, WHITE);
	tft_put_pixel(x+24, y+29, WHITE);
	for (u8 loopX=26;loopX<29;loopX++){
		tft_put_pixel(x+loopX, y+29, WHITE);
	}
	for (u8 loopX=29;loopX<35;loopX++){
		tft_put_pixel(x+loopX, y+29, WHITE);
	}

	tft_put_pixel(x+5, y+30, WHITE);
	tft_put_pixel(x+6, y+30, WHITE);
	tft_put_pixel(x+8, y+30, WHITE);
	tft_put_pixel(x+9, y+30, WHITE);
	tft_put_pixel(x+12, y+30, WHITE);
	tft_put_pixel(x+13, y+30, WHITE);
	tft_put_pixel(x+15, y+30, WHITE);
	tft_put_pixel(x+16, y+30, WHITE);
	tft_put_pixel(x+19, y+30, WHITE);
	tft_put_pixel(x+20, y+30, WHITE);
	tft_put_pixel(x+22, y+30, WHITE);
	tft_put_pixel(x+23, y+30, WHITE);
	tft_put_pixel(x+26, y+30, WHITE);
	tft_put_pixel(x+27, y+30, WHITE);
	tft_put_pixel(x+32, y+30, WHITE);

	for (u8 loopX=5;loopX<10;loopX++){
		tft_put_pixel(x+loopX, y+31, WHITE);
	}
	for (u8 loopX=11;loopX<16;loopX++){
		tft_put_pixel(x+loopX, y+31, WHITE);
	}
	tft_put_pixel(x+19, y+31, WHITE);
	tft_put_pixel(x+20, y+31, WHITE);
	tft_put_pixel(x+22, y+31, WHITE);
	tft_put_pixel(x+23, y+31, WHITE);
	tft_put_pixel(x+25, y+31, WHITE);
	tft_put_pixel(x+26, y+31, WHITE);
	tft_put_pixel(x+27, y+31, WHITE);
	tft_put_pixel(x+31, y+31, WHITE);
	tft_put_pixel(x+32, y+31, WHITE);

	for (u8 loopX=4;loopX<10;loopX++){
		tft_put_pixel(x+loopX, y+32, WHITE);
	}
	tft_put_pixel(x+11, y+32, WHITE);
	tft_put_pixel(x+12, y+32, WHITE);
	tft_put_pixel(x+14, y+32, WHITE);
	tft_put_pixel(x+15, y+32, WHITE);
	tft_put_pixel(x+18, y+32, WHITE);
	tft_put_pixel(x+19, y+32, WHITE);
	tft_put_pixel(x+21, y+32, WHITE);
	tft_put_pixel(x+22, y+32, WHITE);
	tft_put_pixel(x+26, y+32, WHITE);
	tft_put_pixel(x+27, y+32, WHITE);
	tft_put_pixel(x+31, y+32, WHITE);
	tft_put_pixel(x+32, y+32, WHITE);

	tft_put_pixel(x+4, y+33, WHITE);
	tft_put_pixel(x+5, y+33, WHITE);
	tft_put_pixel(x+7, y+33, WHITE);
	tft_put_pixel(x+8, y+33, WHITE);
	tft_put_pixel(x+11, y+33, WHITE);
	tft_put_pixel(x+12, y+33, WHITE);
	tft_put_pixel(x+14, y+33, WHITE);
	tft_put_pixel(x+15, y+33, WHITE);
	tft_put_pixel(x+18, y+33, WHITE);
	tft_put_pixel(x+19, y+33, WHITE);
	tft_put_pixel(x+21, y+33, WHITE);
	tft_put_pixel(x+22, y+33, WHITE);
	tft_put_pixel(x+25, y+33, WHITE);
	tft_put_pixel(x+26, y+33, WHITE);
	tft_put_pixel(x+27, y+33, WHITE);
	tft_put_pixel(x+30, y+33, WHITE);
	tft_put_pixel(x+31, y+33, WHITE);

	tft_put_pixel(x+3, y+34, WHITE);
	tft_put_pixel(x+4, y+34, WHITE);
	tft_put_pixel(x+5, y+34, WHITE);
	tft_put_pixel(x+7, y+34, WHITE);
	tft_put_pixel(x+8, y+34, WHITE);
	tft_put_pixel(x+10, y+34, WHITE);
	tft_put_pixel(x+11, y+34, WHITE);
	tft_put_pixel(x+14, y+34, WHITE);
	tft_put_pixel(x+15, y+34, WHITE);
	tft_put_pixel(x+18, y+34, WHITE);
	tft_put_pixel(x+19, y+34, WHITE);
	tft_put_pixel(x+20, y+34, WHITE);
	tft_put_pixel(x+21, y+34, WHITE);
	tft_put_pixel(x+24, y+34, WHITE);
	tft_put_pixel(x+25, y+34, WHITE);
	tft_put_pixel(x+26, y+34, WHITE);
	tft_put_pixel(x+30, y+34, WHITE);
	tft_put_pixel(x+31, y+34, WHITE);

	tft_put_pixel(x+3, y+35, WHITE);
	tft_put_pixel(x+4, y+35, WHITE);
	tft_put_pixel(x+7, y+35, WHITE);
	tft_put_pixel(x+19, y+35, WHITE);
	tft_put_pixel(x+20, y+35, WHITE);
	tft_put_pixel(x+24, y+35, WHITE);

	//Place minor detail of HKUST word
	tft_put_pixel(x+5, y+28, GREY);
	tft_put_pixel(x+12, y+28, GREY);
	tft_put_pixel(x+16, y+28, GREY);
	tft_put_pixel(x+8, y+29, GREY);
	tft_put_pixel(x+31, y+30, GREY);
	tft_put_pixel(x+4, y+31, GREY);
	tft_put_pixel(x+23, y+32, GREY);
	tft_put_pixel(x+10, y+35, GREY);
	tft_put_pixel(x+11, y+35, GREY);
	tft_put_pixel(x+14, y+35, GREY);
	tft_put_pixel(x+15, y+35, GREY);
	tft_put_pixel(x+18, y+35, GREY);
	tft_put_pixel(x+25, y+35, GREY);
	tft_put_pixel(x+30, y+35, GREY);
	tft_put_pixel(x+31, y+35, GREY);

	tft_put_pixel(x+7, y+28, DARK_GREY);
	tft_put_pixel(x+25, y+30, DARK_GREY);
	tft_put_pixel(x+13, y+32, DARK_GREY);
	tft_put_pixel(x+25, y+32, DARK_GREY);
	tft_put_pixel(x+24, y+33, DARK_GREY);
	tft_put_pixel(x+6, y+35, DARK_GREY);
	tft_put_pixel(x+12, y+34, DARK_GREY);
	tft_put_pixel(x+16, y+34, DARK_GREY);
	tft_put_pixel(x+16, y+35, DARK_GREY);
}
