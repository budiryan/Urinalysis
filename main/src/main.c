#include "main.h"

u8 data1[8];
u8 data2[8];
u8 sensorbar_result[16];
u8 river;
u8 border;
u8 global_state = NORMAL;
int hue_avg;
int begin = -1;
int eco_finish_timestamp = 0;
int end = 0;
int length = 0;
int last_turn = 0;
int full_white = 0;
int system_on = 0;
int last_movement = SERVO_MICROS_MID;
int time1 = 0;
int time2 = 0;
int ninety_time_stamp;
float angle_after_ninety = 0.0;
float angle_enter_river = 0.0;
float factor = 0;
bool imu_fucked_up = false;
bool done_turning = false;
bool sensor_is_flipped = true;

extern int MAX_NINETY_TURNING;
extern int IMU_ANGLE1;
extern float NINETY_IMU;
extern int LESSER_TURNING;
extern int SLOPE_TURNING_RIGHT;
extern int SLOPE_TURNING_LEFT;
extern int DELAY;
extern GAMESIDE side;
extern INFRARED_SENSOR infrared1;
extern INFRARED_SENSOR infrared2;
extern SLOPEZONE current_slope_zone;
extern ZONE game_zone;
extern int passed_river;
extern int passed_down_slope;
extern char current_slope_zone_string[10];
extern char global_state_string[16];
extern int ENTER_RIVER_ENCODER;
extern ENCODER USED_ENCODER;

