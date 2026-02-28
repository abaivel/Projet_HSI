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
#include "fsm_lights.h"

/* Callback functions called on transitions */

static int FsmError(void) { };

/* Transition table */
tTransition transLights[] = {
    /* These are examples */
    { ST_LIGHTS_INIT, EV_LIGHTS_ANY, NULL, ST_LIGHTS_OFF},
    { ST_LIGHTS_OFF, EV_LIGHTS_CMD1, NULL, ST_LIGHTS_ON},
    { ST_LIGHTS_OFF, EV_LIGHTS_CMD0, NULL, ST_LIGHTS_OFF},
    { ST_LIGHTS_ON, EV_LIGHTS_CMD0, NULL, ST_LIGHTS_OFF},
    { ST_LIGHTS_ON, EV_LIGHTS_CMD1, NULL, ST_LIGHTS_ON},
    { ST_LIGHTS_ON, EV_LIGHTS_ACK_REC, NULL, ST_LIGHTS_ACK},
    { ST_LIGHTS_ON, EV_LIGHTS_ACK_NOT_REC, &FsmError, ST_LIGHTS_ERROR},
    { ST_LIGHTS_ACK, EV_LIGHTS_CMD0, NULL, ST_LIGHTS_OFF}
};

#define TRANS_LIGHTS_COUNT (sizeof(transLights)/sizeof(*transLights))

fsm_lights_event_t get_lights_next_event(fsm_lights_state_t current_state, light_type_t which_light) {
    cmd_t cmd;
    ack_t ACK;
    timer_bcgv_t timer;
    
    // We search for the right data depending on the headlight asked
    if (which_light == POSITION_LIGHTS) {
        cmd = get_cmd_position_lights(); 
        ACK = get_ack_position_lights();
        timer = get_timer_position_lights();
    } else if (which_light == LOW_BEAMS_HEADLIGHTS) {
        cmd = get_cmd_low_beams_headlights();
        ACK = get_ack_low_beams_headlights();
        timer = get_timer_low_beams_headlights();
    } else {
        cmd = get_cmd_high_beams_headlights();
        ACK = get_ack_high_beams_headlights();
        timer = get_timer_high_beams_headlights();
    }

    // 2. Desision logic based on state
    switch (current_state) {
        case ST_LIGHTS_OFF:
            if (which_light == POSITION_LIGHTS){
                set_ack_position_lights(0);
            }else if (which_light == LOW_BEAMS_HEADLIGHTS){
                set_ack_low_beams_headlights(0);
            }else{
                set_ack_high_beams_headlights(0);
            }
            if (cmd == 1) return EV_LIGHTS_CMD1; 
            if (cmd == 0) return EV_LIGHTS_CMD0;
            break;

        case ST_LIGHTS_ON:
        if (ACK == 1) {
                if (which_light == POSITION_LIGHTS){
                    set_timer_position_lights(0); // Reset the timer because recieved
                }else if (which_light == LOW_BEAMS_HEADLIGHTS){
                    set_timer_low_beams_headlights(0);
                }else{
                    set_timer_high_beams_headlights(0);
                }
                return EV_LIGHTS_ACK_REC;
            }
            
            // Timeout managment (1s = 10 * 100ms)
            if (timer >= 10) {
                if (which_light == POSITION_LIGHTS){
                    set_timer_position_lights(0); // Reset the timer because recieved
                }else if (which_light == LOW_BEAMS_HEADLIGHTS){
                    set_timer_low_beams_headlights(0);
                }else{
                    set_timer_high_beams_headlights(0);
                }
                return EV_LIGHTS_ACK_NOT_REC;
            } else {
                if (which_light == POSITION_LIGHTS){
                    set_timer_position_lights(timer + 1); // Reset the timer because recieved
                }else if (which_light == LOW_BEAMS_HEADLIGHTS){
                    set_timer_low_beams_headlights(timer + 1);
                }else{
                    set_timer_high_beams_headlights(timer + 1);
                }
            }
            if (cmd == 1) return EV_LIGHTS_CMD1;
            if (cmd == 0) return EV_LIGHTS_CMD0;
            break;

        case ST_LIGHTS_ACK:
            if (cmd == 0) return EV_LIGHTS_CMD0;
            if (cmd == 1) return EV_LIGHTS_CMD1;
            break;

        case ST_LIGHTS_ERROR:
            
            break;

        default:
            break;
    }

    return EV_LIGHTS_NONE;
}

// This function makes ONE machine state move forward by one step
void fsm_lights_update(fsm_lights_state_t *current_state, fsm_lights_event_t event) {
    for (int i = 0; i < TRANS_LIGHTS_COUNT; i++) {
        // If state is matches AND the event matches
        if ((*current_state == transLights[i].state || transLights[i].state == ST_LIGHTS_ANY) && 
            (event == transLights[i].event)) {
            
            *current_state = transLights[i].next_state;
            
            // Execute the associated action (callback) if it exist
            if (transLights[i].callback != NULL) {
                transLights[i].callback();
            }
            break; // Transition found, we quit the for loop
        }
    }
}