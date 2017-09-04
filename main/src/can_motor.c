#include "can_motor.h"

s32 can_motor_encoder_value[CAN_MOTOR_COUNT] = {0};
s32 can_motor_pwm_value[CAN_MOTOR_COUNT] = {0};
s32 can_motor_target_vel_value[CAN_MOTOR_COUNT] = {0};
s32 can_motor_curr_vel_value[CAN_MOTOR_COUNT] = {0};

u16 get_can_motor_id(u8 motor_id)
{
	return (CAN_MOTOR_BASE + (u8)motor_id);
}

void can_motor_init(void)
{
	can_rx_add_filter(CAN_MOTOR_BASE, CAN_RX_MASK_DIGIT_0_F,can_motor_feedback);
}

/*** TX ***/
/**
	* @brief Send encoder value to mainboard (regular called in main)
	* @param None.
	* @retval None.
	*/
void send_encoder(s32 encoder_value, u8 id)
{
  struct CAN_MESSAGE msg;
  msg.id = CAN_MOTOR_BASE + id;
  msg.length = CAN_ENCODER_FEEDBACK_LENGTH;
  msg.data[0] = CAN_ENCODER_FEEDBACK;
  msg.data[1] = one_to_n_bytes(encoder_value,0);
  msg.data[2] = one_to_n_bytes(encoder_value,1);
  msg.data[3] = one_to_n_bytes(encoder_value,2);
  msg.data[4] = one_to_n_bytes(encoder_value,3);
  can_tx_enqueue(msg);

}

/*** User Interface ***/
/** These are how mainboard sent data control motor, keep here for reference ONLY **/
/**
	* @brief Set motor velocity (CAN)
	* @param motor_id (MOTOR_ID enum)
	* @param vel (vel of close_loop is not corresponded to open_loop)
	* @param close_loop_flag: true if close_loop should be applied
	* @retval None.
	*/
void motor_set_vel(MOTOR_ID motor_id, s32 vel, CLOSE_LOOP_FLAG close_loop_flag)
{
	CAN_MESSAGE msg;
	
	assert_param((u8)motor_id < CAN_MOTOR_COUNT);

	msg.id = get_can_motor_id(motor_id);
	msg.length = CAN_MOTOR_VEL_LENGTH;
	msg.data[0] = CAN_MOTOR_VEL_CMD;
	msg.data[1] = (u8)(one_to_n_bytes(vel, 0));
	msg.data[2] = (u8)(one_to_n_bytes(vel, 1));
	msg.data[3] = (u8)(one_to_n_bytes(vel, 2));
	msg.data[4] = (u8)(one_to_n_bytes(vel, 3));
	msg.data[5] = (u8)(close_loop_flag);
	
	can_tx_enqueue(msg);
}

/**
	* @brief Set motor position (CAN)
	* @param motor_id (MOTOR_ID enum)
	* @param vel (vel of close_loop is not corresponded to open_loop)
	* @param pos: The position need to move to relative to current encoder value.
	* @retval None.
	*/
void motor_set_pos(MOTOR_ID motor_id, u16 vel, s32 pos)
{
	CAN_MESSAGE msg;
	
	assert_param((u8)motor_id < CAN_MOTOR_COUNT);
	
	msg.id = get_can_motor_id(motor_id);
	msg.length = CAN_MOTOR_POS_LENGTH;
	msg.data[0] = CAN_MOTOR_POS_CMD;
	msg.data[1] = (u8)(one_to_n_bytes(vel, 0));
	msg.data[2] = (u8)(one_to_n_bytes(vel, 1));
	msg.data[3] = (u8)(one_to_n_bytes(pos, 0));
	msg.data[4] = (u8)(one_to_n_bytes(pos, 1));
	msg.data[5] = (u8)(one_to_n_bytes(pos, 2));
	msg.data[6] = (u8)(one_to_n_bytes(pos, 3));

	can_tx_enqueue(msg);
}

/**
	* @brief Set motor acceleration (CAN)
	* @param motor_id (MOTOR_ID enum)
	* @param accel: acceleration parameter of motor
	* @retval None.
	*/
void motor_set_acceleration(MOTOR_ID motor_id, u16 accel)
{
	CAN_MESSAGE msg;
	
	assert_param((u8)motor_id < CAN_MOTOR_COUNT);
	
	msg.id = get_can_motor_id(motor_id);
	msg.length = CAN_MOTOR_ACCEL_LENGTH;
	msg.data[0] = CAN_MOTOR_ACCEL_CMD;
	msg.data[1] = (u8)(one_to_n_bytes(accel, 0));
	msg.data[2] = (u8)(one_to_n_bytes(accel, 1));

	can_tx_enqueue(msg);
}

/**
	* @brief Lock and stop motor immediately (CAN)
	* @param motor_id (MOTOR_ID enum)
	* @retval None.
	*/
void motor_lock(MOTOR_ID motor_id)
{
	CAN_MESSAGE msg;
	
	assert_param((u8)motor_id < CAN_MOTOR_COUNT);
	
	msg.id = get_can_motor_id(motor_id);
	msg.length = CAN_MOTOR_LOCK_LENGTH;
	msg.data[0] = CAN_MOTOR_LOCK_CMD;

	can_tx_enqueue(msg);
}
/*** End of TX ***/

/*** RX ***/

