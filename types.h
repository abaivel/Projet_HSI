#include <stdint.h>

uint8_t get_cmd_position_headlights();

uint8_t get_acq_position_headlights();

uint32_t get_timer_position_headlights();

uint8_t get_cmd_low_beams_headlights();

uint8_t get_acq_low_beams_headlights();

uint32_t get_timer_low_beams_headlights();

uint8_t get_cmd_high_beams_headlights();

uint8_t get_acq_high_beams_headlights();

uint32_t get_timer_high_beams_headlights();

void set_cmd_position_headlights(uint8_t cmd);

void set_acq_position_headlights(uint8_t acq);

void set_timer_position_headlights(uint32_t timer);

void set_cmd_low_beams_headlights(uint8_t cmd);

void set_acq_low_beams_headlights(uint8_t acq);

void set_timer_low_beams_headlights(uint32_t timer);

void set_cmd_high_beams_headlights(uint8_t cmd);

void set_acq_high_beams_headlights(uint8_t acq);

void set_timer_high_beams_headlights(uint32_t timer);

typedef enum {
    POSITION_HEADLIGHTS, LOW_BEAMS_HEADLIGHTS, HIGH_BEAMS_HEADLIGHTS
} type_feu_t;

typedef struct {
    uint8_t cmd;
    uint8_t acq;
    uint32_t timer;
} t_vehicule_data;