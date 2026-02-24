//#ifndef DRV_API_H_
#define DRV_API_H_

#include <stdint.h>

#define SER_NUM_BGF 11
#define SER_NUM_COMODO 12

typedef uint8_t cmd_t;
typedef uint8_t acq_t;
//typedef uint32_t timer_t;
typedef uint8_t thread_number_t;
typedef uint32_t mileage_t;
typedef uint8_t speed_t;
typedef uint8_t tank_level_t;
typedef uint32_t revolutions_minute_t;
typedef uint8_t crc8_t;

typedef enum {
    POSITION_LIGHTS, LOW_BEAMS_HEADLIGHTS, HIGH_BEAMS_HEADLIGHTS
} light_type_t;

typedef enum {
    RIGHT_BLINKERS, LEFT_BLINKERS, HAZARD_LIGHTS
} blinkers_type_t;

typedef enum {
    NO_CHASSIS_PROBLEM = 0, TIRE_PRESSURE = 1, BRAKE_FAILURE = 2
} chassis_problems_type_t;

typedef enum {
    NO_ENGINE_PROBLEM = 0, PRESSURE_DEFAULT = 1, COOLANT_TEMPERATURE = 2, OIL_OVERHEATING
} engine_problems_type_t;

typedef enum {
    NO_BATTERY_PROBLEM = 0, DISCHARGED = 1, FAILURE = 2
} battery_problems_type_t;

typedef enum {
    POSITION_LIGHTS_ACTIVATION = 1, LOW_BEAMS_HEADLIGHTS_ACTIVATION = 2, HIGH_BEAMS_HEADLIGHTS_ACTIVATION = 3, RIGHT_BLINKERS_ACTIVATION = 4, LEFT_BLINKERS_ACTIVATION = 5
} message_id_to_bgf_t;

typedef enum {
    OFF=0, ON=1
} state_lights_t;

cmd_t get_cmd_hazard_lights();
void set_cmd_hazard_lights(cmd_t new_value);

cmd_t get_cmd_position_lights();
void set_cmd_position_lights(cmd_t new_value);

cmd_t get_cmd_low_beams_headlights();
void set_cmd_low_beams_headlights(cmd_t new_value);

cmd_t get_cmd_high_beams_headlights();
void set_cmd_high_beams_headlights(cmd_t new_value);

cmd_t get_cmd_right_blinkers();
void set_cmd_right_blinkers(cmd_t new_value);

cmd_t get_cmd_left_blinkers();
void set_cmd_left_blinkers(cmd_t new_value);

cmd_t get_cmd_wipers();
void set_cmd_wipers(cmd_t new_value);

cmd_t get_cmd_windshield_washer();
void set_cmd_windshield_washer(cmd_t new_value);

acq_t get_acq_position_lights();
void set_acq_position_lights(acq_t new_value);

acq_t get_acq_low_beams_headlights();
void set_acq_low_beams_headlights(acq_t new_value);

acq_t get_acq_high_beams_headlights();
void set_acq_high_beams_headlights(acq_t new_value);

acq_t get_acq_right_blinkers();
void set_acq_right_blinkers(acq_t new_value);

acq_t get_acq_left_blinkers();
void set_acq_left_blinkers(acq_t new_value);

acq_t get_acq_hazard_lights();
void set_acq_hazard_lights(acq_t new_value);

thread_number_t get_thread_number();
void set_thread_number(thread_number_t new_value);

mileage_t get_mileage();
void set_mileage(mileage_t new_value);

speed_t get_speed();
void set_speed(speed_t new_value);

chassis_problems_type_t get_chassis_problems();
void set_chassis_problems(chassis_problems_type_t new_value);

engine_problems_type_t get_engine_problems();
void set_engine_problems(engine_problems_type_t new_value);

tank_level_t get_tank_level();
void set_tank_level(tank_level_t new_value);

revolutions_minute_t get_revolutions_minute();
void set_revolutions_minute(revolutions_minute_t new_value);

battery_problems_type_t get_battery_problems();
void set_battery_problems(battery_problems_type_t new_value);

crc8_t get_crc8();
void set_crc8(crc8_t new_value);


//#endif