int main(void) {
    //Initialization of all hardware
    system_init();
    u32 ticks_ms_img = 0;
    bool songIsPlayed = false;
    bool startSong = false;
    bool final_music = false;
    bool cali = false;
    int enter_time_stamp;
    reset_all_encoder();
	while (1) {
        if(ticks_ms_img != get_ticks()){
            buzzer_check();
            ticks_ms_img = get_ticks();
            tft_clear();
            fill_sensorbar_array();
            process_array();
            determine_zone();
            if((get_ticks() % 25 == 0) && !imu_fucked_up)
                ardu_imu_value_update();
            //Play song when IMU is calibrated
            if(ardu_imu_calibrated)
                cali = true;
            if(cali && !songIsPlayed){
                MARIO_START;
                songIsPlayed = true;
            }
            switch(system_on){
                case ON:
                    //Emergency turning system
                    if(read_infrared_sensor(INFRARED_SENSOR_UPPER_LEFT) && passed_river){
                       servo_control(BAJAJ_SERVO,SERVO_MICROS_RIGHT - 100);
                    }
                    else if(read_infrared_sensor(INFRARED_SENSOR_UPPER_RIGHT) && passed_river){
                       servo_control(BAJAJ_SERVO, SERVO_MICROS_LEFT + 100);
                    }
                    //Normal working state
                    else{
                        switch(global_state){
                            case NORMAL:
                                switch(current_slope_zone){
                                    case STARTZONE:
                                        if(!startSong){
                                            START_UP_play;
                                            startSong = true;
                                        }                                       
                                        if(game_zone == DARKGREENZONE && (get_minimize_count(USED_ENCODER) > 10)){
                                            reset_all_encoder();
											START_UP_play;
                                            current_slope_zone = GREENSLOPE1;
                                            strcpy(current_slope_zone_string,"GREENSLOPE1");
                                        } 
                                        else
                                            go_normal();
                                    break;
                                    case GREENSLOPE1:
                                        if((game_zone == ORANGEZONE || game_zone == PINKZONE) && (get_minimize_count(USED_ENCODER) > 4)){
                                            reset_all_encoder();
                                            START_UP_play;
                                            current_slope_zone = ORANGE1;
                                            strcpy(current_slope_zone_string,"ORANGE1");
                                        }  
                                        else
                                            go_normal();
                                            
                                    break;
                                    case ORANGE1:
                                        if(game_zone == DARKGREENZONE && (get_minimize_count(USED_ENCODER) > 4)){
                                            reset_all_encoder();
                                            START_UP_play;
                                            current_slope_zone = GREENSLOPE2;
                                            strcpy(current_slope_zone_string,"GREENSLOPE2");
                                        } 
                                        else
                                            go_normal();
                                    break;
                                    case GREENSLOPE2: 
                                        if((game_zone == ORANGEZONE || game_zone == PINKZONE) && (get_minimize_count(USED_ENCODER) > 4)){
                                            reset_all_encoder();
                                            START_UP_play;
                                            current_slope_zone = ORANGE2;
                                            strcpy(current_slope_zone_string,"ORANGE2");
                                        }
                                        else 
                                            go_normal();                                        
                                    break;
                                    case ORANGE2:
                                        if(game_zone == DARKGREENZONE && (get_minimize_count(USED_ENCODER) > 4)){
                                            reset_all_encoder();
											START_UP_play;
                                            current_slope_zone = GREENSLOPE3;
                                            strcpy(current_slope_zone_string,"GREENSLOPE3");
                                        }
                                        else
                                            go_normal();                                            
                                            
                                    break;
                                    case GREENSLOPE3:
                                        if((game_zone == ORANGEZONE || game_zone == PINKZONE) && (get_minimize_count(USED_ENCODER) > 4)){
                                            current_slope_zone = FINISHEDSLOPE;
											START_UP_play;
                                            strcpy(current_slope_zone_string,"FINISHEDSLOPE");
                                        }   
                                        else
                                            go_normal();
                                    break;
                                    case FINISHEDSLOPE:
                                        switch(full_white){
                                            case 0:
                                                ninety_time_stamp = get_full_ticks();
                                                ardu_cal_ypr[0] = (float)NINETY_IMU;
                                                strcpy(global_state_string,"NINETY DEGREE");
                                                global_state = NINETY;
                                                
                                            break;
                                            case 1:
                                                if((river) && !passed_river)
                                                    {
                                                        START_UP_play;
                                                        reset_all_encoder();
                                                        strcpy(global_state_string,"ENTER_RIVER");  
                                                        time1 = get_full_ticks();
                                                        angle_enter_river = ardu_cal_ypr[0];
                                                        global_state = ENTER_RIVER;
                                                    } 
                                                else if(passed_down_slope && (get_minimize_count(USED_ENCODER)> 10)){
                                                    START_UP_play;
                                                    strcpy(global_state_string,"DOWN SLOPE");
                                                    reset_all_encoder();
                                                    global_state = DOWN_SLOPE;
                                                }
                                                else
                                                    go_normal();
                                            break;
                                        }
                                    break;                                           
                                }
                            break;
                            case NINETY:
                                go_ninety();
                            break;
                            case CHECK_NINETY:
                                finish_ninety();
                            break;
                            case EXIT_RIVER:
                                go_straight_little_bit(); //Prevent it from falling down
                            break;
                            case ENTER_RIVER: //Right before locking the angle with IMU
                                //Stopping condition:
                                if(get_count(USED_ENCODER) > ENTER_RIVER_ENCODER){
                                    switch(side){
                                        case REDSIDE:
											servo_control(BAJAJ_SERVO,SERVO_MICROS_MID + 350);
                                        break;
                                        case BLUESIDE:
                                            servo_control(BAJAJ_SERVO,SERVO_MICROS_MID - 350);
                                        break;
                                    }
                                    strcpy(global_state_string,"ESCAPEISLAND");
                                    START_UP_play;
                                    reset_all_encoder();
                                    time2 = get_full_ticks();
                                    global_state = RIVERING; 
                                }
                                else
                                    go_normal();
                            break;
                            case RIVERING:
                                escape_first_island();
                            break;
                            case RIVERING2:
                                scan_river();
                            break;
                            case DOWN_SLOPE: //End game, make it turn extreme right / left for the hybrid to grip propeller
                                if(get_minimize_count(USED_ENCODER) > 10 && (game_zone != LIGHTGREENZONE)){
                                    enter_time_stamp = get_full_ticks();
                                    strcpy(global_state_string,"FINISH GAME");
                                    //Align servo to middle first before start turning max
                                    last_movement = SERVO_MICROS_MID;
                                    global_state = FINISH;
                                }
                                else
                                    go_normal();
                            break;
                            case FINISH:
                                //Lock the servo angle
                                if((get_full_ticks() - enter_time_stamp) > (int)DELAY){
                                    if(!final_music){
                                        final_music = true;
                                        START_UP_play;
                                    }
                                    switch(side){
                                        case REDSIDE:
                                            servo_control(BAJAJ_SERVO,SERVO_MICROS_MID + 700);
                                        break;
                                        case BLUESIDE:
                                            servo_control(BAJAJ_SERVO,SERVO_MICROS_MID - 700);
                                        break;  
                                    }
                                }
                                else
                                    go_normal();
                            break;          
                        }
                    }
                    print_data(); //Print every data in the on(servo is active) system
                    run_user_interface(); //Button functions
                    break;
               
                case OFF:
                    servo_control(BAJAJ_SERVO,SERVO_MICROS_MID);
                    initalize_values();
                    print_system_off(); //Print every data in off(servo locked in 90 degrees) system
                    break;
              }
              length = 0;
              //Button functions are run by this
              tft_update();
        }
    }
}

