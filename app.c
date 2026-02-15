#include <stdint.h>
#include <stdio.h>
#include "drv_api.h"
#include "fsm_feux.h"

static fsm_feux_state_t etat_position = ST_ETEINTS;
static fsm_feux_state_t etat_croisement = ST_ETEINTS;
static fsm_feux_state_t etat_route = ST_ETEINTS;

static fsm_feux_state_t previous_etat_position = ST_ETEINTS;
static fsm_feux_state_t previous_etat_croisement = ST_ETEINTS;
static fsm_feux_state_t previous_etat_route = ST_ETEINTS;

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
        set_cmd_feux_position(1);
    }else {
        set_cmd_feux_position(0);
    }
    if (trame >= 32){
        trame -= 32;
        set_cmd_feux_croisement(1);
    }else{
        set_cmd_feux_croisement(0);
    }
    if (trame >= 16){
        trame -= 16;
        set_cmd_feux_route(1);
    }else{
        set_cmd_feux_route(0);
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
                            printf("RECU ACQ BGF: ID=1, VAL=%d\n", message.frame[1]);
                            set_acq_feux_position(message.frame[1]);
                            break;
                        case 2:
                            set_acq_feux_croisement(message.frame[1]);
                            break;
                        case 3:
                            set_acq_feux_route(message.frame[1]);
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
    if (previous_etat_position != etat_position){
        serial_frame_t serialData[1];
        serialData[0].serNum = 11;
        serialData[0].frameSize = 2;
        if (get_cmd_feux_position()==0){
            serialData[0].frame[0]=1;
            serialData[0].frame[1]=0;
        }else{
            serialData[0].frame[0]=1;
            serialData[0].frame[1]=1;
        }
        printf("ENVOI BGF: ID=%d, VAL=%d\n", serialData[0].frame[0], serialData[0].frame[1]);
        int32_t res_ser = drv_write_ser(fd_trame, serialData, 1);
    }
    if (previous_etat_croisement != etat_croisement){
        serial_frame_t serialData[1];
        serialData[0].serNum = 11;
        serialData[0].frameSize = 2;
        if (get_cmd_feux_croisement()==0){
            serialData[0].frame[0]=2;
            serialData[0].frame[1]=0;
        }else{
            serialData[0].frame[0]=2;
            serialData[0].frame[1]=1;
        }
        printf("ENVOI BGF: ID=%d, VAL=%d\n", serialData[0].frame[0], serialData[0].frame[1]);
        int32_t res_ser = drv_write_ser(fd_trame, serialData, 1);
    }
    if (previous_etat_route != etat_route){
        serial_frame_t serialData[1];
        serialData[0].serNum = 11;
        serialData[0].frameSize = 2;
        if (get_cmd_feux_route()==0){
            serialData[0].frame[0]=3;
            serialData[0].frame[1]=0;
        }else{
            serialData[0].frame[0]=3;
            serialData[0].frame[1]=1;
        }
        printf("ENVOI BGF: ID=%d, VAL=%d\n", serialData[0].frame[0], serialData[0].frame[1]);
        int32_t res_ser = drv_write_ser(fd_trame, serialData, 1);
    }
    
    uint8_t udpFrame[10]={0};
    if (get_acq_feux_position()==1 && get_cmd_feux_position()==1){
        udpFrame[0]+=128;
    }
    if (get_acq_feux_croisement()==1 && get_cmd_feux_croisement()==1){
        udpFrame[0]+=64;
    }
    if (get_acq_feux_route()==1 && get_cmd_feux_route()==1){
        udpFrame[0]+=32;
    }

    int32_t res_udp = drv_write_udp_200ms(fd_trame, udpFrame);
}

void init_comm(){

}

int main(){
    int32_t fd_trame = drv_open();

    printf("etat_position : %d\n", etat_position);
    printf("etat_croisement : %d\n", etat_croisement);
    printf("etat_route : %d\n", etat_route);

    while (1){
        
        receive_udp_frame(fd_trame);

        previous_etat_position = etat_position;
        fsm_feux_event_t ev_pos = get_next_event(etat_position, FEU_POSITION);
        //printf("ev_pos : %d\n", ev_pos);
        fsm_update(&etat_position, ev_pos);

        // Gestion des Feux de Croisement
        previous_etat_croisement = etat_croisement;
        fsm_feux_event_t ev_crois = get_next_event(etat_croisement, FEU_CROISEMENT);
        fsm_update(&etat_croisement, ev_crois);

        // Gestion des Feux de Route
        previous_etat_route = etat_route;
        fsm_feux_event_t ev_route = get_next_event(etat_route, FEU_ROUTE);
        fsm_update(&etat_route, ev_route);

        printf("etat_position : %d\n", etat_position);
        printf("etat_croisement : %d\n", etat_croisement);
        printf("etat_route : %d\n", etat_route);

        //printf("cmd_position : %d\n", get_cmd_feux_position());

        send_trame(fd_trame);
    }
    drv_close(fd_trame);
}