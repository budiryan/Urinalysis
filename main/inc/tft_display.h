#ifndef __TFT_DISPLAY_H
#define __TFT_DISPLAY_H

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "1.8 TFT_ascii.h"

#include <stdio.h>
#include <stdarg.h>

#include "ticks.h"

typedef enum {
	PIN_ON_TOP = 0,
	PIN_ON_LEFT = 1,
	PIN_ON_BOTTOM = 2,
	PIN_ON_RIGHT = 3
}TFT_ORIENTATION;

// SPI, RST, DC (Converted to F4)
#define TFT_RST_PORT	GPIOB
#define TFT_RST_PIN		GPIO_Pin_14 
#define TFT_DC_PORT		GPIOC
#define TFT_DC_PIN		GPIO_Pin_9

#define TFT_SPI			SPI2
#define GPIO_CS			GPIOB
#define GPIO_Pin_CS	GPIO_Pin_12
#define RCC_AHB1Periph_GPIO_CS	RCC_AHB1Periph_GPIOB


// Color
#define	BGR888_MODE		1

#if (!BGR888_MODE)
#define	RGB888TO565(RGB888)  (((RGB888 >> 8) & 0xF800) |((RGB888 >> 5) & 0x07E0) | ((RGB888 >> 3) & 0x001F))
#else 
#define	RGB888TO565(BGR888)  (((BGR888 >> 19) & 0x001F) |((BGR888 >> 5) & 0x07E0) | (((u32)BGR888 << 8) & 0xF800))
#endif

//to minimize the MCU calculation
#define WHITE               0xFFFF//    (RGB888TO565(0xFFFFFF))
#define BLACK               0x0000//    (RGB888TO565(0x000000))
#define DARK_GREY           0x52AA//    (RGB888TO565(0x555555))
#define GREY                0x001F//    (RGB888TO565(0xAAAAAA))
#define RED                 0xAD55//    (RGB888TO565(0xFF0000))
#define DARK_RED            (RGB888TO565(0xF20000))
#define ORANGE              0x04DF//    (RGB888TO565(0xFF9900))
#define YELLOW              0x07FF//    (RGB888TO565(0xFFFF00))
#define GREEN               0x07E0//    (RGB888TO565(0x00FF00))
#define DARK_GREEN          0x0660//    (RGB888TO565(0x00CC00))
#define BLUE                0xF800//    (RGB888TO565(0x0000FF))
#define BLUE2               0x6104//    (RGB888TO565(0x202060))
#define SKY_BLUE            0xFE62//    (RGB888TO565(0x11CFFF))
#define CYAN                0xFC51//    (RGB888TO565(0x8888FF))
#define PURPLE              0xAD40//    (RGB888TO565(0x00AAAA))
#define PINK                RGB888TO565(0xFFB6C1)
#define DARKBLUE            RGB888TO565(0x000083)
#define DARKWHITE           RGB888TO565(0x9F9F9F)



#define MAX_WIDTH				128
#define MAX_HEIGHT				160
#define CHAR_WIDTH				8
#define CHAR_HEIGHT				16

#define CHAR_MAX_X_VERTICAL		16
#define CHAR_MAX_Y_VERTICAL		10

#define CHAR_MAX_X_HORIZONTAL	20
#define CHAR_MAX_Y_HORIZONTAL	8

#define CHAR_MAX_X				20		// max between CHAR_MAX_X_VERTICAL and CHAR_MAX_X_HORIZONTAL
#define CHAR_MAX_Y				10		// max between CHAR_MAX_Y_VERTICAL and CHAR_MAX_Y_HORIZONTAL

//extern u8 tft_orientation;
//extern u8 tft_width;
//extern u8 tft_height;
//extern u16 curr_bg_color;
//extern u16 curr_text_color;
//extern u16 curr_text_color_sp;

extern char text[CHAR_MAX_X][CHAR_MAX_Y];
extern u16 text_color[CHAR_MAX_X][CHAR_MAX_Y];
extern u16 bg_color[CHAR_MAX_X][CHAR_MAX_Y];
extern u8 text_bg_color_prev[CHAR_MAX_X][CHAR_MAX_Y]; // for transmit for xbc, msb 4bits: text color, lsb 4bits: bg color

void tft_prints_enable(u8 i);

void tft_spi_init(void);
void tft_write_command(u8 command);
void tft_write_data(u8 data);
void tft_config(void);
void tft_reset(void);

void tft_init(u8 orientation, u16 in_bg_color, u16 in_text_color, u16 in_text_color_sp);
void tft_enable(void);
void tft_disable(void);
void tft_set_bg_color(u16 in_bg_color);
u16 tft_get_bg_color(void);
void tft_set_text_color(u16 in_text_color);
u16 tft_get_text_color(void);
void tft_set_special_color(u16 text_color_sp);
u16 tft_get_special_text_color(void);
u8 tft_get_orientation(void);
void tft_set_orientation(u8 o);
u8 tft_get_max_x_char(void);
u8 tft_get_max_y_char(void);
 
void tft_set_pixel_pos(u8 x, u8 y);
void tft_set_char_pos(u8 x1, u8 y1, u8 x2, u8 y2);
void tft_force_clear(void);
void tft_clear_line(u8 line);
void tft_clear(void);
void tft_toggle(void);
void tft_put_pixel(u8 x, u8 y, u16 color);
void tft_fill_color(u16 color);
u8 tft_char_is_changed(u8 x, u8 y);
void tft_clear_row(u8 y);
void tft_prints(u8 x, u8 y, const char * pstr, ...);
void tft_update(void);
void tft_update_trigger(void (*fx)(void));

void tft_put_logo(u8 x, u8 y);

#endif		/* __TFT_DISPLAY_H */
