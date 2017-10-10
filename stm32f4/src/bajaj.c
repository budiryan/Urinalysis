#include "bajaj.h"

ZONE game_zone;
SLOPEZONE current_slope_zone;
GAMESIDE side;
INFRARED_SENSOR infrared1;
INFRARED_SENSOR infrared2;

int MAX_NINETY_TURNING;
int IMU_ANGLE1;
int LESSER_TURNING;
int NINETY_TURNING;
int SLOPE_TURNING_RIGHT;
int SLOPE_TURNING_LEFT;
int DELAY;
int passed_river = 0;
int passed_down_slope = 0;
int imu_movement;
int sat1;
int value1;
int button_count_red = 0;
int button_count_white = 0;
int ENTER_RIVER_ENCODER;
int SECOND_RIVER_ENCODER;
int time_delta;
int servo_delta;
int button_white_count = 0;
int button_red_count = 0;
char game_zone_string[12]= "UNKNOWN";
char current_slope_zone_string[16];
char global_state_string[16] = "UPSLOPE";
bool retry = false;
float imu_factor;
float NINETY_IMU;

u8 data1[8];
u8 data2[8];
u8 sensorbar_result[16];
u8 river;
u8 border;
u8 global_state;
int begin;
int end;
int length;
int last_movement;
int last_turn;
float factor;
bool sensor_is_flipped;
int full_white;
int hue_avg;
int time1;
int time2;
float angle_after_ninety;
float angle_enter_river;
bool done_turning;
int ninety_time_stamp;
bool system_on;

ENCODER USED_ENCODER = ENCODER1;

void initalize_values(void){
    if(button_pressed(BUTTON_K0)){
        while(button_pressed(BUTTON_K0));
        //Tune for competition
        ENTER_RIVER_ENCODER = 6300;
        SECOND_RIVER_ENCODER = 4500;
        
        
        tft_init(PIN_ON_BOTTOM,DARKWHITE,DARK_RED,RED); 
        IMU_ANGLE1 = -25;
        NINETY_IMU = -180;
        LESSER_TURNING = -275;
        SLOPE_TURNING_RIGHT = SERVO_MICROS_MID + 300;
        SLOPE_TURNING_LEFT = SERVO_MICROS_MID - 300;
        DELAY = 2000;
        side = REDSIDE;
        infrared1 = INFRARED_SENSOR_RIGHT;
        infrared2 = INFRARED_SENSOR_LEFT;
        button_red_count = 0;
        NINETY_TURNING = SERVO_MICROS_MID + 400;
        current_slope_zone = STARTZONE;
        strcpy(current_slope_zone_string,"STARTZONE");
        ardu_cal_ypr[0] = (float)0;
        system_on = 1;
    }
    else if(button_pressed(BUTTON_K1)){
        while(button_pressed(BUTTON_K1));
        //Tune for competition
        ENTER_RIVER_ENCODER = 7500;
        SECOND_RIVER_ENCODER = 5500;
        
        
        tft_init(PIN_ON_BOTTOM,DARKWHITE,DARKBLUE,RED);
        IMU_ANGLE1 = 25;
        NINETY_IMU = 180;
        LESSER_TURNING = 300;
        SLOPE_TURNING_RIGHT = SERVO_MICROS_MID + 300;
        SLOPE_TURNING_LEFT = SERVO_MICROS_MID - 300;
        DELAY = 2000;
        side = BLUESIDE;
        infrared1 = INFRARED_SENSOR_LEFT;
        infrared2 = INFRARED_SENSOR_RIGHT;
        button_white_count = 0;
        NINETY_TURNING = SERVO_MICROS_MID - 380;
        current_slope_zone = STARTZONE;
        strcpy(current_slope_zone_string,"STARTZONE");
		ardu_cal_ypr[0] = (float)0;
        system_on = 1;
    }
}

