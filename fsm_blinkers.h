#ifndef FSM_BLINKERS_H_
#define FSM_BLINKERS_H_

#include "bcgv_lib.h"

/* States */
typedef enum {
    ST_BLINKERS_ANY = -1,                            /* Any state */
    ST_BLINKERS_INIT = 0,                            /* Init state */
    ST_BLINKERS_OFF = 1,
    ST_BLINKERS_ACTIVATED_ON = 2,
    ST_BLINKERS_ACTIVATED_OFF = 3,
    ST_BLINKERS_ACK_ON = 4,
    ST_BLINKERS_ACK_OFF = 5,
    ST_BLINKERS_ERROR = 6,
    ST_BLINKERS_ENDED = 255                           /* Final state */
} fsm_blinkers_state_t;

/* Events */
typedef enum {
    EV_BLINKERS_ANY = -1,                            /* Any event */
    EV_BLINKERS_NONE = 0,                            /* No event */
    EV_BLINKERS_CMD0 = 1,
    EV_BLINKERS_CMD1 = 2,
    EV_BLINKERS_ACK_REC = 3,
    EV_BLINKERS_ACK_NOT_REC = 4,
    EV_BLINKERS_1SEC = 5,
    EV_BLINKERS_ERR = 255                            /* Error event */
} fsm_blinkers_event_t;

/* Transition structure */
typedef struct {
    fsm_blinkers_state_t state;
    fsm_blinkers_event_t event;
    int (*callback)(void);
    fsm_blinkers_state_t next_state;
} tBlinkersTransition;

#define TRANS_BLINKERS_COUNT (sizeof(transBlinkers)/sizeof(*transBlinkers))

fsm_blinkers_event_t get_blinkers_next_event(fsm_blinkers_state_t current_state, blinkers_type_t which_blinker);

// This function makes ONE machine state move forward by one step
void fsm_blinkers_update(fsm_blinkers_state_t *current_state, fsm_blinkers_event_t event);

#endif