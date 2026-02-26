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
} fsm_wipers_state_t;

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
} fsm_wipers_event_t;

/* Callback functions called on transitions */
static int FsmError(void);

/* Transition structure */
typedef struct {
    fsm_wipers_state_t state;
    fsm_wipers_event_t event;
    int (*callback)(void);
    int next_state;
} tTransition;

#define TRANS_COUNT (sizeof(trans)/sizeof(*trans))

fsm_wipers_event_t get_next_event(fsm_wipers_event_t current_state);