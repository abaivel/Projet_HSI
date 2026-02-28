#include <stdint.h>
#include <stdio.h>
#include "drv_api.h"
#include "fsm_lights.h"
#include "fsm_blinkers.h"
#include "fsm_wipers.h"
#include "checksum.h"

static fsm_lights_state_t state_position = ST_LIGHTS_OFF;
static fsm_lights_state_t state_low_beams = ST_LIGHTS_OFF;
static fsm_lights_state_t state_high_beams = ST_LIGHTS_OFF;
static fsm_blinkers_state_t state_hazard_lights = ST_BLINKERS_OFF;
static fsm_blinkers_state_t state_left_blinkers = ST_BLINKERS_OFF;
static fsm_blinkers_state_t state_right_blinkers = ST_BLINKERS_OFF;
static fsm_wipers_state_t state_wipers = ST_WIPERS_WASHERS_ALL_OFF;

static fsm_lights_state_t previous_state_position = ST_LIGHTS_OFF;
static fsm_lights_state_t previous_state_low_beams = ST_LIGHTS_OFF;
static fsm_lights_state_t previous_state_high_beams = ST_LIGHTS_OFF;
static fsm_blinkers_state_t previous_state_hazard_lights = ST_BLINKERS_OFF;
static fsm_blinkers_state_t previous_state_left_blinkers = ST_BLINKERS_OFF;
static fsm_blinkers_state_t previous_state_right_blinkers = ST_BLINKERS_OFF;
static fsm_wipers_state_t previous_state_wipers = ST_WIPERS_WASHERS_ALL_OFF;

static int index = 0;

void print_frame(uint8_t frame[], int size_frame){
    for (int i = 0;i<size_frame;i++){
        printf("%hhu ",frame[i]);
    }
    printf("\n");
}

void print_message(serial_frame_t message[], int size_message){
    for (int i=0;i<size_message;i++){
        printf("\nsize: %ld\n", message[i].frameSize);
        //printf("%d\n", message[i].serNum);
        print_frame(message[i].frame,2);
    }
}

void translate_serial(uint8_t frame){
    if (frame >= 128){
        frame -= 128;
        set_cmd_hazard_lights(1);
    }else{
        set_cmd_hazard_lights(0);
    }
    if (frame >= 64){
        frame -=64;
        set_cmd_position_lights(1);
    }else {
        set_cmd_position_lights(0);
    }
    if (frame >= 32){
        frame -= 32;
        set_cmd_low_beams_headlights(1);
    }else{
        set_cmd_low_beams_headlights(0);
    }
    if (frame >= 16){
        frame -= 16;
        set_cmd_high_beams_headlights(1);
    }else{
        set_cmd_high_beams_headlights(0);
    }
    if (frame >= 8){
        frame -= 8;
        set_cmd_right_blinkers(1);
    }else{
        set_cmd_right_blinkers(0);
    }
    if (frame >= 4){
        frame -= 4;
        set_cmd_left_blinkers(1);
    }else{
        set_cmd_left_blinkers(0);
    }
    if (frame >= 2){
        frame -= 2;
        set_cmd_wipers(1);
    }else{
        set_cmd_wipers(0);
    }
    if (frame == 1){
        frame -= 1;
        set_cmd_windshield_washer(1);
    }else{
        set_cmd_windshield_washer(0);
    }

}

void translate_udp_frame(uint8_t frame[]){
    set_mileage(((uint32_t)frame[1] << 24) |
    ((uint32_t)frame[2] << 16) |
    ((uint32_t)frame[3] << 8)  |
    ((uint32_t)frame[4]));

    set_speed(frame[5]);
    
    set_chassis_problems(frame[6]);

    set_engine_problems(frame[7]);

    set_tank_level(frame[8]);

    set_revolutions_minute(((uint32_t)frame[9] << 24) |
    ((uint32_t)frame[10] << 16) |
    ((uint32_t)frame[11] << 8)  |
    ((uint32_t)frame[12]));

    set_battery_problems(frame[13]);


}

