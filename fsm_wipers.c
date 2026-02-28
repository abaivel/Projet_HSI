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
#include "fsm_wipers.h"

/* Callback functions called on transitions */
static int FsmError(void) {};

/* Transition table */
tWipersTransition transWipers[] = {
    /* These are examples */
    { ST_WIPERS_WASHERS_INIT, EV_WIPERS_WASHERS_ANY, NULL, ST_WIPERS_WASHERS_ALL_OFF},
    { ST_WIPERS_WASHERS_ALL_OFF, EV_WIPERS_WASHERS_CMD_EG1, NULL, ST_WIPERS_ACTIVE},
    { ST_WIPERS_WASHERS_ALL_OFF, EV_WIPERS_WASHERS_CMD_LG1, NULL, ST_WIPERS_WASHERS_ACTIVE},
    { ST_WIPERS_WASHERS_ALL_OFF, EV_WIPERS_WASHERS_CMD_EG0_CMD_LG0, NULL, ST_WIPERS_WASHERS_ALL_OFF},
    { ST_WIPERS_ACTIVE, EV_WIPERS_WASHERS_CMD_EG1, NULL, ST_WIPERS_ACTIVE},
    { ST_WIPERS_ACTIVE, EV_WIPERS_WASHERS_CMD_EG0, NULL, ST_WIPERS_WASHERS_ALL_OFF},
    { ST_WIPERS_ACTIVE, EV_WIPERS_WASHERS_CMD_LG1, NULL, ST_WIPERS_WASHERS_ACTIVE},
    { ST_WIPERS_WASHERS_ACTIVE, EV_WIPERS_WASHERS_CMD_LG1, NULL, ST_WIPERS_WASHERS_ACTIVE},
    { ST_WIPERS_WASHERS_ACTIVE, EV_WIPERS_WASHERS_CMD_LG0, NULL, ST_WIPERS_WASHERS_TIMER_OFF},
    { ST_WIPERS_WASHERS_TIMER_OFF, EV_WIPERS_WASHERS_CMD_LG1, NULL, ST_WIPERS_WASHERS_ACTIVE},
    { ST_WIPERS_WASHERS_TIMER_OFF, EV_WIPERS_WASHERS_LESS_2SEC, NULL, ST_WIPERS_WASHERS_TIMER_OFF},
    { ST_WIPERS_WASHERS_TIMER_OFF, EV_WIPERS_WASHERS_MORE_2SEC, NULL, ST_WIPERS_WASHERS_ALL_OFF},
};

#define TRANS_WIPERS_COUNT (sizeof(transWipers)/sizeof(*transWipers))

fsm_wipers_event_t get_wipers_next_event(fsm_wipers_state_t current_state)
{
    cmd_t cmd_wp, cmd_ww;
    timer_bcgv_t timer;
    
    cmd_wp = get_cmd_wipers(); //Wipers - wp
    cmd_ww = get_cmd_windshield_washer(); //Window washers - ww
    timer = get_timer_wipers();
    
    switch (current_state) {
        case ST_WIPERS_WASHERS_ALL_OFF:
            if (cmd_wp == 0 && cmd_ww == 0){
                return EV_WIPERS_WASHERS_CMD_EG0_CMD_LG0;
            }else if (cmd_ww == 1) {
                return EV_WIPERS_WASHERS_CMD_LG1;
            }else if (cmd_wp == 1) {
                return EV_WIPERS_WASHERS_CMD_EG1;
            }
            break;   
        case ST_WIPERS_ACTIVE:
            if (cmd_ww == 1) {
                return EV_WIPERS_WASHERS_CMD_LG1;
            }else if (cmd_wp == 1) {
                return EV_WIPERS_WASHERS_CMD_EG1;
            }else if (cmd_wp == 0) {
                return EV_WIPERS_WASHERS_CMD_EG0;
            }
            break;   
        case ST_WIPERS_WASHERS_ACTIVE:
            if (cmd_ww == 1){
                return EV_WIPERS_WASHERS_CMD_LG1;
            }else if (cmd_ww == 0) {
                set_timer_wipers(0);
                return EV_WIPERS_WASHERS_CMD_LG0;
            }
            break; 
        case ST_WIPERS_WASHERS_TIMER_OFF:
            if (timer >= 20) { // Timeout managment (2s = 20 * 100ms)
                set_timer_wipers(0);
                return EV_WIPERS_WASHERS_MORE_2SEC;
            }else {
                set_timer_wipers(timer + 1);
                //printf("\ntimer: %d\n",timer);
                return EV_WIPERS_WASHERS_LESS_2SEC;
            }
            if (cmd_ww == 1){ return EV_WIPERS_WASHERS_CMD_LG1; }
            break;            
        default:
            break;
    }
}

// This function makes ONE machine state move forward by one step
void fsm_wipers_update(fsm_wipers_state_t *current_state, fsm_wipers_event_t event) {
    for (int i = 0; i < TRANS_WIPERS_COUNT; i++) {
        // If state is matches AND the event matches
        if ((*current_state == transWipers[i].state || transWipers[i].state == ST_WIPERS_WASHERS_ANY) && 
            (event == transWipers[i].event)) {
            
            *current_state = transWipers[i].next_state;
            
            // Execute the associated action (callback) if it exist
            if (transWipers[i].callback != NULL) {
                transWipers[i].callback();
            }
            break; // Transition found, we quit the for loop
        }
    }
}
