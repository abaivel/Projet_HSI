#include "types.h"

static t_vehicule_data feux_position;
static t_vehicule_data feux_croisement;
static t_vehicule_data feux_route;

uint8_t get_cmd_feux_position(){
    return feux_position.cmd;
}

uint8_t get_acq_feux_position(){
    return feux_position.acq;
}

uint32_t get_timer_feux_position(){
    return feux_position.timer;
}

uint8_t get_cmd_feux_croisement(){
    return feux_croisement.cmd;
}

uint8_t get_acq_feux_croisement(){
    return feux_croisement.acq;
}

uint32_t get_timer_feux_croisement(){
    return feux_croisement.timer;
}

uint8_t get_cmd_feux_route(){
    return feux_route.cmd;
}

uint8_t get_acq_feux_route(){
    return feux_route.acq;
}

uint32_t get_timer_feux_route(){
    return feux_route.timer;
}

void set_cmd_feux_position(uint8_t cmd){
    feux_position.cmd = cmd;
}

void set_acq_feux_position(uint8_t acq){
    feux_position.acq = acq;
}

void set_timer_feux_position(uint32_t timer){
    feux_position.timer = timer;
}

void set_cmd_feux_croisement(uint8_t cmd){
    feux_croisement.cmd = cmd;
}

void set_acq_feux_croisement(uint8_t acq){
    feux_croisement.acq = acq;
}

void set_timer_feux_croisement(uint32_t timer){
    feux_croisement.timer = timer;
}

void set_cmd_feux_route(uint8_t cmd){
    feux_route.cmd = cmd;
}

void set_acq_feux_route(uint8_t acq){
    feux_route.acq = acq;
}

void set_timer_feux_route(uint32_t timer){
    feux_route.timer = timer;
}