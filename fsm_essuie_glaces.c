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
#include "bcgv_lib.h"

/* States */
typedef enum {
    ST_ANY = -1,                            /* Any state */
    ST_INIT = 0,                            /* Init state */
    ST_TOUT_ETEINTS = 1,
    ST_ESSUIE_GLACES_ACTIVES = 2,
    ST_TOUT_ACTIVES = 3,
    ST_TIMER_ETEINTS = 4,
    ST_TERM = 255                           /* Final state */
} fsm_state_t;

/* Events */
typedef enum {
    EV_ANY = -1,                            /* Any event */
    EV_NONE = 0,                            /* No event */
    EV_CMD_EG0 = 1,
    EV_CMD_EG1 = 2,
    EV_CMD_LG0 = 3,
    EV_CMD_LG1 = 4,
    EV_CMD_EG0_CMD_LG0 = 5,
    EV_LESS_2SEC = 6,
    EV_MORE_2SEC = 7,
    EV_ERR = 255                            /* Error event */
} fsm_event_t;

/* Callback functions called on transitions */
static int FsmError(void) {};

/* Transition structure */
typedef struct {
    fsm_state_t state;
    fsm_event_t event;
    int (*callback)(void);
    int next_state;
} tTransition;

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

int get_next_event(int current_state)
{
    int event = EV_NONE;
    cmd_t cmd_wp, cmd_ww;
    timer_bcgv_t timer;
    
    cmd_wp = get_cmd_wipers();
    cmd_ww = get_cmd_windshield_washer();

    if (current_state == ST_INIT) {
        current_state = ST_TOUT_ETEINTS;
    }

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
    return event;
}

int main(void)
{
    int i = 0;
    int ret = 0; 
    int event = EV_NONE;
    int state = ST_INIT;
    
    /* While FSM hasn't reach end state */
    while (state != ST_TERM) {
        
        /* Get event */
        event = get_next_event(state);
        
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
