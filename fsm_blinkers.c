/**
 * \file        fsm.c
 * \author      Alexis Daley
 * \version     0.4
 * \date        08 otober 2023
 * \brief       This is a template file to create a Finite State Machine.
 * \details
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "fsm_blinkers.h"

/* Transition table */
tBlinkersTransition transBlinkers[] = {
    /* These are examples */
    { ST_BLINKERS_INIT, EV_BLINKERS_ANY, NULL, ST_BLINKERS_OFF},
    { ST_BLINKERS_OFF, EV_BLINKERS_CMD0, NULL, ST_BLINKERS_OFF},
    { ST_BLINKERS_OFF, EV_BLINKERS_CMD1, NULL, ST_BLINKERS_ACTIVATED_ON},
    { ST_BLINKERS_ACTIVATED_ON, EV_BLINKERS_CMD1, NULL, ST_BLINKERS_ACTIVATED_ON},
    { ST_BLINKERS_ACTIVATED_ON, EV_BLINKERS_ACK_REC, NULL, ST_BLINKERS_ACK_ON},
    { ST_BLINKERS_ACTIVATED_ON, EV_BLINKERS_ACK_NOT_REC, NULL, ST_BLINKERS_ERROR},
    { ST_BLINKERS_ACK_ON, EV_BLINKERS_CMD1, NULL, ST_BLINKERS_ACK_ON},
    { ST_BLINKERS_ACK_ON, EV_BLINKERS_CMD0, NULL, ST_BLINKERS_OFF},
    { ST_BLINKERS_ACK_ON, EV_BLINKERS_1SEC, NULL, ST_BLINKERS_ACTIVATED_OFF},
    { ST_BLINKERS_ACTIVATED_OFF, EV_BLINKERS_CMD1, NULL, ST_BLINKERS_ACTIVATED_OFF},
    { ST_BLINKERS_ACTIVATED_OFF, EV_BLINKERS_CMD0, NULL, ST_BLINKERS_OFF},
    { ST_BLINKERS_ACTIVATED_OFF, EV_BLINKERS_ACK_REC, NULL, ST_BLINKERS_ACK_OFF},
    { ST_BLINKERS_ACTIVATED_OFF, EV_BLINKERS_ACK_NOT_REC, NULL, ST_BLINKERS_ERROR},
    { ST_BLINKERS_ACK_OFF, EV_BLINKERS_CMD1, NULL, ST_BLINKERS_ACK_OFF},
    { ST_BLINKERS_ACK_OFF, EV_BLINKERS_CMD0, NULL, ST_BLINKERS_OFF},
    { ST_BLINKERS_ACK_OFF, EV_BLINKERS_1SEC, NULL, ST_BLINKERS_ACTIVATED_ON},
    { ST_BLINKERS_ANY, EV_BLINKERS_ERR, NULL, ST_BLINKERS_ENDED}
};

#define TRANS_BLINKERS_COUNT (sizeof(transBlinkers)/sizeof(*transBlinkers))

