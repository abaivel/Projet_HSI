#include "bcgv_lib.h"

typedef enum {
    ST_ANY = -1,                            /* Any state */
    ST_INIT = 0,                            /* Init state */
    ST_ETEINTS = 1,
    ST_ALLUMES = 2,
    ST_ACQUITTES = 3,
    ST_ERREUR = 4,
    ST_TERM = 255                           /* Final state */
} fsm_feux_state_t;

/* Events */
typedef enum {
    EV_ANY = -1,                            /* Any event */
    EV_NONE = 0,                            /* No event */
    EV_CMD0 = 1,
    EV_CMD1 = 2,
    EV_ACQ_RECU = 3,
    EV_ACQ_NON_RECU = 4,
    EV_ERR = 255                            /* Error event */
} fsm_feux_event_t;

/* Callback functions called on transitions */

static int FsmError(void);

/* Transition structure */
typedef struct {
    fsm_feux_state_t state;
    fsm_feux_event_t event;
    int (*callback)(void);
    fsm_feux_state_t next_state;
} tTransition;


#define TRANS_COUNT (sizeof(trans)/sizeof(*trans))

fsm_feux_event_t get_next_event(fsm_feux_state_t current_state, light_type_t quel_feu);

// This function makes ONE machine state move forward by one step
void fsm_update(fsm_feux_state_t *current_state, fsm_feux_event_t event);