void receive_udp_frame(int32_t fd_frame){
    static uint8_t previous_frame_num = 0; 
    
    while (1){
        uint8_t frame[15];
        serial_frame_t messages[16];
        uint32_t serialDataLen = 16;
        int32_t res_udp = drv_read_udp_100ms(fd_frame, frame);
        uint8_t crc8 = crc_8(frame,14);
        if (res_udp == DRV_SUCCESS && frame[14]==crc8){
            uint8_t current_frame_num = frame[0];
            if (current_frame_num != (uint8_t)((get_thread_number() + 1)%100)) {
                printf("Error: Frame sequence mismatch! Expected %d, Got %d\n", 
                       (uint8_t)((get_thread_number() + 1)%100), current_frame_num);
            }
            set_thread_number(current_frame_num);
            translate_udp_frame(frame);
            int32_t res = drv_read_ser(fd_frame, messages, &serialDataLen);
            if (res == DRV_SUCCESS && serialDataLen > 0){
                int ack_blk_right =2, ack_blk_left=2;
                for (int i = 0; i<serialDataLen;i++){
                    serial_frame_t message = messages[i];
                    if (message.serNum == SER_NUM_COMODO){
                         translate_serial(message.frame[0]);
                    }else if (message.serNum == SER_NUM_BGF){
                        switch (message.frame[0])
                        {
                        case 1:
                            set_ack_position_lights(message.frame[1]);
                            break;
                        case 2:
                            set_ack_low_beams_headlights(message.frame[1]);
                            break;
                        case 3:
                            set_ack_high_beams_headlights(message.frame[1]);
                            break;
                        case 4:
                            ack_blk_right = 1;
                            break;
                        case 5:
                            ack_blk_left = 1;
                            break;
                        default:
                            break;
                        }
                    }
                }
                if (ack_blk_left !=2 && ack_blk_right !=2 && ack_blk_left == ack_blk_right){
                    set_ack_hazard_lights(ack_blk_left);
                }else if (ack_blk_right != 2){
                    set_ack_right_blinkers(ack_blk_right);
                }else if (ack_blk_left != 2){
                    set_ack_left_blinkers(ack_blk_left);
                }
            }else if (frame[14]!=crc_8(frame,14)){
                printf("UDP frame NOT TRANSLATED BECAUSE CRC8\n");
            }
            return;
        }
    }
}