/*** motor command decoding ***/
//void motor_cmd_decoding(CanRxMsg msg)
//{
//	try {
//		u8 id = msg.StdId - CAN_MOTOR_BASE;
//		switch (msg.Data[0]) {
//			case CAN_MOTOR_VEL_CMD:
//				if (msg.DLC == CAN_MOTOR_VEL_LENGTH) {
//					const u8 VEL_SIZE = 4;	// velocity data contain 4 bytes
//					u8 fragment_vel[VEL_SIZE] = {0};
//					for (u8 i = 0; i < VEL_SIZE; ++i) {
//						fragment_vel[i] = msg.Data[i+1];
//					}
//					CLOSE_LOOP_FLAG loop_flag = (CLOSE_LOOP_FLAG) msg.Data[5];// 5-th byte is loop-flag (start as 0th byte)
//					// velocity or pwm control.
//					s32 velocity = n_bytes_to_one(fragment_vel, VEL_SIZE);
//					// Ignore if same velocity is sent.
//					(loop_flag == CLOSE_LOOP) ? motor::get_instance()->set_target_vel(velocity) :
//							motor::get_instance()->set_pwm(velocity);
//					NO_CAN_LED.off();
//				}
//				break;
//			case CAN_MOTOR_ACCEL_CMD:
//				if (msg.DLC == CAN_MOTOR_ACCEL_LENGTH) {
//					const u8 ACCEL_SIZE = 2;	// acceleration data contain 2 bytes
//					u8 fragment_accel[ACCEL_SIZE] = {0};
//					for (u8 i = 0; i < ACCEL_SIZE; ++i) {
//						fragment_accel[i] = msg.Data[i+1];
//					}
//					motor::get_instance()->set_accel(n_bytes_to_one(fragment_accel, ACCEL_SIZE));
//				}
//				break;
//			case CAN_MOTOR_POS_CMD:
//				// Coming Soon.
//				break;
//			case CAN_MOTOR_LOCK_CMD:
//				if (msg.DLC == CAN_MOTOR_LOCK_LENGTH) {
//					motor::get_instance()->lock();
//				}
//				break;
//			default:
//				return;
//		}
//	} catch (motor_error&) {
//		return;
//	}
//}
void display_value(int x){
    tft_prints(0,6,"msg.StdId: %d",x);
}

void display_value2(int y){
    tft_prints(0,7,"Data: %d",y);
}


void can_motor_feedback(CanRxMsg msg){
	switch (msg.Data[0]) {
		case CAN_ENCODER_FEEDBACK:
			if (msg.DLC == CAN_ENCODER_FEEDBACK_LENGTH) {
				// Range check 
				if (msg.StdId >= CAN_MOTOR_BASE && msg.StdId < CAN_MOTOR_BASE + CAN_MOTOR_COUNT) {
					s32 feedback = n_bytes_to_one(&msg.Data[1], 4);
					can_motor_encoder_value[msg.StdId - CAN_MOTOR_BASE] = feedback;
				}
			}
		break;
		case CAN_PWM_FEEDBACK:
			if (msg.DLC == CAN_PWM_FEEDBACK_LENGTH) {
				// Range check 
				if (msg.StdId >= CAN_MOTOR_BASE && msg.StdId < CAN_MOTOR_BASE + CAN_MOTOR_COUNT) {
					s32 feedback = n_bytes_to_one(&msg.Data[1], 4);
					can_motor_pwm_value[msg.StdId - CAN_MOTOR_BASE] = feedback;
				}
			}
		break;
		case CAN_TARGET_VEL_FEEDBACK:
			if (msg.DLC == CAN_TARGET_VEL_FEEDBACK_LENGTH) {
				// Range check 
				if (msg.StdId >= CAN_MOTOR_BASE && msg.StdId < CAN_MOTOR_BASE + CAN_MOTOR_COUNT) {
					s32 feedback = n_bytes_to_one(&msg.Data[1], 4);
					can_motor_target_vel_value[msg.StdId - CAN_MOTOR_BASE] = feedback;
				}
			}
		break;
		case CAN_CURR_VEL_FEEDBACK:
			if (msg.DLC == CAN_CURR_VEL_FEEDBACK_LENGTH) {
				// Range check 
				if (msg.StdId >= CAN_MOTOR_BASE && msg.StdId < CAN_MOTOR_BASE + CAN_MOTOR_COUNT) {
					s32 feedback = n_bytes_to_one(&msg.Data[1], 4);
					can_motor_curr_vel_value[msg.StdId - CAN_MOTOR_BASE] = feedback;
				}
			}
		break;
	}
}



/*** User interface ***/
/** These are how mainboard receive data, keep here for reference ONLY **/
void can_motor_feedback_encoder(CanRxMsg msg)
{
	switch (msg.Data[0]) {
		case CAN_ENCODER_FEEDBACK:
			if (msg.DLC == CAN_ENCODER_FEEDBACK_LENGTH) {
				// Range check 
				if (msg.StdId >= CAN_MOTOR_BASE && msg.StdId < CAN_MOTOR_BASE + CAN_MOTOR_COUNT) {
					s32 feedback = n_bytes_to_one(&msg.Data[1], 4);
					can_motor_encoder_value[msg.StdId - CAN_MOTOR_BASE] = feedback;
				}
			}
		break;
	}
}

s32 get_encoder_value(MOTOR_ID motor_id)
{
	return can_motor_encoder_value[motor_id];
}

s32 get_pwm_value(MOTOR_ID motor_id){
    return can_motor_pwm_value[motor_id];
}


