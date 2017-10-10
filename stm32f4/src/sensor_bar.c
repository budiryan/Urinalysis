#include "sensor_bar.h"
u16 data1[8];
u16 data2[8];
u16 sensorbar_result[16];


void receive(CanRxMsg msg){
    for(int i = 0; i < 8 ;i++){
        data1[i] = msg.Data[i];
    }
}

void receive2(CanRxMsg msg2){
    for(int i = 0; i < 8 ; i++){
        data2[i] = msg2.Data[i]; 
    }
}
void fill_sensorbar_array(){
    for(int i = 0; i < 8; i++){
        sensorbar_result[i] = data2[7-i];
    }
    for(int i = 0; i < 8; i++){
        sensorbar_result[8+i] = data1[7-i];
    }
}

void sensorbar_init(void){
    can_init();
    can_rx_init();
    can_rx_add_filter(ADDRESS1,CAN_RX_MASK_EXACT,receive);
    can_rx_add_filter(ADDRESS2,CAN_RX_MASK_EXACT,receive2);
}

void print_sensorbar_array(){
    tft_prints(0,0,"Sensor output");
    for(int i = 0; i < 16 ;i++){
        tft_prints(i,1,"%d",sensorbar_result[i]);
    }
}
