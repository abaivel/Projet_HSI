#include "bcgv_lib.h"

typedef enum {
    ST_LIGHTS_ANY = -1,                            /* Any state */
    ST_LIGHTS_INIT = 0,                            /* Init state */
    ST_LIGHTS_OFF = 1,
    ST_LIGHTS_ON = 2,
    ST_LIGHTS_ACK = 3,
    ST_LIGHTS_ERROR = 4,
    ST_LIGHTS_ENDED = 255                           /* Final state */
} fsm_lights_state_t;

/* Events */
typedef enum {
    EV_LIGHTS_ANY = -1,                            /* Any event */
    EV_LIGHTS_NONE = 0,                            /* No event */
    EV_LIGHTS_CMD0 = 1,
    EV_LIGHTS_CMD1 = 2,
    EV_LIGHTS_ACK_REC = 3,
    EV_LIGHTS_ACK_NOT_REC = 4,
    EV_LIGHTS_ERR = 255                            /* Error event */
} fsm_lights_event_t;

/* Callback functions called on transitions */

static int FsmError(void);

/* Transition structure */
typedef struct {
    fsm_lights_state_t state;
    fsm_lights_event_t event;
    int (*callback)(void);
    fsm_lights_state_t next_state;
} tTransition;


#define TRANS_LIGHTS_COUNT (sizeof(transLights)/sizeof(*transLights))

fsm_lights_event_t get_lights_next_event(fsm_lights_state_t current_state, light_type_t which_lights);

// This function makes ONE machine state move forward by one step
void fsm_lights_update(fsm_lights_state_t *current_state, fsm_lights_event_t event);