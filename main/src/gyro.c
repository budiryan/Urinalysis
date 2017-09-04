#include "gyro.h"
#include "approx_math.h"

s16 angle = 0, real_x = 0, real_y = 0;
s16 angle_offset = 0, x_offset = 0, y_offset = 0;


s32 SHIFT_X = 75; // 75 // 92
s32 SHIFT_Y = -73; // -73 //-280

static POSITION gyro_pos = {0, 0, 0};
static POSITION gyro_pos_raw = {0, 0, 0};
static u8 rx_state = 0; 
static u8 max_state = 0;
static u8 rx_command = 0;
static u8 min_command = 255;
static u8 buf_rec = 0;
static u8 buf_data[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

static u8 rx_command_arr[GYRO_COMMAND_LENGTH] = {GYRO_UPDATED, GYRO_REPLY};
static u8 buf_len[GYRO_COMMAND_LENGTH] = {0x06, 0x01};		//data size, for confirm data

volatile u8 reply_flag = 0;

volatile u8 gyro_available = 0;


void plus_x(void)
{
	SHIFT_X++;
    return;
}
void minus_x(void)
{
	SHIFT_X--;
}
void plus_y(void)
{
	SHIFT_Y++;
}
void minus_y(void)
{
	SHIFT_Y--;
}

s32 gyro_get_shift_x(void)
{
    return SHIFT_X;
}

s32 gyro_get_shift_y(void)
{
	return SHIFT_Y;
}
/**
  * @brief  Initialization of Gyro
  * @param  None
  * @retval None
  */
void gyro_init(void)
{
   uart_init(GYRO_UART, 115200);
   uart_interrupt(GYRO_UART);
}


/**
	* @brief Get the position object
	* @param None
	* @retval The position object
	*/
const POSITION* get_pos(void)
{
	return &gyro_pos;
}

const POSITION* get_pos_raw(void)
{
  return &gyro_pos_raw;
}

/**
  * @brief  Get the Angle (yaw)
  * @param  None
  * @retval Angle (yaw)
  */
s16 get_angle(void)
{
	return gyro_pos.angle;
}

/**
  * @brief  Send the update flag
  * @param  None
  * @retval None
  */
void gyro_pos_update(void)			// unuseful
{
	uart_tx_byte(GYRO_UART, GYRO_WAKEUP);
	uart_tx_byte(GYRO_UART, GYRO_UPDATE);
	uart_tx_byte(GYRO_UART, 0);
}

/**
  * @brief  Send the calibration flag
  * @param  None
  * @retval 1 = successful, 0 = failed
  */
u8 gyro_cal(void)
{
	u16 ticks_last = get_ticks();
	reply_flag &= ~GYRO_FLAG_CAL;
	
	uart_tx_byte(GYRO_UART, GYRO_WAKEUP);
	uart_tx_byte(GYRO_UART, GYRO_CAL);
	uart_tx_byte(GYRO_UART, 0);
	
	while (!(reply_flag & GYRO_FLAG_CAL)) {
		if ((get_ticks()+1000-ticks_last) % 1000 >= 20)			// 20 ms timeout
			return 0;
	}
	return 1;
}

/**
  * @brief  Update the offset of X, Y coordinate and Angle
  * @param  x: X coordinate to be set
  * @param  y: Y coordinate to be set
  * @param  a: angle to be set
  * @retval 1 = successful, 0 = failed
  */
u8 gyro_pos_set(s16 x, s16 y, s16 a)
{
	u16 ticks_last = get_ticks();
	reply_flag &= ~GYRO_FLAG_SET_POS;
	
	uart_tx_byte(GYRO_UART, GYRO_WAKEUP);
	uart_tx_byte(GYRO_UART, GYRO_POS_SET);
	uart_tx_byte(GYRO_UART, 0x06);
	uart_tx_byte(GYRO_UART, x >> 8);
	uart_tx_byte(GYRO_UART, x & 0xFF);
	uart_tx_byte(GYRO_UART, y >> 8);
	uart_tx_byte(GYRO_UART, y & 0xFF);
	uart_tx_byte(GYRO_UART, a >> 8);
	uart_tx_byte(GYRO_UART, a & 0xFF);
	
	u16 timeout = 100;
	while (!(reply_flag & GYRO_FLAG_SET_POS)) {
		if (!(--timeout)) {
			return 0;
		}
	}

	return 1;
}

/**
  * @brief  Interrupt for USART3
  * @param  None
  * @retval None
  */
void USART3_IRQHandler(void)
{
	u8 rx_data, i;
	u16 x, y, a;
    
	if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
	{
		rx_data = (u8)USART_ReceiveData(USART3);
		switch (rx_state) {
			case 0:	// wakeup
				if (rx_data == GYRO_WAKEUP) {
					rx_command = 0xFF;
					buf_rec = 0;
					rx_state++;
				}
				break;
			case 1:	// command
				for (i = 0; i < GYRO_COMMAND_LENGTH; i ++) {
					if (rx_data == rx_command_arr[i]) {
						rx_command = i;
						rx_state++;
						break;
					}
				}
				if (rx_command == 0xFF)	// command not in list		
					rx_state = 0;
				break;
			case 2: // confirm command
				if (rx_data != buf_len[rx_command]) {		// wrong data length
					rx_state = 0;
					break;
				}
				rx_state++;
				if (buf_len[rx_command] > 0) {
					break;
				}
			case 3: // receive data
				if (buf_len[rx_command] == 0) {
					rx_state++;
				} else {
					buf_data[buf_rec++] = rx_data;
					if (buf_rec >= buf_len[rx_command]) {
						rx_state++;
					} else {
						break;
					}
				}
			case 4:
				switch (rx_command) {
					case 0:		// GYRO_UPDATED // X and Y are flipped.
						y = buf_data[0];
						y <<= 8;
						y |= buf_data[1];
						x = buf_data[2];
						x <<= 8;
						x |= buf_data[3];
						a = buf_data[4];
						a <<= 8;
						a |= buf_data[5];
						
						if (a < 3600) {
							gyro_available = 1;
							
							gyro_pos_raw.x = (s16) x;
							gyro_pos_raw.y = (s16) y;
							gyro_pos_raw.angle = (s16) a;
							
                            gyro_pos.x = (X_FLIP*x*10000-SHIFT_X*10000+SHIFT_X*int_cos(gyro_pos_raw.angle)+SHIFT_Y*int_sin(gyro_pos_raw.angle))/10000;
                            gyro_pos.y = (Y_FLIP*y*10000-SHIFT_Y*10000+SHIFT_Y*int_cos(gyro_pos_raw.angle)-SHIFT_X*int_sin(gyro_pos_raw.angle))/10000;
                            gyro_pos.angle = gyro_pos_raw.angle;
							
						} else {
							gyro_available = 0;
						}
						break;
					case 1:		// GYRO_REPLY
						reply_flag |= (1 << buf_data[0]);
						break;
							
				}
				rx_state = 0;
				break;
		}
	}
    
}


s16 get_X(void)
{
   s32 pos_x = (real_x*10000-SHIFT_X*10000+SHIFT_X*int_cos(angle)+SHIFT_Y*int_sin(angle))/10000;
	//return real_x;
	return pos_x;//real_x;
}

/**
  * @brief  Get the Y coordinate
  * @param  None
  * @retval Y coordinate
  */
s16 get_Y(void)
{
	s32 pos_y = (real_y*10000-SHIFT_Y*10000+SHIFT_Y*int_cos(angle)-SHIFT_X*int_sin(angle))/10000;
	//return real_y;
	return pos_y;//real_y;
}
