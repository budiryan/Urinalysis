#ifndef __SENSORBARH
#define __SENSORBARH
#include "can_protocol.h"
#include "tft_display.h"

#define ADDRESS1 0x0C5 //Its not a variable!
#define ADDRESS2 0x0C6 //It will be replaced during compilation !!!



//Internal functions
void receive(CanRxMsg msg);
void receive(CanRxMsg msg2);


void sensorbar_init(void);
void fill_sensorbar_array(void);
void print_sensorbar_array(void);

#endif