fsm_blinkers_event_t get_blinkers_next_event(fsm_blinkers_state_t current_state, blinkers_type_t which_blinker) {
    cmd_t cmd;
    ack_t ack;
    timer_bcgv_t timer;
    if (which_blinker == HAZARD_LIGHTS) {
        cmd = get_cmd_hazard_lights();
        ack = get_ack_hazard_lights();
        timer = get_timer_hazard_lights();
    }else if (which_blinker == LEFT_BLINKERS) {
        cmd = get_cmd_left_blinkers();
        ack = get_ack_left_blinkers();
        timer = get_timer_left_blinkers();
    }else if (which_blinker == RIGHT_BLINKERS) {
        cmd = get_cmd_right_blinkers();
        ack = get_ack_right_blinkers();
        timer = get_timer_right_blinkers();
    }
    switch (current_state)
    {
    case ST_BLINKERS_OFF:
        if (cmd == 0) { return EV_BLINKERS_CMD0; }
        if (cmd == 1) { return EV_BLINKERS_CMD1; }
        break;
    case ST_BLINKERS_ACTIVATED_ON:
        if (ack == 1) {
            if (which_blinker == HAZARD_LIGHTS){
                set_timer_hazard_lights(0); // Reset the timer because recieved
            }else if (which_blinker == LEFT_BLINKERS){
                set_timer_left_blinkers(0);
            }else if (which_blinker == RIGHT_BLINKERS){
                set_timer_right_blinkers(0);
            }
            return EV_BLINKERS_ACK_REC;
        }
        if (timer >= 10) {
            if (which_blinker == HAZARD_LIGHTS){
                set_timer_hazard_lights(0); // Reset the timer because recieved
            }else if (which_blinker == LEFT_BLINKERS){
                set_timer_left_blinkers(0);
            }else if (which_blinker == RIGHT_BLINKERS){
                set_timer_right_blinkers(0);
            }
            return EV_BLINKERS_ACK_NOT_REC;
        } else {
            if (which_blinker == HAZARD_LIGHTS){
                set_timer_hazard_lights(timer + 1); // Reset the timer because recieved
            }else if (which_blinker == LEFT_BLINKERS){
                set_timer_left_blinkers(timer + 1);
            }else if (which_blinker == RIGHT_BLINKERS){
                set_timer_right_blinkers(timer + 1);
            }
        }
        if (cmd == 0) { return EV_BLINKERS_CMD0; }
        if (cmd == 1) { return EV_BLINKERS_CMD1; }
        // Timeout managment (1s = 10 * 100ms)
        break;
    case ST_BLINKERS_ACK_ON:
        if (timer >= 10) {
            if (which_blinker == HAZARD_LIGHTS){
                set_timer_hazard_lights(0); // Reset the timer because recieved
            }else if (which_blinker == LEFT_BLINKERS){
                set_timer_left_blinkers(0);
            }else if (which_blinker == RIGHT_BLINKERS){
                set_timer_right_blinkers(0);
            }
            return EV_BLINKERS_1SEC;
        } else {
            if (which_blinker == HAZARD_LIGHTS){
                set_timer_hazard_lights(timer + 1); // Reset the timer because recieved
            }else if (which_blinker == LEFT_BLINKERS){
                set_timer_left_blinkers(timer + 1);
            }else if (which_blinker == RIGHT_BLINKERS){
                set_timer_right_blinkers(timer + 1);
            }
        }
        if (cmd == 0) { return EV_BLINKERS_CMD0; }
        if (cmd == 1) { return EV_BLINKERS_CMD1; }
        break;
    case ST_BLINKERS_ACTIVATED_OFF:
        if (ack == 1) {
            if (which_blinker == HAZARD_LIGHTS){
                set_timer_hazard_lights(0);
            }else if (which_blinker == LEFT_BLINKERS){
                set_timer_left_blinkers(0);
            }else if (which_blinker == RIGHT_BLINKERS){
                set_timer_right_blinkers(0);
            }
            return EV_BLINKERS_ACK_REC;
        }
        if (timer >= 10) {
            if (which_blinker == HAZARD_LIGHTS){
                set_timer_hazard_lights(0);
            }else if (which_blinker == LEFT_BLINKERS){
                set_timer_left_blinkers(0);
            }else if (which_blinker == RIGHT_BLINKERS){
                set_timer_right_blinkers(0);
            }
            return EV_BLINKERS_ACK_NOT_REC;
        } else {
            if (which_blinker == HAZARD_LIGHTS){
                set_timer_hazard_lights(timer + 1);
            }else if (which_blinker == LEFT_BLINKERS){
                set_timer_left_blinkers(timer + 1);
            }else if (which_blinker == RIGHT_BLINKERS){
                set_timer_right_blinkers(timer + 1);
            }
        }
        if (cmd == 0) { return EV_BLINKERS_CMD0; }
        if (cmd == 1) { return EV_BLINKERS_CMD1; }
        break;
    case ST_BLINKERS_ACK_OFF:
        if (timer >= 10) {
            if (which_blinker == HAZARD_LIGHTS){
                set_timer_hazard_lights(0);
            }else if (which_blinker == LEFT_BLINKERS){
                set_timer_left_blinkers(0);
            }else if (which_blinker == RIGHT_BLINKERS){
                set_timer_right_blinkers(0);
            }
            return EV_BLINKERS_1SEC;
        } else {
            if (which_blinker == HAZARD_LIGHTS){
                set_timer_hazard_lights(timer + 1);
            }else if (which_blinker == LEFT_BLINKERS){
                set_timer_left_blinkers(timer + 1);
            }else if (which_blinker == RIGHT_BLINKERS){
                set_timer_right_blinkers(timer + 1);
            }
        }
        if (cmd == 0) { return EV_BLINKERS_CMD0; }
        if (cmd == 1) { return EV_BLINKERS_CMD1; }
        break;
    case ST_BLINKERS_ERROR:
        break;
    default:
        break;
    }
    return EV_BLINKERS_NONE;
}

// This function makes ONE machine state move forward by one step
void fsm_blinkers_update(fsm_blinkers_state_t *current_state, fsm_blinkers_event_t event) {
    for (int i = 0; i < (int)TRANS_BLINKERS_COUNT; i++) {
        // If state is matches AND the event matches
        if ((*current_state == transBlinkers[i].state || transBlinkers[i].state == ST_BLINKERS_ANY) && 
            (event == transBlinkers[i].event)) {
            
            *current_state = transBlinkers[i].next_state;
            
            // Execute the associated action (callback) if it exist
            if (transBlinkers[i].callback != NULL) {
                transBlinkers[i].callback();
            }
            break; // Transition found, we quit the for loop
        }
    }
}