void system_init(){
    led_init();			//Initiate LED
    ticks_init();		//Ticks initialization
    tft_init(PIN_ON_BOTTOM,BLACK,WHITE,RED);     //LCD Initialization
    buzzer_init();	//Initialize buzzer
    servo_init();
    button_init();
    encoder_init();
    infrared_sensor_init();
    ardu_imu_init();
   
    //Initialize the CAN protocol for motor
    can_init();
    can_rx_init();
    can_rx_add_filter(0x0C5,CAN_RX_MASK_EXACT,receive);
    can_rx_add_filter(0x0C6,CAN_RX_MASK_EXACT,receive2);
    can_rx_add_filter(0x0C7,CAN_RX_MASK_EXACT,receive3);
}

void receive(CanRxMsg msg){
    for(int i = 0; i < 8 ;i++){
        data1[i] = msg.Data[i]; 
    }
}
void receive2(CanRxMsg msg){
    for(int i = 0; i < 8 ; i++){
        data2[i] = msg.Data[i]; 
    }
}

void receive3(CanRxMsg msg){
    hue_avg = msg.Data[0];
}

void fill_sensorbar_array(){
    if(get_ticks() % 10 == 0){
        for(int i = 0; i < 8; i++){
            if(sensor_is_flipped) sensorbar_result[i] = data2[7 - i];
            else sensorbar_result[i] = data1[i];
        }
        for(int i = 0; i < 8; i++){
            if(sensor_is_flipped) sensorbar_result[8+i] = data1[7 - i];
            else sensorbar_result[8 + i] = data2[i];
        }
    }
}

int get_correct_req(){
	int angleDiff = retry ? 90 : angle_enter_river - angle_after_ninety;
	int req = ENTER_RIVER_ENCODER * int_sin(angleDiff * 10) / 10000;
	if (angleDiff > 90) req = ENTER_RIVER_ENCODER + (ENTER_RIVER_ENCODER - req);
    req = req > ENTER_RIVER_ENCODER + 200 ? ENTER_RIVER_ENCODER + 200 : req;
	return req;
}

void print_data(){
    for(int i = 0; i < 16 ;i++) tft_prints(i,0,"%d",sensorbar_result[i]);
    tft_prints(0,1,"COLOR: %s",game_zone_string);
    tft_prints(0,2,"SLOPE STATUS:");
    tft_prints(0,3,"%s",current_slope_zone_string); 
    tft_prints(0,4,"GLOBAL STATE:");
    tft_prints(0,5,"%s",global_state_string);
    tft_prints(0,6,"yaw:%.2f",ardu_cal_ypr[0]);
    tft_prints(0,7,"enc up: %d", get_correct_req());
    tft_prints(0,8,"e1:%d", get_count(ENCODER1));
    tft_prints(0,9,"e2:%d",get_count(ENCODER2));
}
void print_system_off(void){
    tft_prints(0,0,"PRESS RED / WHITE");
    tft_prints(0,1,"e1:%d e2:%d",get_minimize_count(ENCODER1),get_minimize_count(ENCODER2));
    tft_prints(0,3,"zone: %s",game_zone_string);
    for(int i = 0; i < 16 ;i++) tft_prints(i,4,"%d",sensorbar_result[i]);
    tft_prints(0,5,"calibrated:%d",ardu_imu_calibrated);
    tft_prints(0,6,"yaw:%.2f",ardu_cal_ypr[0]);
    tft_prints(0,7,"length:%d fw:%d",length,full_white);
    tft_prints(0,8,"il:%d ir:%d",read_infrared_sensor(INFRARED_SENSOR_LEFT),read_infrared_sensor(INFRARED_SENSOR_RIGHT));
    tft_prints(0,9,"ul:%d ur:%d", read_infrared_sensor(INFRARED_SENSOR_UPPER_LEFT),read_infrared_sensor(INFRARED_SENSOR_UPPER_RIGHT));
}

