#include "ardu_imu.h"

/**************************************************************************************************************
** ROBOCON 2016
** H K U S T
**
** Author:	Rex Cheng
** Contact:	hkchengad@connect.ust.hk
** February 2016
**
** This file is for using the 9DOF ArduIMU, it gives yaw, pitch and roll
** The IMU has a MCU, so all the calculation is done on the stick
** Communication is simply UART.
** I uses the SparkFun 9DOF Razor IMU firmware here.
** Follow the instruction in the firmware's github page to calibrate and setup it.
** Use a FTDI to flash program inside.
** Note that I uses static calibration on startup for yaw, but this should be done on the chip for angular acceleration instead
** But well I am too lazy, this is good enough.
** 
** For actual usage, refer to the adapter, but briefly, init->sync->start to keep updating->cali->ok
**
** Reference: http://www.geeetech.com/xzn-arduimu-9-degrees-of-freedom-p-535.html
** See Also: http://www.geeetech.com/wiki/index.php/ArduIMU_9_Degrees_of_Freedom
** Firmware: https://github.com/ptrbrtz/razor-9dof-ahrs
***************************************************************************************************************/

//The IMU will send back #SYNCH01\r\n
static u8 sync_progress = '#';
static u8 ardu_imu_buffer[12] = {0};
static s8 ardu_imu_buffer_pointer = 0;

bool ardu_imu_synced = false;
bool ardu_imu_staged = false;
bool ardu_imu_pre_staged = false;
bool ardu_imu_calibrated = false;
float ardu_out_ypr[3] = {0}; 
float ardu_cal_ypr[3] = {0}; 

static float yaw_samples[SAMPLE_SIZE] = {0};
static u8 sample_size = 0;
static float yaw_bias = 0;

/**
** Receiver function for the imu
** Also handle the synchronizing with the imu
**/
void ardu_imu_receiver(u8 byte){
	//Sync with the IMU
	if (ardu_imu_synced == false){
		//Match the sync sequence
		if (byte == sync_progress){
			switch(byte){
				case '#':
					sync_progress = 'S';
					break;
				case 'S':
					sync_progress = 'Y';
					break;
				case 'Y':
					sync_progress = 'N';
					break;
				case 'N':
					sync_progress = 'C';
					break;
				case 'C':
					sync_progress = 'H';
					break;
				case 'H':
					sync_progress = '0';
					break;
				case '0':
					sync_progress = '1';
					break;
				case '1':
					sync_progress = '\r';
					break;
				case '\r':
					sync_progress = '\n';
					break;
				case '\n':
					//Finish syncing, init other parts
					sync_progress = '#';
					ardu_imu_synced = true;
					#ifdef IMU_USE_CONTINUOUS_MODE
						uart_tx(ARDU_IMU_UART, "#o1#ob#oe0"); //Continuous binary output and request syncing
					#else
						uart_tx(ARDU_IMU_UART, "#o0#ob#oe0#f"); //Discrete binary output upon sending #f, request syncing
					#endif 
					break;
			}
		}else{
			sync_progress = '#';
		}
	}else{
		//If sync is finished
		ardu_imu_buffer[ardu_imu_buffer_pointer] = byte;
		//Put received byte into a array, waiting to be resolved
		if (++ardu_imu_buffer_pointer == 12){
			ardu_imu_buffer_pointer = 0;
			#ifndef IMU_USE_CONTINUOUS_MODE
				//Request an additional frame when discrete mode is used
				uart_tx_byte(ARDU_IMU_UART, '#');
				uart_tx_byte(ARDU_IMU_UART, 'f');
			#endif
			if (ardu_imu_synced && !ardu_imu_staged){
				//Notify the main loop that syncing is ready
				ardu_imu_pre_staged = true;
			}
		}
	}
}

//Try to synchronize with the imu, often take multiple times
void ardu_imu_try_sync(){
	uart_tx(ARDU_IMU_UART, "#s01"); //Request syncing
}

void ardu_imu_init(){
	uart_init(ARDU_IMU_UART, 115200);
	uart_interrupt_init(ARDU_IMU_UART, ardu_imu_receiver);
	ardu_imu_synced = ardu_imu_staged = ardu_imu_pre_staged = ardu_imu_calibrated = false;
	sync_progress = '#';
	yaw_bias = 0;
}

static u8 sync_count = 0;
//Update the values in array
void ardu_imu_value_update(){
	if (!ardu_imu_synced){
		if (sync_count > 5){
			ardu_imu_try_sync();
			sync_count = 0;
		}else{
			sync_count++;
		}
	}else{
		for (u8 i=0; i<3; i++){
			byte2float dataset;
			for (u8 k = (i*4); k < (i*4 + 4); k++){
				//Make 4 bytes into one float
				dataset.chars[(k%4)] = ardu_imu_buffer[k];
			}
			ardu_cal_ypr[i] += fabs_diff(dataset.f, ardu_out_ypr[i]);
			ardu_out_ypr[i] = dataset.f;
		}
		
		if(!ardu_imu_calibrated){
			yaw_samples[sample_size++] = ardu_out_ypr[0];
			if (sample_size>=SAMPLE_SIZE){
				yaw_bias = (yaw_samples[SAMPLE_SIZE-1] - yaw_samples[0]) / SAMPLE_SIZE;
				ardu_imu_calibrated = true;
			}
		}else{
			ardu_cal_ypr[0] -= yaw_bias;
		}
	}
}
