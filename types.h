#include <stdint.h>

uint8_t get_cmd_feux_position();

uint8_t get_acq_feux_position();

uint32_t get_timer_feux_position();

uint8_t get_cmd_feux_croisement();

uint8_t get_acq_feux_croisement();

uint32_t get_timer_feux_croisement();

uint8_t get_cmd_feux_route();

uint8_t get_acq_feux_route();

uint32_t get_timer_feux_route();

void set_cmd_feux_position(uint8_t cmd);

void set_acq_feux_position(uint8_t acq);

void set_timer_feux_position(uint32_t timer);

void set_cmd_feux_croisement(uint8_t cmd);

void set_acq_feux_croisement(uint8_t acq);

void set_timer_feux_croisement(uint32_t timer);

void set_cmd_feux_route(uint8_t cmd);

void set_acq_feux_route(uint8_t acq);

void set_timer_feux_route(uint32_t timer);

typedef enum {
    FEU_POSITION, FEU_CROISEMENT, FEU_ROUTE
} type_feu_t;

typedef struct {
    uint8_t cmd;
    uint8_t acq;
    uint32_t timer;
} t_vehicule_data;