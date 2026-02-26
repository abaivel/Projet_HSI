#include <stdint.h>
#include <stdio.h>
#include "drv_api.h"
#include "fsm_feux.h"
#include "fsm_clignotants.h"
#include "fsm_essuie_glaces.h"

static fsm_feux_state_t state_position = ST_ETEINTS;
static fsm_feux_state_t state_low_beams = ST_ETEINTS;
static fsm_feux_state_t state_high_beams = ST_ETEINTS;
static fsm_blinkers_state_t state_hazard_lights = ST_ETEINTS;
static fsm_blinkers_state_t state_left_blinkers = ST_ETEINTS;
static fsm_blinkers_state_t state_right_blinkers = ST_ETEINTS;

static fsm_feux_state_t previous_state_position = ST_ETEINTS;
static fsm_feux_state_t previous_state_low_beams = ST_ETEINTS;
static fsm_feux_state_t previous_state_high_beams = ST_ETEINTS;
static fsm_blinkers_state_t previous_state_hazard_lights = ST_ETEINTS;
static fsm_blinkers_state_t previous_state_left_blinkers = ST_ETEINTS;
static fsm_blinkers_state_t previous_state_right_blinkers = ST_ETEINTS;

static int index = 0;

void print_trame(uint8_t trame[], int size_trame){
    for (int i = 0;i<size_trame;i++){
        printf("%hhu ",trame[i]);
    }
    printf("\n");
}

void print_message(serial_frame_t message[], int size_message){
    for (int i=0;i<size_message;i++){
        printf("\nsize: %ld\n", message[i].frameSize);
        //printf("%d\n", message[i].serNum);
        print_trame(message[i].frame,2);
    }
}

void translate_serial(uint8_t trame){
    if (trame >= 128){
        trame -= 128;
    }
    if (trame >= 64){
        trame -=64;
        set_cmd_position_lights(1);
    }else {
        set_cmd_position_lights(0);
    }
    if (trame >= 32){
        trame -= 32;
        set_cmd_low_beams_headlights(1);
    }else{
        set_cmd_low_beams_headlights(0);
    }
    if (trame >= 16){
        trame -= 16;
        set_cmd_high_beams_headlights(1);
    }else{
        set_cmd_high_beams_headlights(0);
    }
    if (trame >= 8){
        trame -= 8;
    }
    if (trame >= 4){
        trame -= 4;
    }
    if (trame >= 2){
        trame -= 2;
    }
    if (trame == 1){
        trame -= 1;
    }
}

void receive_udp_frame(int32_t fd_trame){
    while (1){
        uint8_t trame[15];
        serial_frame_t messages[16];
        uint32_t serialDataLen = 16;
        int32_t res_udp = drv_read_udp_100ms(fd_trame, trame);
        if (res_udp == DRV_SUCCESS){
            //print_trame(trame,15);
            int32_t res = drv_read_ser(fd_trame, messages, &serialDataLen);
            if (res == DRV_SUCCESS && serialDataLen > 0){
                for (int i = 0; i<serialDataLen;i++){
                    serial_frame_t message = messages[i];
                    if (message.frameSize == 1){
                        //printf("serNum=%d\n", message.serNum);
                        print_trame(message.frame,message.frameSize);
                        translate_serial(message.frame[0]);
                    }else if (message.frameSize > 1){
                        printf("ACQ:\n");
                        print_trame(message.frame,message.frameSize);
                        switch (message.frame[0])
                        {
                        case 1:
                            printf("RECIVED ACQ BGF: ID=1, VAL=%d\n", message.frame[1]);
                            set_acq_position_lights(message.frame[1]);
                            break;
                        case 2:
                            set_acq_low_beams_headlights(message.frame[1]);
                            break;
                        case 3:
                            set_acq_high_beams_headlights(message.frame[1]);
                            break;
                        default:
                            break;
                        }
                    }
                }
            }
            return;
        }
    }
}

void send_trame(int32_t fd_trame){
    if (previous_state_position != state_position){
        serial_frame_t serialData[1];
        serialData[0].serNum = 11;
        serialData[0].frameSize = 2;
        if (get_cmd_position_lights()==0){
            serialData[0].frame[0]=1;
            serialData[0].frame[1]=0;
        }else{
            serialData[0].frame[0]=1;
            serialData[0].frame[1]=1;
        }
        printf("SEND BGF: ID=%d, VAL=%d\n", serialData[0].frame[0], serialData[0].frame[1]);
        int32_t res_ser = drv_write_ser(fd_trame, serialData, 1);
    }
    if (previous_state_low_beams != state_low_beams){
        serial_frame_t serialData[1];
        serialData[0].serNum = 11;
        serialData[0].frameSize = 2;
        if (get_cmd_low_beams_headlights()==0){
            serialData[0].frame[0]=2;
            serialData[0].frame[1]=0;
        }else{
            serialData[0].frame[0]=2;
            serialData[0].frame[1]=1;
        }
        printf("SEND BGF: ID=%d, VAL=%d\n", serialData[0].frame[0], serialData[0].frame[1]);
        int32_t res_ser = drv_write_ser(fd_trame, serialData, 1);
    }
    if (previous_state_high_beams != state_high_beams){
        serial_frame_t serialData[1];
        serialData[0].serNum = 11;
        serialData[0].frameSize = 2;
        if (get_cmd_high_beams_headlights()==0){
            serialData[0].frame[0]=3;
            serialData[0].frame[1]=0;
        }else{
            serialData[0].frame[0]=3;
            serialData[0].frame[1]=1;
        }
        printf("SEND BGF: ID=%d, VAL=%d\n", serialData[0].frame[0], serialData[0].frame[1]);
        int32_t res_ser = drv_write_ser(fd_trame, serialData, 1);
    }
    
    uint8_t udpFrame[10]={0};
    if (get_acq_position_lights()==1 && get_cmd_position_lights()==1){
        udpFrame[0]+=128;
    }
    if (get_acq_low_beams_headlights()==1 && get_cmd_low_beams_headlights()==1){
        udpFrame[0]+=64;
    }
    if (get_acq_high_beams_headlights()==1 && get_cmd_high_beams_headlights()==1){
        udpFrame[0]+=32;
    }

    int32_t res_udp = drv_write_udp_200ms(fd_trame, udpFrame);
}

void init_comm(){

}

int main(){
    int32_t fd_trame = drv_open();

    printf("state_position : %d\n", state_position);
    printf("state_low_beams : %d\n", state_low_beams);
    printf("state_high_beams : %d\n", state_high_beams);

    while (1){
        
        receive_udp_frame(fd_trame);

        previous_state_position = state_position;
        fsm_feux_event_t ev_pos = get_next_event(state_position, POSITION_LIGHTS);
        //printf("ev_pos : %d\n", ev_pos);
        fsm_update(&state_position, ev_pos);

        // managment of low beams headlights
        previous_state_low_beams = state_low_beams;
        fsm_feux_event_t ev_crois = get_next_event(state_low_beams, LOW_BEAMS_HEADLIGHTS);
        fsm_update(&state_low_beams, ev_crois);

        // Managment of high beams headlights
        previous_state_high_beams = state_high_beams;
        fsm_feux_event_t ev_route = get_next_event(state_high_beams, HIGH_BEAMS_HEADLIGHTS);
        fsm_update(&state_high_beams, ev_route);

        printf("state_position : %d\n", state_position);
        printf("state_low_beams : %d\n", state_low_beams);
        printf("state_high_beams : %d\n", state_high_beams);

        //printf("cmd_position : %d\n", get_cmd_position_lights());

        send_trame(fd_trame);
    }
    drv_close(fd_trame);
}