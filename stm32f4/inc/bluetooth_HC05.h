#ifndef __BLUETOOTH_H
#define __BLUETOOTH_H
#include "stm32f4xx.h"

#define MAX_STRLEN 12 // this is the maximum string length of our string in characters
extern volatile char received_string[MAX_STRLEN+1]; // this will hold the recieved string

#define RCV_BUFFER_SIZE 40

static volatile char rcvBuffer[RCV_BUFFER_SIZE];
static volatile char rcvMessage[RCV_BUFFER_SIZE];
static volatile uint32_t rcvIndex = 0;
static volatile uint8_t rcvMessageFlag = 0;
static volatile uint8_t rcvMessageSize = 0;


void bluetooth_init(uint32_t baudrate);

void bluetooth_send_char(char item);

void bluetooth_send_string(char* items);

#endif
