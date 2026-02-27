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
tBlinkersTransition transBlinkers[] = {
    /* These are examples */
    { ST_BLINKERS_INIT, EV_BLINKERS_ANY, NULL, ST_BLINKERS_ETEINTS},
    { ST_BLINKERS_ETEINTS, EV_BLINKERS_CMD0, NULL, ST_BLINKERS_ETEINTS},
    { ST_BLINKERS_ETEINTS, EV_BLINKERS_CMD1, NULL, ST_BLINKERS_ACTIVES_ALLUMES},
    { ST_BLINKERS_ACTIVES_ALLUMES, EV_BLINKERS_CMD1, NULL, ST_BLINKERS_ACTIVES_ALLUMES},
    { ST_BLINKERS_ACTIVES_ALLUMES, EV_BLINKERS_ACQ_RECU, NULL, ST_BLINKERS_ACQUITTES_VOYANT_ALLUME},
    { ST_BLINKERS_ACTIVES_ALLUMES, EV_BLINKERS_ACQ_NON_RECU, &FsmError, ST_BLINKERS_ERREUR},
    { ST_BLINKERS_ACQUITTES_VOYANT_ALLUME, EV_BLINKERS_CMD1, NULL, ST_BLINKERS_ACQUITTES_VOYANT_ALLUME},
    { ST_BLINKERS_ACQUITTES_VOYANT_ALLUME, EV_BLINKERS_CMD0, NULL, ST_BLINKERS_ETEINTS},
    { ST_BLINKERS_ACQUITTES_VOYANT_ALLUME, EV_BLINKERS_1SEC, NULL, ST_BLINKERS_ACTIVES_ETEINTS},
    { ST_BLINKERS_ACTIVES_ETEINTS, EV_BLINKERS_CMD1, NULL, ST_BLINKERS_ACTIVES_ETEINTS},
    { ST_BLINKERS_ACTIVES_ETEINTS, EV_BLINKERS_CMD0, NULL, ST_BLINKERS_ETEINTS},
    { ST_BLINKERS_ACTIVES_ETEINTS, EV_BLINKERS_ACQ_RECU, NULL, ST_BLINKERS_ACQUITTES_VOYANT_ETEINT},
    { ST_BLINKERS_ACTIVES_ETEINTS, EV_BLINKERS_ACQ_NON_RECU, &FsmError, ST_BLINKERS_ERREUR},
    { ST_BLINKERS_ACQUITTES_VOYANT_ETEINT, EV_BLINKERS_CMD1, NULL, ST_BLINKERS_ACQUITTES_VOYANT_ETEINT},
    { ST_BLINKERS_ACQUITTES_VOYANT_ETEINT, EV_BLINKERS_CMD0, NULL, ST_BLINKERS_ETEINTS},
    { ST_BLINKERS_ACQUITTES_VOYANT_ETEINT, EV_BLINKERS_1SEC, NULL, ST_BLINKERS_ACTIVES_ALLUMES},
    { ST_BLINKERS_ANY, EV_BLINKERS_ERR, &FsmError, ST_BLINKERS_TERM}
};

#define TRANS_BLINKERS_COUNT (sizeof(transBlinkers)/sizeof(*transBlinkers))

fsm_blinkers_event_t get_blinkers_next_event(fsm_blinkers_state_t current_state, blinkers_type_t which_blinker) {
    int event = EV_BLINKERS_NONE;
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
    case ST_BLINKERS_ETEINTS:
        if (cmd == 0) { return EV_BLINKERS_CMD0; }
        if (cmd == 1) { return EV_BLINKERS_CMD1; }
        break;
    case ST_BLINKERS_ACTIVES_ALLUMES:
        if (acq == 1) {
            if (which_blinker == HAZARD_LIGHTS){
                set_timer_hazard_lights(0); // Reset the timer because recieved
            }else if (which_blinker == LEFT_BLINKERS){
                set_timer_left_blinkers(0);
            }else if (which_blinker == RIGHT_BLINKERS){
                set_timer_right_blinkers(0);
            }
            return EV_BLINKERS_ACQ_RECU;
        }
        if (timer >= 10) {
            if (which_blinker == HAZARD_LIGHTS){
                set_timer_hazard_lights(0); // Reset the timer because recieved
            }else if (which_blinker == LEFT_BLINKERS){
                set_timer_left_blinkers(0);
            }else if (which_blinker == RIGHT_BLINKERS){
                set_timer_right_blinkers(0);
            }
            return EV_BLINKERS_ACQ_NON_RECU;
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
    case ST_BLINKERS_ACQUITTES_VOYANT_ALLUME:
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
    case ST_BLINKERS_ACTIVES_ETEINTS:
        if (acq == 1) {
            if (which_blinker == HAZARD_LIGHTS){
                set_timer_hazard_lights(0);
            }else if (which_blinker == LEFT_BLINKERS){
                set_timer_left_blinkers(0);
            }else if (which_blinker == RIGHT_BLINKERS){
                set_timer_right_blinkers(0);
            }
            return EV_BLINKERS_ACQ_RECU;
        }
        if (timer >= 10) {
            if (which_blinker == HAZARD_LIGHTS){
                set_timer_hazard_lights(0);
            }else if (which_blinker == LEFT_BLINKERS){
                set_timer_left_blinkers(0);
            }else if (which_blinker == RIGHT_BLINKERS){
                set_timer_right_blinkers(0);
            }
            return EV_BLINKERS_ACQ_NON_RECU;
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
    case ST_BLINKERS_ACQUITTES_VOYANT_ETEINT:
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
    case ST_BLINKERS_ERREUR:
        break;
    default:
        break;
    }
}

// This function makes ONE machine state move forward by one step
void fsm_blinkers_update(fsm_blinkers_state_t *current_state, fsm_blinkers_event_t event) {
    for (int i = 0; i < TRANS_BLINKERS_COUNT; i++) {
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