void send_frame(int32_t fd_frame){
    if (previous_state_position != state_position){
        serial_frame_t serialData[1];
        serialData[0].serNum = SER_NUM_BGF;
        serialData[0].frameSize = 2;
        if (state_position == ST_LIGHTS_OFF || state_position == ST_LIGHTS_ERROR){
            serialData[0].frame[0]=POSITION_LIGHTS_ACTIVATION;
            serialData[0].frame[1]=0;
        }else{
            serialData[0].frame[0]=POSITION_LIGHTS_ACTIVATION;
            serialData[0].frame[1]=1;
        }
        //printf("SEND BGF: ID=%d, VAL=%d\n", serialData[0].frame[0], serialData[0].frame[1]);
        int32_t res_ser = drv_write_ser(fd_frame, serialData, 1);
    }
    if (previous_state_low_beams != state_low_beams){
        serial_frame_t serialData[1];
        serialData[0].serNum = SER_NUM_BGF;
        serialData[0].frameSize = 2;
        if (state_low_beams == ST_LIGHTS_OFF || state_low_beams == ST_LIGHTS_ERROR){
            serialData[0].frame[0]=LOW_BEAMS_HEADLIGHTS_ACTIVATION;
            serialData[0].frame[1]=0;
        }else{
            serialData[0].frame[0]=LOW_BEAMS_HEADLIGHTS_ACTIVATION;
            serialData[0].frame[1]=1;
        }
        //printf("SEND BGF: ID=%d, VAL=%d\n", serialData[0].frame[0], serialData[0].frame[1]);
        int32_t res_ser = drv_write_ser(fd_frame, serialData, 1);
    }
    if (previous_state_high_beams != state_high_beams){
        serial_frame_t serialData[1];
        serialData[0].serNum = SER_NUM_BGF;
        serialData[0].frameSize = 2;
        if (state_high_beams == ST_LIGHTS_OFF || state_high_beams == ST_LIGHTS_ERROR){
            serialData[0].frame[0]=HIGH_BEAMS_HEADLIGHTS_ACTIVATION;
            serialData[0].frame[1]=0;
        }else{
            serialData[0].frame[0]=HIGH_BEAMS_HEADLIGHTS_ACTIVATION;
            serialData[0].frame[1]=1;
        }
        //printf("SEND BGF: ID=%d, VAL=%d\n", serialData[0].frame[0], serialData[0].frame[1]);
        int32_t res_ser = drv_write_ser(fd_frame, serialData, 1);
    }
    if (previous_state_hazard_lights != state_hazard_lights){
        serial_frame_t serialData[2];
        serialData[0].serNum = SER_NUM_BGF;
        serialData[0].frameSize = 2;
        serialData[1].serNum = SER_NUM_BGF;
        serialData[1].frameSize = 2;
        if (state_hazard_lights == ST_BLINKERS_OFF || state_hazard_lights == ST_BLINKERS_ACTIVATED_OFF || state_hazard_lights == ST_BLINKERS_ACK_OFF || state_hazard_lights == ST_BLINKERS_ERROR){
            serialData[0].frame[0]=RIGHT_BLINKERS_ACTIVATION;
            serialData[0].frame[1]=0;
            serialData[1].frame[0]=LEFT_BLINKERS_ACTIVATION;
            serialData[1].frame[1]=0;
        }else{
            serialData[0].frame[0]=RIGHT_BLINKERS_ACTIVATION;
            serialData[0].frame[1]=1;
            serialData[1].frame[0]=LEFT_BLINKERS_ACTIVATION;
            serialData[1].frame[1]=1;
        }
        //printf("SEND BGF: ID=%d, VAL=%d\n", serialData[0].frame[0], serialData[0].frame[1]);
        int32_t res_ser = drv_write_ser(fd_frame, serialData, 2);
    }
    if (previous_state_right_blinkers != state_right_blinkers){
        serial_frame_t serialData[1];
        serialData[0].serNum = SER_NUM_BGF;
        serialData[0].frameSize = 2;
        if (state_right_blinkers == ST_BLINKERS_OFF || state_right_blinkers == ST_BLINKERS_ACTIVATED_OFF || state_right_blinkers == ST_BLINKERS_ACK_OFF || state_right_blinkers == ST_BLINKERS_ERROR){
            serialData[0].frame[0]=RIGHT_BLINKERS_ACTIVATION;
            serialData[0].frame[1]=0;
        }else{
            serialData[0].frame[0]=RIGHT_BLINKERS_ACTIVATION;
            serialData[0].frame[1]=1;
        }
        //printf("SEND BGF: ID=%d, VAL=%d\n", serialData[0].frame[0], serialData[0].frame[1]);
        int32_t res_ser = drv_write_ser(fd_frame, serialData, 1);
    }
    if (previous_state_left_blinkers != state_left_blinkers){
        serial_frame_t serialData[1];
        serialData[0].serNum = SER_NUM_BGF;
        serialData[0].frameSize = 2;
        if (state_left_blinkers == ST_BLINKERS_OFF || state_left_blinkers == ST_BLINKERS_ACTIVATED_OFF || state_left_blinkers == ST_BLINKERS_ACK_OFF || state_left_blinkers == ST_BLINKERS_ERROR){
            serialData[0].frame[0]=LEFT_BLINKERS_ACTIVATION;
            serialData[0].frame[1]=0;
        }else{
            serialData[0].frame[0]=LEFT_BLINKERS_ACTIVATION;
            serialData[0].frame[1]=1;
        }
        //printf("SEND BGF: ID=%d, VAL=%d\n", serialData[0].frame[0], serialData[0].frame[1]);
        int32_t res_ser = drv_write_ser(fd_frame, serialData, 1);
    }
    
    uint8_t udpFrame[10]={0};
    if (get_ack_position_lights()==1 && get_cmd_position_lights()==1){
        udpFrame[0]+=128;
    }
    if (get_ack_low_beams_headlights()==1 && get_cmd_low_beams_headlights()==1){
        udpFrame[0]+=64;
    }
    if (get_ack_high_beams_headlights()==1 && get_cmd_high_beams_headlights()==1){
        udpFrame[0]+=32;
    }
    if (get_tank_level() <= 2){
        udpFrame[0]+=16;
    }
    if (get_engine_problems() != NO_ENGINE_PROBLEM){
        udpFrame[0]+=8;
    }
    if (get_chassis_problems() == PRESSURE_DEFAULT){
        udpFrame[0]+=4;
    }
    if (get_battery_problems() == DISCHARGED){
        udpFrame[0]+=1;
    }

    if (state_hazard_lights != ST_BLINKERS_OFF && state_hazard_lights != ST_BLINKERS_ERROR){
        udpFrame[1]+=128;
    }
    if (get_battery_problems() == FAILURE){
        udpFrame[1]+=64;
    }
    if (get_engine_problems() == COOLANT_TEMPERATURE){
        udpFrame[1]+=32;
    }
    if (get_engine_problems() == PRESSURE_DEFAULT){
        udpFrame[1]+=16;
    }
    if (get_engine_problems() == OIL_OVERHEATING){
        udpFrame[1]+=8;
    }
    if (get_chassis_problems() == BRAKE_FAILURE){
        udpFrame[1]+=4;
    }
    if (state_wipers == ST_WIPERS_ACTIVE || state_wipers == ST_WIPERS_WASHERS_ACTIVE || state_wipers == ST_WIPERS_WASHERS_TIMER_OFF) {
        udpFrame[1] += 2;
    }
    if (state_wipers == ST_WIPERS_WASHERS_ACTIVE || state_wipers == ST_WIPERS_WASHERS_TIMER_OFF) {
        udpFrame[1] += 1;
    }

    udpFrame[2]   = (get_mileage() >> 24) & 0xFF;
    udpFrame[3] = (get_mileage()  >> 16) & 0xFF;
    udpFrame[4] = (get_mileage()  >> 8)  & 0xFF;
    udpFrame[5] = get_mileage() & 0xFF;

    udpFrame[6] = get_speed();
    udpFrame[7] = (get_tank_level()*100)/40;

    udpFrame[8] = (get_revolutions_minute()  >> 8)  & 0xFF;
    udpFrame[9] = get_revolutions_minute() & 0xFF;

    //print_frame(udpFrame, 10);

    int32_t res_udp = drv_write_udp_200ms(fd_frame, udpFrame);
}

