#ifndef __GYRO_H
#define __GYRO_H

#include "usart.h"
#include "tft_display.h"
#include "approx_math.h"

#define MENU_ADD_GYRO_TEST menu_add(5, "Position Test", position_test)

#define GYRO_UART  COM2

/*** Command List ***/
#define GYRO_WAKEUP				0x01

#define GYRO_UPDATE				    0x10
#define GYRO_CAL				      0x20
#define GYRO_POS_SET			    0x30
#define GYRO_AUTO_UPDATE	    0x40

/*** Reply Command List ***/
#define GYRO_REPLY				    0x50
#define GYRO_UPDATED			    0x80

#define GYRO_COMMAND_LENGTH		2

#define GYRO_FLAG_SET_POS		  0x01
#define GYRO_FLAG_CAL			    0x02

typedef struct {
	s16 x, y, angle;
} POSITION;


#define	X_FLIP						1
#define	Y_FLIP						1
/** Varies along robots (depends on the encoder position) **/

extern volatile u8 gyro_available;

void plus_x(void);
void minus_x(void);
void plus_y(void);
void minus_y(void);
s32 gyro_get_shift_x(void);
s32 gyro_get_shift_y(void);

const POSITION* get_pos(void);	// Get the position ({x, y, angle})
const POSITION* get_pos_raw(void);

void gyro_init(void);
void gyro_rx_handler(u8 rx_data);
void gyro_pos_update(void);		//update gyro values only when auto-update function is disabled in gyro
u8 gyro_cal(void);	//callibrate gyro
u8 gyro_pos_set(s16 x, s16 y, s16 a);	//set position of gyro

s16 get_X(void);
s16 get_Y(void);
s16 get_angle(void);


#endif		/* __GYRO_H */