void process_array(){
    //Seperate the marking into a 2 stage process
    
    //scan left properties
    int begin_left = -1;
    int end_left;
    int length_left = 0;
    for(int k = 0; k < 16; k++) {
        int el = sensorbar_result[k];
        if (el == 1) {
            length_left++;
            if (begin_left == -1) begin_left = k;
            if(sensorbar_result[k + 1] == 0){
                end_left = k; //If the rest is zero, regards others as noise
                break;
            }
            else {
                end_left = k;
            }  
        }   
    }
    
    //scan from right properties
    int begin_right;
    int end_right = -1;
    int length_right = 0;
    for(int i = 15; i >= 0; i--) {
        int el = sensorbar_result[i];
        if (el == 1) {
            length_right++;
            if (end_right == -1) end_right = i;
            if(sensorbar_result[i - 1] == 0){
                begin_right = i; //If the rest is zero, regards others as noise
                break;
            }
            else {
                begin_right = i;
            }  
        }   
    }
    
    //Now compare left and right,regard other's shorter segment data as a noise
    if(length_left > length_right){
        length = length_left;
        begin = begin_left;
        end = end_left;
    }
    else{
        length = length_right;
        begin = begin_right;
        end = end_right;        
    }
    
}


void go_normal(void){
    if (get_full_ticks() - last_turn >= (int)DELAY){            
        if (length >= 1 && length <= 16) {
            if (full_white && !passed_river){
                if ((((begin + end)/ 2)) < 0) {
                    float factor = 0;
                    last_movement = (SERVO_MICROS_LEFT ) - (factor * (SERVO_MICROS_LEFT - SERVO_MICROS_RIGHT));
                }
                else{
                    float factor = (((begin + end)/ 2)) / (float) 16;
                    last_movement = (SERVO_MICROS_LEFT) - (factor * (SERVO_MICROS_LEFT - SERVO_MICROS_RIGHT));
                }
            }
            
            else if(full_white && passed_river && !passed_down_slope){
                float factor = ((begin + end)/ 2) / (float) 16;
                last_movement = (SERVO_MICROS_LEFT) - (factor * (SERVO_MICROS_LEFT - SERVO_MICROS_RIGHT));
            }
            
            else if(passed_down_slope){
                switch(side){
                    case REDSIDE:
                        if ((((begin + end)/ 2) + 2) > 16) {
                            last_movement = SERVO_MICROS_RIGHT;
                        }
                        else{
                            float factor = (((begin + end)/ 2) + 2) / (float) 16;
                            last_movement = (SERVO_MICROS_LEFT) - (factor * (SERVO_MICROS_LEFT - SERVO_MICROS_RIGHT));
                        }
                    break;
                    case BLUESIDE:
                        if ((((begin + end)/ 2) + 1) > 16) {
                            last_movement = SERVO_MICROS_RIGHT;
                        }
                        else{
                            float factor = (((begin + end)/ 2) + 1) / (float) 16;
                            last_movement = (SERVO_MICROS_LEFT) - (factor * (SERVO_MICROS_LEFT - SERVO_MICROS_RIGHT));
                        }
                    break;
                }                
            }
            else{
                float factor = ((begin + end)/ 2) / (float) 16;
                last_movement = (SLOPE_TURNING_LEFT) - (factor * (SLOPE_TURNING_LEFT - SLOPE_TURNING_RIGHT));
            }
        }
        if(game_zone == LIGHTGREENZONE && passed_river && get_minimize_count(USED_ENCODER) > 10){
            passed_down_slope = 1;
        }
    servo_control(BAJAJ_SERVO,last_movement);
    }
}
void go_ninety(void){
    switch(side){
        case REDSIDE:
            last_movement = NINETY_TURNING;
            if(((int)ardu_cal_ypr[0] > -80) || ((get_full_ticks() - ninety_time_stamp) > 3000)){
                full_white = 1;
                strcpy(global_state_string,"BEFORE RIVER");
                reset_all_encoder();
                START_UP_play;
                angle_after_ninety = ardu_cal_ypr[0];
                global_state = CHECK_NINETY;
            }
        break;
        case BLUESIDE:
            last_movement = NINETY_TURNING;
            if(((int)ardu_cal_ypr[0] < 80) || ((get_full_ticks() - ninety_time_stamp) > 3000)){
                full_white = 1;
                strcpy(global_state_string,"BEFORE RIVER");
                reset_all_encoder();
                START_UP_play;
				ardu_cal_ypr[0] = (float)0;
				angle_after_ninety = ardu_cal_ypr[0];
                global_state = CHECK_NINETY;
            }
        break;
    }
    servo_control(BAJAJ_SERVO,last_movement);
}