void init_comm(){

}

int main(){
    int32_t fd_frame = drv_open();

    /*printf("state_position : %d\n", state_position);
    printf("state_low_beams : %d\n", state_low_beams);
    printf("state_high_beams : %d\n", state_high_beams);
    printf("state_hazard_lights : %d\n", state_hazard_lights);
    printf("state_left_blinkers : %d\n", state_left_blinkers);
    printf("state_right_blinkers : %d\n", state_right_blinkers);
    printf("state_wipers : %d\n", state_wipers);*/

    while (1){
        
        receive_udp_frame(fd_frame);

        previous_state_position = state_position;
        fsm_lights_event_t ev_pos = get_lights_next_event(state_position, POSITION_LIGHTS);
        //printf("ev_pos : %d\n", ev_pos);
        fsm_lights_update(&state_position, ev_pos);

        // managment of low beams headlights
        previous_state_low_beams = state_low_beams;
        fsm_lights_event_t ev_crois = get_lights_next_event(state_low_beams, LOW_BEAMS_HEADLIGHTS);
        fsm_lights_update(&state_low_beams, ev_crois);

        // Managment of high beams headlights
        previous_state_high_beams = state_high_beams;
        fsm_lights_event_t ev_route = get_lights_next_event(state_high_beams, HIGH_BEAMS_HEADLIGHTS);
        fsm_lights_update(&state_high_beams, ev_route);

        previous_state_hazard_lights = state_hazard_lights;
        fsm_blinkers_event_t ev_haz = get_blinkers_next_event(state_hazard_lights, HAZARD_LIGHTS);
        fsm_blinkers_update(&state_hazard_lights, ev_haz);

        previous_state_left_blinkers = state_left_blinkers;
        fsm_blinkers_event_t ev_left_blink = get_blinkers_next_event(state_left_blinkers, LEFT_BLINKERS);
        fsm_blinkers_update(&state_left_blinkers, ev_left_blink);

        previous_state_right_blinkers = state_right_blinkers;
        fsm_blinkers_event_t ev_right_blink = get_blinkers_next_event(state_right_blinkers, RIGHT_BLINKERS);
        fsm_blinkers_update(&state_right_blinkers, ev_right_blink);
        
        previous_state_wipers = state_wipers;
        fsm_wipers_event_t ev_wipers = get_wipers_next_event(state_wipers); 
        fsm_wipers_update(&state_wipers, ev_wipers);

        /*printf("state_position : %d\n", state_position);
        printf("state_low_beams : %d\n", state_low_beams);
        printf("state_high_beams : %d\n", state_high_beams);
        printf("state_hazard_lights : %d\n", state_hazard_lights);
        printf("state_left_blinkers : %d\n", state_left_blinkers);
        printf("state_right_blinkers : %d\n", state_right_blinkers);
        printf("state_wipers : %d\n", state_wipers);*/

        send_frame(fd_frame);
    }
    drv_close(fd_frame);
}