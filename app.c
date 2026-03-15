/**
 * \file        app.c
 * \author      Alexandra Baivel and Dawid Malicki
 * \brief       This is the main file to start the app
 * \details
 */


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


/**
 * \brief   Decode the serial message and update all the necessary datas
 * \param       frame : The serial message to decode
 * \return  void
 */
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

/**
 * \brief   Decode the udp frame and update all the necessary datas
 * \param       frame : The udp frame to decode
 * \return  void
 */
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


/**
 * \brief   Receive the udp frame and the serial message from the driver
 * \param       fd_frame : The file descriptor of the channel with the driver
 * \return  void
 */
void receive_frames(int32_t fd_frame){
    uint8_t expected_frame_num, current_frame_num;
    uint8_t frame[15];
    serial_frame_t messages[16];
    uint32_t serialDataLen = 16;
    int32_t res_udp;
    uint8_t crc8;
    int32_t res;
    int ack_blk_right, ack_blk_left;
    uint8_t i;
    serial_frame_t message;
    while (1){
        res_udp = drv_read_udp_100ms(fd_frame, frame);
        crc8 = crc_8(frame,14);
        if (res_udp == DRV_SUCCESS && frame[14]==crc8){
            current_frame_num = frame[0];
            expected_frame_num = get_frame_number() + 1;
            if (expected_frame_num == 101){
                expected_frame_num = 1;
            }
            if (current_frame_num != expected_frame_num) {
                printf("Error: Frame sequence mismatch! Expected %d, Got %d\n", 
                       expected_frame_num, current_frame_num);
            }
            set_frame_number(current_frame_num);
            translate_udp_frame(frame);
            res = drv_read_ser(fd_frame, messages, &serialDataLen);
            if (res == DRV_SUCCESS && serialDataLen > 0){
                ack_blk_right =2;
                ack_blk_left=2;
                for (i = 0; i<serialDataLen;i++){
                    message = messages[i];
                    if (message.serNum == SER_NUM_COMODO){
                        translate_serial(message.frame[0]);
                    }else if (message.serNum == SER_NUM_BGF){
                        switch (message.frame[0])
                        {
                        case 1:
                            set_ack_position_lights(1);
                            break;
                        case 2:
                            set_ack_low_beams_headlights(1);
                            break;
                        case 3:
                            set_ack_high_beams_headlights(1);
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
                    set_ack_hazard_lights(1);
                }else if (ack_blk_right != 2){
                    set_ack_right_blinkers(1);
                }else if (ack_blk_left != 2){
                    set_ack_left_blinkers(1);
                }
            }
            return;
        }
    }
}


/**
 * \brief   Send the udp frame and the serial message to the driver
 * \param       fd_frame : The file descriptor of the channel with the driver
 * \return  void
 */
void send_frames(int32_t fd_frame){
    serial_frame_t serialData[2];
    int32_t res_ser;
    uint8_t udpFrame[10]={0};
    int32_t res_udp;

    //Sending serial messages

    //Sending serial message for the position lights only if its state has changed
    if (previous_state_position != state_position){
        serialData[0].serNum = SER_NUM_BGF;
        serialData[0].frameSize = 2;
        if (state_position == ST_LIGHTS_OFF || state_position == ST_LIGHTS_ERROR){
            serialData[0].frame[0]=POSITION_LIGHTS_ACTIVATION;
            serialData[0].frame[1]=0;
        }else{
            serialData[0].frame[0]=POSITION_LIGHTS_ACTIVATION;
            serialData[0].frame[1]=1;
        }
        res_ser = drv_write_ser(fd_frame, serialData, 1);
        if (res_ser == DRV_ERROR){
            printf("Error: There has been a error while writing data on serial lines");
        }
    }
    //Sending serial message for the low beams headlights only if its state has changed
    if (previous_state_low_beams != state_low_beams){
        serialData[0].serNum = SER_NUM_BGF;
        serialData[0].frameSize = 2;
        if (state_low_beams == ST_LIGHTS_OFF || state_low_beams == ST_LIGHTS_ERROR){
            serialData[0].frame[0]=LOW_BEAMS_HEADLIGHTS_ACTIVATION;
            serialData[0].frame[1]=0;
        }else{
            serialData[0].frame[0]=LOW_BEAMS_HEADLIGHTS_ACTIVATION;
            serialData[0].frame[1]=1;
        }
        res_ser = drv_write_ser(fd_frame, serialData, 1);
        if (res_ser == DRV_ERROR){
            printf("Error: There has been a error while writing data on serial lines");
        }
    }
    //Sending serial message for the high beams headlights only if its state has changed
    if (previous_state_high_beams != state_high_beams){
        serialData[0].serNum = SER_NUM_BGF;
        serialData[0].frameSize = 2;
        if (state_high_beams == ST_LIGHTS_OFF || state_high_beams == ST_LIGHTS_ERROR){
            serialData[0].frame[0]=HIGH_BEAMS_HEADLIGHTS_ACTIVATION;
            serialData[0].frame[1]=0;
        }else{
            serialData[0].frame[0]=HIGH_BEAMS_HEADLIGHTS_ACTIVATION;
            serialData[0].frame[1]=1;
        }
        res_ser = drv_write_ser(fd_frame, serialData, 1);
        if (res_ser == DRV_ERROR){
            printf("Error: There has been a error while writing data on serial lines");
        }
    }
    //Sending serial message for the hazard lights only if its state has changed
    if (previous_state_hazard_lights != state_hazard_lights){
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
        res_ser = drv_write_ser(fd_frame, serialData, 2);
        if (res_ser == DRV_ERROR){
            printf("Error: There has been a error while writing data on serial lines");
        }
    }
    //Sending serial message for the right blinker only if its state has changed
    if (previous_state_right_blinkers != state_right_blinkers){
        serialData[0].serNum = SER_NUM_BGF;
        serialData[0].frameSize = 2;
        if (state_right_blinkers == ST_BLINKERS_OFF || state_right_blinkers == ST_BLINKERS_ACTIVATED_OFF || state_right_blinkers == ST_BLINKERS_ACK_OFF || state_right_blinkers == ST_BLINKERS_ERROR){
            serialData[0].frame[0]=RIGHT_BLINKERS_ACTIVATION;
            serialData[0].frame[1]=0;
        }else{
            serialData[0].frame[0]=RIGHT_BLINKERS_ACTIVATION;
            serialData[0].frame[1]=1;
        }
        res_ser = drv_write_ser(fd_frame, serialData, 1);
        if (res_ser == DRV_ERROR){
            printf("Error: There has been a error while writing data on serial lines");
        }
    }
    //Sending serial message for the left blinker only if its state has changed
    if (previous_state_left_blinkers != state_left_blinkers){
        serialData[0].serNum = SER_NUM_BGF;
        serialData[0].frameSize = 2;
        if (state_left_blinkers == ST_BLINKERS_OFF || state_left_blinkers == ST_BLINKERS_ACTIVATED_OFF || state_left_blinkers == ST_BLINKERS_ACK_OFF || state_left_blinkers == ST_BLINKERS_ERROR){
            serialData[0].frame[0]=LEFT_BLINKERS_ACTIVATION;
            serialData[0].frame[1]=0;
        }else{
            serialData[0].frame[0]=LEFT_BLINKERS_ACTIVATION;
            serialData[0].frame[1]=1;
        }
        res_ser = drv_write_ser(fd_frame, serialData, 1);
        if (res_ser == DRV_ERROR){
            printf("Error: There has been a error while writing data on serial lines");
        }
    }

    //Sending the udp frame

    if (get_ack_position_lights()==1 && get_cmd_position_lights()==1){
        udpFrame[0]+=128;
    }
    if (get_ack_low_beams_headlights()==1 && get_cmd_low_beams_headlights()==1){
        udpFrame[0]+=64;
    }
    if (get_ack_high_beams_headlights()==1 && get_cmd_high_beams_headlights()==1){
        udpFrame[0]+=32;
    }
    if (get_tank_level() <= 2){ //5% of 40 = 2
        udpFrame[0]+=16;
    }
    if (get_engine_problems() != NO_ENGINE_PROBLEM){
        udpFrame[0]+=8;
    }
    if (get_chassis_problems() == TIRE_PRESSURE){
        udpFrame[0]+=4;
    }
    if (get_battery_problems() == DISCHARGED){
        udpFrame[0]+=1;
    }

    if (state_hazard_lights == ST_BLINKERS_ACTIVATED_ON || state_hazard_lights == ST_BLINKERS_ACK_ON){ //the warning light on the dashboard must blink with the blinkers
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

    revolutions_minute_t rev_min = get_revolutions_minute()/10;
    udpFrame[8] = (rev_min  >> 8)  & 0xFF;
    udpFrame[9] = rev_min & 0xFF;

    //print_frame(udpFrame, 10);

    res_udp = drv_write_udp_200ms(fd_frame, udpFrame);
    if (res_udp == DRV_ERROR){
        printf("Error: There has been a error while writing UDP 200ms frame");
    }
}

int main(){
    fsm_lights_event_t ev_pos;
    fsm_lights_event_t ev_low_beams;
    fsm_lights_event_t ev_high_beams;
    fsm_blinkers_event_t ev_haz;
    fsm_blinkers_event_t ev_left_blink;
    fsm_blinkers_event_t ev_right_blink;
    fsm_wipers_event_t ev_wipers;
    int32_t fd_frame = drv_open();

    while (1){
        
        receive_frames(fd_frame);

        // Management of position lights
        previous_state_position = state_position;
        ev_pos = get_lights_next_event(state_position, POSITION_LIGHTS);
        fsm_lights_update(&state_position, ev_pos);

        // Management of low beams headlights
        previous_state_low_beams = state_low_beams;
        ev_low_beams = get_lights_next_event(state_low_beams, LOW_BEAMS_HEADLIGHTS);
        fsm_lights_update(&state_low_beams, ev_low_beams);

        // Management of high beams headlights
        previous_state_high_beams = state_high_beams;
        ev_high_beams = get_lights_next_event(state_high_beams, HIGH_BEAMS_HEADLIGHTS);
        fsm_lights_update(&state_high_beams, ev_high_beams);

        // Management of hazard lights
        previous_state_hazard_lights = state_hazard_lights;
        ev_haz = get_blinkers_next_event(state_hazard_lights, HAZARD_LIGHTS);
        fsm_blinkers_update(&state_hazard_lights, ev_haz);

        // Management of left blinker
        previous_state_left_blinkers = state_left_blinkers;
        ev_left_blink = get_blinkers_next_event(state_left_blinkers, LEFT_BLINKERS);
        fsm_blinkers_update(&state_left_blinkers, ev_left_blink);

        // Management of right blinker
        previous_state_right_blinkers = state_right_blinkers;
        ev_right_blink = get_blinkers_next_event(state_right_blinkers, RIGHT_BLINKERS);
        fsm_blinkers_update(&state_right_blinkers, ev_right_blink);
        
        // Management of wipers
        previous_state_wipers = state_wipers;
        ev_wipers = get_wipers_next_event(state_wipers); 
        fsm_wipers_update(&state_wipers, ev_wipers);

        send_frames(fd_frame);
    }
    drv_close(fd_frame);
}