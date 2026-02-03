#include <stdint.h>
#include <stdio.h>
#include "drv_api.h"

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

int main(){
    int32_t fd_trame = drv_open();
    int i =0;
    while (1){
        uint8_t trame[15];
        serial_frame_t messages[16];
        uint32_t serialDataLen = 16;
        int32_t res_udp = drv_read_udp_100ms(fd_trame, trame);
        
        if (res_udp == DRV_SUCCESS){
            print_trame(trame,15);
            int32_t res = drv_read_ser(fd_trame, messages, &serialDataLen);
            serial_frame_t message = messages[0];
            if (res == DRV_SUCCESS && serialDataLen > 0){
                print_trame(message.frame,message.frameSize);
            }
        }
        i++;
    }
    drv_close(fd_trame);
}