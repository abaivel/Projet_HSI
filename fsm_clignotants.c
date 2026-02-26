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
#include "fsm_clignotants.h"


/* Callback functions called on transitions */

static int FsmError(void) { };

/* Transition table */
tTransition trans[] = {
    /* These are examples */
    { ST_INIT, EV_ANY, NULL, ST_ETEINTS},
    { ST_ETEINTS, EV_CMD0, NULL, ST_ETEINTS},
    { ST_ETEINTS, EV_CMD1, NULL, ST_ACTIVES_ALLUMES},
    { ST_ACTIVES_ALLUMES, EV_CMD1, NULL, ST_ACTIVES_ALLUMES},
    { ST_ACTIVES_ALLUMES, EV_ACQ_RECU, NULL, ST_ACQUITTES_VOYANT_ALLUME},
    { ST_ACTIVES_ALLUMES, EV_ACQ_NON_RECU, &FsmError, ST_ERREUR},
    { ST_ACQUITTES_VOYANT_ALLUME, EV_CMD1, NULL, ST_ACQUITTES_VOYANT_ALLUME},
    { ST_ACQUITTES_VOYANT_ALLUME, EV_CMD0, NULL, ST_ETEINTS},
    { ST_ACQUITTES_VOYANT_ALLUME, EV_1SEC, NULL, ST_ACTIVES_ETEINTS},
    { ST_ACTIVES_ETEINTS, EV_CMD1, NULL, ST_ACTIVES_ETEINTS},
    { ST_ACTIVES_ETEINTS, EV_CMD0, NULL, ST_ETEINTS},
    { ST_ACTIVES_ETEINTS, EV_ACQ_RECU, NULL, ST_ACQUITTES_VOYANT_ETEINT},
    { ST_ACTIVES_ETEINTS, EV_ACQ_NON_RECU, &FsmError, ST_ERREUR},
    { ST_ACQUITTES_VOYANT_ETEINT, EV_CMD1, NULL, ST_ACQUITTES_VOYANT_ETEINT},
    { ST_ACQUITTES_VOYANT_ETEINT, EV_CMD0, NULL, ST_ETEINTS},
    { ST_ACQUITTES_VOYANT_ETEINT, EV_1SEC, NULL, ST_ACTIVES_ALLUMES},
    { ST_ANY, EV_ERR, &FsmError, ST_TERM}
};

#define TRANS_COUNT (sizeof(trans)/sizeof(*trans))

