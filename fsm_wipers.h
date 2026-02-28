#include "bcgv_lib.h"

/* States */
typedef enum {
    ST_WIPERS_WASHERS_ANY = -1,                            /* Any state */
    ST_WIPERS_WASHERS_INIT = 0,                            /* Init state */
    ST_WIPERS_WASHERS_ALL_OFF = 1,
    ST_WIPERS_ACTIVE = 2,
    ST_WIPERS_WASHERS_ACTIVE = 3,
    ST_WIPERS_WASHERS_TIMER_OFF = 4,
    ST_WIPERS_WASHERS_TERM = 255                           /* Final state */
} fsm_wipers_state_t;

/* Events */
typedef enum {
    EV_WIPERS_WASHERS_ANY = -1,                            /* Any event */
    EV_WIPERS_WASHERS_NONE = 0,                            /* No event */
    EV_WIPERS_WASHERS_CMD_EG0 = 1,
    EV_WIPERS_WASHERS_CMD_EG1 = 2,
    EV_WIPERS_WASHERS_CMD_LG0 = 3,
    EV_WIPERS_WASHERS_CMD_LG1 = 4,
    EV_WIPERS_WASHERS_CMD_EG0_CMD_LG0 = 5,
    EV_WIPERS_WASHERS_LESS_2SEC = 6,
    EV_WIPERS_WASHERS_MORE_2SEC = 7,
    EV_WIPERS_WASHERS_ERR = 255                            /* Error event */
} fsm_wipers_event_t;

/* Callback functions called on transitions */
static int FsmError(void);

/* Transition structure */
typedef struct {
    fsm_wipers_state_t state;
    fsm_wipers_event_t event;
    int (*callback)(void);
    int next_state;
} tWipersTransition;

#define TRANS_WIPERS_COUNT (sizeof(transWipers)/sizeof(*transWipers))

fsm_wipers_event_t get_wipers_next_event(fsm_wipers_state_t current_state);

void fsm_wipers_update(fsm_wipers_state_t *current_state, fsm_wipers_event_t event);