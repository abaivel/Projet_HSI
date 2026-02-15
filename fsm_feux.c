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
#include "fsm_feux.h"


/* Callback functions called on transitions */

static int FsmError(void) { };

/* Transition table */
tTransition trans[] = {
    /* These are examples */
    { ST_INIT, EV_ANY, NULL, ST_ETEINTS},
    { ST_ETEINTS, EV_CMD1, NULL, ST_ALLUMES},
    { ST_ETEINTS, EV_CMD0, NULL, ST_ETEINTS},
    { ST_ALLUMES, EV_CMD0, NULL, ST_ETEINTS},
    { ST_ALLUMES, EV_CMD1, NULL, ST_ALLUMES},
    { ST_ALLUMES, EV_ACQ_RECU, NULL, ST_ACQUITTES},
    { ST_ALLUMES, EV_ACQ_NON_RECU, &FsmError, ST_ERREUR},
    {ST_ACQUITTES, EV_CMD0, NULL, ST_ETEINTS}
};

#define TRANS_COUNT (sizeof(trans)/sizeof(*trans))

fsm_feux_event_t get_next_event(fsm_feux_state_t current_state, type_feu_t quel_feu) {
    uint8_t cmd, acq;
    uint32_t timer;
    
    // On va chercher la bonne donnée selon le feu demandé
    if (quel_feu == FEU_POSITION) {
        cmd = get_cmd_feux_position(); 
        acq = get_acq_feux_position();
        timer = get_timer_feux_position();
    } else if (quel_feu == FEU_CROISEMENT) {
        cmd = get_cmd_feux_croisement();
        acq = get_acq_feux_croisement();
        timer = get_timer_feux_croisement();
    } else {
        cmd = get_cmd_feux_route();
        acq = get_acq_feux_route();
        timer = get_timer_feux_route();
    }

    // 2. Logique de décision par état
    switch (current_state) {
        case ST_ETEINTS:
            if (quel_feu == FEU_POSITION){
                    set_acq_feux_position(0);
            }else if (quel_feu == FEU_CROISEMENT){
                set_acq_feux_croisement(0);
            }else{
                set_acq_feux_route(0);
            }
            if (cmd == 1) return EV_CMD1; 
            if (cmd == 0) return EV_CMD0;
            break;

        case ST_ALLUMES:
            if (cmd == 1) return EV_CMD1;
            if (cmd == 0) return EV_CMD0;

            if (acq == 1) {
                if (quel_feu == FEU_POSITION){
                    set_timer_feux_position(0); // Reset le timer car reçu
                }else if (quel_feu == FEU_CROISEMENT){
                    set_timer_feux_croisement(0);
                }else{
                    set_timer_feux_route(0);
                }
                return EV_ACQ_RECU;
            }
            
            // Gestion du timeout (1s = 10 * 100ms)
            if (timer >= 10) {
                if (quel_feu == FEU_POSITION){
                    set_timer_feux_position(0); // Reset le timer car reçu
                }else if (quel_feu == FEU_CROISEMENT){
                    set_timer_feux_croisement(0);
                }else{
                    set_timer_feux_route(0);
                }
                return EV_ACQ_NON_RECU;
            } else {
                if (quel_feu == FEU_POSITION){
                    set_timer_feux_position(timer + 1); // Reset le timer car reçu
                }else if (quel_feu == FEU_CROISEMENT){
                    set_timer_feux_croisement(timer + 1);
                }else{
                    set_timer_feux_route(timer + 1);
                }
            }
            break;

        case ST_ACQUITTES:
            if (cmd == 0) return EV_CMD0;
            if (cmd == 1) return EV_CMD1;
            break;

        case ST_ERREUR:
            
            break;

        default:
            break;
    }

    return EV_NONE;
}

// Cette fonction fait avancer UNE machine à état d'un pas (step)
void fsm_update(fsm_feux_state_t *current_state, fsm_feux_event_t event) {
    for (int i = 0; i < TRANS_COUNT; i++) {
        // Si l'état correspond ET l'événement correspond
        if ((*current_state == trans[i].state || trans[i].state == ST_ANY) && 
            (event == trans[i].event)) {
            
            *current_state = trans[i].next_state;
            
            // Exécuter l'action associée (callback) si elle existe
            if (trans[i].callback != NULL) {
                trans[i].callback();
            }
            break; // Transition trouvée, on sort de la boucle for
        }
    }
}