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

/* States */
typedef enum {
    ST_ANY = -1,                            /* Any state */
    ST_INIT = 0,                            /* Init state */
    ST_ETEINTS = 1,
    ST_ALLUMES = 2,
    ST_ACQUITTES = 3,
    ST_ERREUR = 4,
    ST_TERM = 255                           /* Final state */
} fsm_state_t;

/* Events */
typedef enum {
    EV_ANY = -1,                            /* Any event */
    EV_NONE = 0,                            /* No event */
    EV_CMD0 = 1,
    EV_CMD1 = 2,
    EV_ACQ_RECU = 3,
    EV_ACQ_NON_RECU = 4,
    EV_ERR = 255                            /* Error event */
} fsm_event_t;

/* Callback functions called on transitions */

static int FsmError(void) { };

/* Transition structure */
typedef struct {
    fsm_state_t state;
    fsm_event_t event;
    int (*callback)(void);
    fsm_state_t next_state;
} tTransition;

/* Transition table */
tTransition trans[] = {
    /* These are examples */
    { ST_INIT, EV_ANY, NULL, ST_ETEINTS},
    { ST_ETEINTS, EV_CMD1, NULL, ST_ALLUMES},
    { ST_ALLUMES, EV_ACQ_RECU, NULL, ST_ACQUITTES},
    { ST_ALLUMES, EV_ACQ_NON_RECU, &FsmError, ST_ERREUR},
    {ST_ACQUITTES, EV_CMD0, NULL, ST_ETEINTS}
};

#define TRANS_COUNT (sizeof(trans)/sizeof(*trans))

int get_next_event(int current_state)
{
    int event = EV_NONE;

    /* Here, you can get the parameters of your FSM */

    /* Build all the events */

    /*Example code : 
    if (PARAM1 == ...) {
        event = EV_EVENT1
    }
    else if (PARAM2 == ... && PARAM3 == ...) {
        event = EV_EVENT2
    }
    ...
    */
    uint8_t cmd_position = get_cmd_feux_position();

    if (current_state == ST_ETEINTS){

    }else if (current_state == ST_ALLUMES){

    }else if (current_state == ST_ACQUITTES){

    }else if (current_state == ST_ERREUR){

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
