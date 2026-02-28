#include "types.h"

static t_vehicule_data position_headlights;
static t_vehicule_data low_beams_headlights;
static t_vehicule_data high_beams_headlights;

uint8_t get_cmd_position_headlights(){
    return position_headlights.cmd;
}

uint8_t get_ack_position_headlights(){
    return position_headlights.ack;
}

uint32_t get_timer_position_headlights(){
    return position_headlights.timer;
}

uint8_t get_cmd_low_beams_headlights(){
    return low_beams_headlights.cmd;
}

uint8_t get_ack_low_beams_headlights(){
    return low_beams_headlights.ack;
}

uint32_t get_timer_low_beams_headlights(){
    return low_beams_headlights.timer;
}

uint8_t get_cmd_high_beams_headlights(){
    return high_beams_headlights.cmd;
}

uint8_t get_ack_high_beams_headlights(){
    return high_beams_headlights.ack;
}

uint32_t get_timer_high_beams_headlights(){
    return high_beams_headlights.timer;
}

void set_cmd_position_headlights(uint8_t cmd){
    position_headlights.cmd = cmd;
}

void set_ack_position_headlights(uint8_t ack){
    position_headlights.ack = ack;
}

void set_timer_position_headlights(uint32_t timer){
    position_headlights.timer = timer;
}

void set_cmd_low_beams_headlightsuint8_t(uint8_t cmd){
    low_beams_headlights.cmd = cmd;
}

void set_ack_low_beams_headlightsuint8_t(uint8_t ack){
    low_beams_headlights.ack = ack;
}

void set_timer_low_beams_headlightsuint32_t(uint8_t timer){
    low_beams_headlights.timer = timer;
}

void set_cmd_high_beams_headlights(uint8_t cmd){
    high_beams_headlights.cmd = cmd;
}

void set_ack_high_beams_headlights(uint8_t ack){
    high_beams_headlights.ack = ack;
}

void set_timer_high_beams_headlights(uint32_t timer){
    high_beams_headlights.timer = timer;
}