void go_using_imu(void){
    imu_factor = ardu_cal_ypr[0] / 180.0f;
    last_movement = imu_movement = SERVO_MICROS_MID - (imu_factor * 400);
    servo_control(BAJAJ_SERVO,last_movement);
    
    //Stopping condition
    if((get_count(ENCODER1) > 20000) && !read_infrared_sensor(infrared2)){
        reset_all_encoder();
        strcpy(global_state_string,"EXIT RIVER");
        START_UP_play;
        global_state = EXIT_RIVER;
    }
}

void go_straight_little_bit(void){
    //Stopping condition
    if(get_count(USED_ENCODER) > 2000){
        passed_river = 1;
        strcpy(global_state_string,"AFTER RIVER");
        reset_all_encoder();
        START_UP_play;
        global_state = NORMAL;
    }
    else{
        switch(side){
            case BLUESIDE:
                last_movement = SERVO_MICROS_MID + (int)LESSER_TURNING;
            break;
            case REDSIDE:
                last_movement = SERVO_MICROS_MID + (int)LESSER_TURNING;
            break;
        }        
        servo_control(BAJAJ_SERVO, last_movement);
    }
}

void determine_zone(){
    switch(hue_avg){
        case 0:
            switch(side){
                case REDSIDE:
                    game_zone = PINKZONE;
                    strcpy(game_zone_string,"PinkStart");
                    break;
                case BLUESIDE:
                    game_zone = LIGHTBLUEZONE;
                    strcpy(game_zone_string,"BlueStart");
                    break;
            }
            river  = 0;
            break;
        case 1:
            switch(side){
                case REDSIDE:
                    game_zone = PINKZONE;
                    strcpy(game_zone_string,"PinkStart");
                    break;
                case BLUESIDE:
                    game_zone = LIGHTBLUEZONE;
                    strcpy(game_zone_string,"BlueStart");
                    break;
            }
            river = 0;
            break;
        case 2:
            game_zone = DARKGREENZONE;
            strcpy(game_zone_string,"Darkgreen");
            river = 1;
            break;
        case 3:
            game_zone = ORANGEZONE;
            strcpy(game_zone_string,"Orange");
            river = 0;
            break;
        case 4:
            game_zone = BLUEZONE;
            strcpy(game_zone_string,"Darkblue");
            river = 1;
            break;
        case 5:
            game_zone = LIGHTGREENZONE;
            strcpy(game_zone_string,"Lightgreen");
            river = 0;
            break;
        default:
            game_zone = NOCOLOURZONE;
            strcpy(game_zone_string,"Unknown");
            
    }
}

