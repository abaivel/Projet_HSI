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
#include "fsm_essuie_glaces.h"

/* Callback functions called on transitions */
static int FsmError(void) {};

/* Transition table */
tTransition trans[] = {
    /* These are examples */
    { ST_INIT, EV_ANY, NULL, ST_TOUT_ETEINTS},
    { ST_TOUT_ETEINTS, EV_CMD_EG1, NULL, ST_ESSUIE_GLACES_ACTIVES},
    { ST_TOUT_ETEINTS, EV_CMD_LG1, NULL, ST_TOUT_ACTIVES},
    { ST_TOUT_ETEINTS, EV_CMD_EG0_CMD_LG0, NULL, ST_TOUT_ETEINTS},
    { ST_ESSUIE_GLACES_ACTIVES, EV_CMD_EG1, NULL, ST_ESSUIE_GLACES_ACTIVES},
    { ST_ESSUIE_GLACES_ACTIVES, EV_CMD_EG0, NULL, ST_TOUT_ETEINTS},
    { ST_ESSUIE_GLACES_ACTIVES, EV_CMD_LG1, NULL, ST_TOUT_ACTIVES},
    { ST_TOUT_ACTIVES, EV_CMD_LG1, NULL, ST_TOUT_ACTIVES},
    { ST_TOUT_ACTIVES, EV_CMD_LG0, NULL, ST_TIMER_ETEINTS},
    { ST_TIMER_ETEINTS, EV_CMD_LG1, NULL, ST_TOUT_ACTIVES},
    { ST_TIMER_ETEINTS, EV_LESS_2SEC, NULL, ST_TIMER_ETEINTS},
    { ST_TIMER_ETEINTS, EV_MORE_2SEC, NULL, ST_TOUT_ETEINTS},
};

#define TRANS_COUNT (sizeof(trans)/sizeof(*trans))

fsm_wipers_event_t get_next_event(fsm_wipers_event_t current_state)
{
    cmd_t cmd_wp, cmd_ww;
    timer_bcgv_t timer;
    
    cmd_wp = get_cmd_wipers();
    cmd_ww = get_cmd_windshield_washer();
    
    switch (current_state) {
        case ST_TOUT_ETEINTS:
            if (cmd_wp == 0 && cmd_ww == 0){
                return EV_CMD_EG0_CMD_LG0;
            }else if (cmd_ww == 1) {
                return EV_CMD_LG1;
            }else if (cmd_wp == 1) {
                return EV_CMD_EG1;
            }
            break;   
        case ST_ESSUIE_GLACES_ACTIVES:
            if (cmd_ww == 1) {
                return EV_CMD_LG1;
            }else if (cmd_wp == 1) {
                return EV_CMD_EG1;
            }else if (cmd_wp == 0) {
                return EV_CMD_EG0;
            }
            break;   
        case ST_TOUT_ACTIVES:
            if (cmd_ww == 1){
                return EV_CMD_LG1;
            }else if (cmd_ww == 0) {
                set_timer_wipers(0);
                return EV_CMD_LG0;
            }
            break; 
        case ST_TIMER_ETEINTS:
            if (cmd_ww == 1){
                return EV_CMD_LG1;
            }
            if (timer >= 20) { // Timeout managment (2s = 20 * 100ms)
                set_timer_wipers(0);
                return EV_MORE_2SEC;
            }else {
                set_timer_wipers(timer + 1);
                return EV_LESS_2SEC;
            }
            break;            
        default:
            break;
    }
}

int main(void)
{
    int i = 0;
    int ret = 0; 
    fsm_wipers_event_t event = EV_NONE;
    fsm_wipers_state_t state = ST_INIT;
    
    /* While FSM hasn't reach end state */
    while (state != ST_TERM) {
        
        /* Get event */
        event = get_wipers_next_event(state);
        
        /* For each transitions */
        for (i = 0; i < TRANS_COUNT; i++) {
            /* If State is current state OR The transition applies to all states ...*/
            if ((state == trans[i].state) || (ST_ANY == trans[i].state)) {
                /* If event is the transition event OR the event applies to all */
                if ((event == trans[i].event) || (EV_ANY == trans[i].event)) {
                    /* Apply the new state */
                    state = trans[i].next_state;
                    if (trans[i].callback != NULL) {
                        /* Call the state function */
                        ret = (trans[i].callback)();
                    }
                    break;
                }
            }
        }
    }

    return ret;
}
