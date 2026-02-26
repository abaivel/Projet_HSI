#include "bcgv_lib.h"

/* States */
typedef enum {
    ST_ANY = -1,                            /* Any state */
    ST_INIT = 0,                            /* Init state */
    ST_ETEINTS = 1,
    ST_ACTIVES_ALLUMES = 2,
    ST_ACTIVES_ETEINTS = 3,
    ST_ACQUITTES_VOYANT_ALLUME = 4,
    ST_ACQUITTES_VOYANT_ETEINT = 5,
    ST_ERREUR = 6,
    ST_TERM = 255                           /* Final state */
} fsm_blinkers_state_t;

/* Events */
typedef enum {
    EV_ANY = -1,                            /* Any event */
    EV_NONE = 0,                            /* No event */
    EV_CMD0 = 1,
    EV_CMD1 = 2,
    EV_ACQ_RECU = 3,
    EV_ACQ_NON_RECU = 4,
    EV_1SEC = 5,
    EV_ERR = 255                            /* Error event */
} fsm_blinkers_event_t;

static int FsmError(void);

/* Transition structure */
typedef struct {
    fsm_blinkers_state_t state;
    fsm_blinkers_event_t event;
    int (*callback)(void);
    fsm_blinkers_state_t next_state;
} tTransition;

#define TRANS_COUNT (sizeof(trans)/sizeof(*trans))

fsm_blinkers_state_t get_next_event(fsm_blinkers_state_t current_state, blinkers_type_t which_blinker);

// This function makes ONE machine state move forward by one step
void fsm_blinker_update(fsm_blinkers_event_t *current_state, fsm_blinkers_state_t event);