void run_user_interface(void){
    //User Interface Section
    if(button_pressed(BUTTON_K0)){
        button_count_red++;
        if(button_count_red > 100){
            while(button_pressed(BUTTON_K0));
            switch(button_red_count){
                case 0:
                    current_slope_zone = GREENSLOPE1;
                    strcpy(current_slope_zone_string,"GREENSLOPE1");
                    button_red_count++;
                break;
                case 1:
                    current_slope_zone = ORANGE1;
                    strcpy(current_slope_zone_string,"ORANGE1");
                    button_red_count++;
                break;
                case 2:
                    current_slope_zone = GREENSLOPE2;
                    strcpy(current_slope_zone_string,"GREENSLOPE2");
                    button_red_count++;    
                break;
                case 3:
                    current_slope_zone = ORANGE2;
                    strcpy(current_slope_zone_string,"ORANGE2");
                    button_red_count++;     
                break;
                case 4:
                    current_slope_zone = GREENSLOPE3;
                    strcpy(current_slope_zone_string,"GREENSLOPE3");
                    button_red_count++;  
                break;
                case 5:
                    current_slope_zone = STARTZONE;
                    strcpy(current_slope_zone_string,"STARTZONE");
                    button_red_count = 0;    
                break;
            }
			START_UP_play;
            button_count_red = 0;
			ardu_cal_ypr[0] = (float)0;
        }
    }
    
    if(button_pressed(BUTTON_K1)){
        button_count_white++;
        if(button_count_white > 100){
            while(button_pressed(BUTTON_K1));
            switch(button_white_count){
                case 0:
                    full_white = 1;
                    passed_river = 0;
                    passed_down_slope = 0;
                    global_state = NORMAL;
                    reset_all_encoder();
                    IMU_ANGLE1 = -85;
                    strcpy(global_state_string,"BEFORE RIVER");
					START_UP_play;
                    button_white_count++;
                break;
                case 1:
                    passed_river = 1;
                    full_white = 1;
                    passed_down_slope = 0;
                    global_state = NORMAL;
                    strcpy(global_state_string,"AFTER RIVER");
					START_UP_play;
                    button_white_count++;
                break;
                case 2:
                    full_white = 1;
                    passed_river = 0;
                    passed_down_slope = 0;                
                    button_white_count = 0;
					START_UP_play;
                    IMU_ANGLE1 = -50;
                break;    
            }
            current_slope_zone = FINISHEDSLOPE;
            strcpy(current_slope_zone_string,"FINISHEDSLOPE");
            button_count_white = 0;
			retry = true;
        }
    }
}

void escape_first_island(void){
    switch(side){
        case REDSIDE:
            if(!done_turning && get_count(USED_ENCODER) > SECOND_RIVER_ENCODER){
                CLICK_MUSIC;
                reset_all_encoder();
                done_turning = true;
                global_state = RIVERING2;
            }
        break;
        case BLUESIDE:
            if(!done_turning && get_count(USED_ENCODER) > SECOND_RIVER_ENCODER){
                CLICK_MUSIC;
                reset_all_encoder();
                done_turning = true;
                global_state = RIVERING2;
            } 
        break;
    }
}

void scan_river(void){
    const int RIVER_TURNING_LEFT = SERVO_MICROS_MID - 200;
    const int RIVER_TURNING_RIGHT = SERVO_MICROS_MID + 200;
    if(done_turning && get_minimize_count(USED_ENCODER) > 13) {
        START_UP_play;
        reset_all_encoder();
        global_state = EXIT_RIVER;
    }
    else{    
        float factor = ((begin + end)/ 2) / (float) 16;
        last_movement = (RIVER_TURNING_LEFT) - (factor * (RIVER_TURNING_LEFT - RIVER_TURNING_RIGHT));
        servo_control(BAJAJ_SERVO, last_movement); 
    }
}



void finish_ninety(void){
    if(length != 0)
        global_state = NORMAL;
    else{
        time_delta = 100;
        switch(side){
            case REDSIDE:
                servo_delta = -40;
                if((get_full_ticks() % time_delta == 0) && (last_movement < (SERVO_MICROS_MID + 700))){
                    last_movement += servo_delta;
                    servo_control(BAJAJ_SERVO, last_movement);
                }
                break;
            case BLUESIDE:
                servo_delta = 40;
                if((get_full_ticks() % time_delta == 0) && (last_movement > (SERVO_MICROS_MID - 700))){
                    last_movement += servo_delta;
                    servo_control(BAJAJ_SERVO, last_movement);
                }
                break;
        }
    }

}