fsm_blinkers_state_t get_next_event(fsm_blinkers_state_t current_state, blinkers_type_t which_blinker) {
    int event = EV_NONE;
    cmd_t cmd;
    acq_t acq;
    timer_bcgv_t timer;
    if (which_blinker == HAZARD_LIGHTS) {
        cmd = get_cmd_hazard_lights();
        acq = get_acq_hazard_lights();
        timer = get_timer_hazard_lights();
    }else if (which_blinker == LEFT_BLINKERS) {
        cmd = get_cmd_left_blinkers();
        acq = get_acq_left_blinkers();
        timer = get_timer_left_blinkers();
    }else if (which_blinker == RIGHT_BLINKERS) {
        cmd = get_cmd_right_blinkers();
        acq = get_acq_right_blinkers();
        timer = get_timer_right_blinkers();
    }

    switch (current_state)
    {
    case ST_ETEINTS:
        if (cmd == 0) { return EV_CMD0; }
        if (cmd == 1) { return EV_CMD1; }
        break;
    case ST_ACTIVES_ALLUMES:
        if (cmd == 0) { return EV_CMD0; }
        if (cmd == 1) { return EV_CMD1; }
        if (acq == 1) {
            if (which_blinker == HAZARD_LIGHTS){
                set_timer_hazard_lights(0); // Reset the timer because recieved
            }else if (which_blinker == LEFT_BLINKERS){
                set_timer_left_blinkers(0);
            }else if (which_blinker == RIGHT_BLINKERS){
                set_timer_right_blinkers(0);
            }
            return EV_ACQ_RECU;
        }
        // Timeout managment (1s = 10 * 100ms)
        if (timer >= 10) {
            if (which_blinker == HAZARD_LIGHTS){
                set_timer_hazard_lights(0); // Reset the timer because recieved
            }else if (which_blinker == LEFT_BLINKERS){
                set_timer_left_blinkers(0);
            }else if (which_blinker == RIGHT_BLINKERS){
                set_timer_right_blinkers(0);
            }
            return EV_ACQ_NON_RECU;
        } else {
            if (which_blinker == HAZARD_LIGHTS){
                set_timer_hazard_lights(timer + 1); // Reset the timer because recieved
            }else if (which_blinker == LEFT_BLINKERS){
                set_timer_left_blinkers(timer + 1);
            }else if (which_blinker == RIGHT_BLINKERS){
                set_timer_right_blinkers(timer + 1);
            }
        }
        break;
    case ST_ACQUITTES_VOYANT_ALLUME:
        if (cmd == 0) { return EV_CMD0; }
        if (cmd == 1) { return EV_CMD1; }
        if (timer >= 10) {
            if (which_blinker == HAZARD_LIGHTS){
                set_timer_hazard_lights(0); // Reset the timer because recieved
            }else if (which_blinker == LEFT_BLINKERS){
                set_timer_left_blinkers(0);
            }else if (which_blinker == RIGHT_BLINKERS){
                set_timer_right_blinkers(0);
            }
            return EV_1SEC;
        } else {
            if (which_blinker == HAZARD_LIGHTS){
                set_timer_hazard_lights(timer + 1); // Reset the timer because recieved
            }else if (which_blinker == LEFT_BLINKERS){
                set_timer_left_blinkers(timer + 1);
            }else if (which_blinker == RIGHT_BLINKERS){
                set_timer_right_blinkers(timer + 1);
            }
        }
        break;
    case ST_ACTIVES_ETEINTS:
        if (cmd == 0) { return EV_CMD0; }
        if (cmd == 1) { return EV_CMD1; }
        if (acq == 1) {
            if (which_blinker == HAZARD_LIGHTS){
                set_timer_hazard_lights(0);
            }else if (which_blinker == LEFT_BLINKERS){
                set_timer_left_blinkers(0);
            }else if (which_blinker == RIGHT_BLINKERS){
                set_timer_right_blinkers(0);
            }
            return EV_ACQ_RECU;
        }
        if (timer >= 10) {
            if (which_blinker == HAZARD_LIGHTS){
                set_timer_hazard_lights(0); 
            }else if (which_blinker == LEFT_BLINKERS){
                set_timer_left_blinkers(0);
            }else if (which_blinker == RIGHT_BLINKERS){
                set_timer_right_blinkers(0);
            }
            return EV_ACQ_NON_RECU;
        } else {
            if (which_blinker == HAZARD_LIGHTS){
                set_timer_hazard_lights(timer + 1);
            }else if (which_blinker == LEFT_BLINKERS){
                set_timer_left_blinkers(timer + 1);
            }else if (which_blinker == RIGHT_BLINKERS){
                set_timer_right_blinkers(timer + 1);
            }
        }
        break;
    case ST_ACQUITTES_VOYANT_ETEINT:
        if (cmd == 0) { return EV_CMD0; }
        if (cmd == 1) { return EV_CMD1; }
        if (timer >= 10) {
            if (which_blinker == HAZARD_LIGHTS){
                set_timer_hazard_lights(0);
            }else if (which_blinker == LEFT_BLINKERS){
                set_timer_left_blinkers(0);
            }else if (which_blinker == RIGHT_BLINKERS){
                set_timer_right_blinkers(0);
            }
            return EV_1SEC;
        } else {
            if (which_blinker == HAZARD_LIGHTS){
                set_timer_hazard_lights(timer + 1);
            }else if (which_blinker == LEFT_BLINKERS){
                set_timer_left_blinkers(timer + 1);
            }else if (which_blinker == RIGHT_BLINKERS){
                set_timer_right_blinkers(timer + 1);
            }
        }
        break;
    case ST_ERREUR:
        break;
    default:
        break;
    }
}

// This function makes ONE machine state move forward by one step
void fsm__blinker_update(fsm_blinkers_event_t *current_state, fsm_blinkers_state_t event) {
    for (int i = 0; i < TRANS_COUNT; i++) {
        // If state is matches AND the event matches
        if ((*current_state == trans[i].state || trans[i].state == ST_ANY) && 
            (event == trans[i].event)) {
            
            *current_state = trans[i].next_state;
            
            // Execute the associated action (callback) if it exist
            if (trans[i].callback != NULL) {
                trans[i].callback();
            }
            break; // Transition found, we quit the for loop
        }
    }
}
