#include "bcgv_lib.h"

static cmd_t cmd_hazard_lights = 0;
static cmd_t cmd_position_lights = 0;
static cmd_t cmd_low_beams_headlights = 0;
static cmd_t cmd_high_beams_headlights = 0;
static cmd_t cmd_right_blinkers = 0;
static cmd_t cmd_left_blinkers = 0;
static cmd_t cmd_wipers = 0;
static cmd_t cmd_windshield_washer = 0;
static ack_t ack_position_lights = 0;
static ack_t ack_low_beams_headlights = 0;
static ack_t ack_high_beams_headlights = 0;
static ack_t ack_right_blinkers = 0;
static ack_t ack_left_blinkers = 0;
static ack_t ack_hazard_lights = 0;
static timer_bcgv_t timer_position_lights = 0;
static timer_bcgv_t timer_low_beams_headlights = 0;
static timer_bcgv_t timer_high_beams_headlights = 0;
static timer_bcgv_t timer_right_blinkers = 0;
static timer_bcgv_t timer_left_blinkers = 0;
static timer_bcgv_t timer_hazard_lights = 0;
static timer_bcgv_t timer_wipers = 0;
static thread_number_t thread_number = 0;
static mileage_t mileage = 0;
static speed_t speed = 0;
static chassis_problems_type_t chassis_problems = NO_CHASSIS_PROBLEM;
static engine_problems_type_t engine_problems = NO_ENGINE_PROBLEM;
static tank_level_t tank_level = 0;
static revolutions_minute_t revolutions_minute = 0;
static battery_problems_type_t battery_problems = NO_BATTERY_PROBLEM;
static crc8_t crc8 = 0;


cmd_t get_cmd_hazard_lights(){
  return cmd_hazard_lights;
}

void set_cmd_hazard_lights(cmd_t new_value){
  if (new_value >= 0 && new_value <= 1){
      cmd_hazard_lights = new_value;
  }
}

cmd_t get_cmd_position_lights(){
  return cmd_position_lights;
}

void set_cmd_position_lights(cmd_t new_value){
  if (new_value >= 0 && new_value <= 1){
      cmd_position_lights = new_value;
  }
}

cmd_t get_cmd_low_beams_headlights(){
  return cmd_low_beams_headlights;
}

void set_cmd_low_beams_headlights(cmd_t new_value){
  if (new_value >= 0 && new_value <= 1){
      cmd_low_beams_headlights = new_value;
  }
}

cmd_t get_cmd_high_beams_headlights(){
  return cmd_high_beams_headlights;
}

void set_cmd_high_beams_headlights(cmd_t new_value){
  if (new_value >= 0 && new_value <= 1){
      cmd_high_beams_headlights = new_value;
  }
}

cmd_t get_cmd_right_blinkers(){
  return cmd_right_blinkers;
}

void set_cmd_right_blinkers(cmd_t new_value){
  if (new_value >= 0 && new_value <= 1){
      cmd_right_blinkers = new_value;
  }
}

cmd_t get_cmd_left_blinkers(){
  return cmd_left_blinkers;
}

void set_cmd_left_blinkers(cmd_t new_value){
  if (new_value >= 0 && new_value <= 1){
      cmd_left_blinkers = new_value;
  }
}

cmd_t get_cmd_wipers(){
  return cmd_wipers;
}

void set_cmd_wipers(cmd_t new_value){
  if (new_value >= 0 && new_value <= 1){
      cmd_wipers = new_value;
  }
}

cmd_t get_cmd_windshield_washer(){
  return cmd_windshield_washer;
}

void set_cmd_windshield_washer(cmd_t new_value){
  if (new_value >= 0 && new_value <= 1){
      cmd_windshield_washer = new_value;
  }
}

ack_t get_ack_position_lights(){
  return ack_position_lights;
}

void set_ack_position_lights(ack_t new_value){
  if (new_value >= 0 && new_value <= 1){
      ack_position_lights = new_value;
  }
}

ack_t get_ack_low_beams_headlights(){
  return ack_low_beams_headlights;
}

void set_ack_low_beams_headlights(ack_t new_value){
  if (new_value >= 0 && new_value <= 1){
      ack_low_beams_headlights = new_value;
  }
}

ack_t get_ack_high_beams_headlights(){
  return ack_high_beams_headlights;
}

void set_ack_high_beams_headlights(ack_t new_value){
  if (new_value >= 0 && new_value <= 1){
      ack_high_beams_headlights = new_value;
  }
}

ack_t get_ack_right_blinkers(){
  return ack_right_blinkers;
}

void set_ack_right_blinkers(ack_t new_value){
  if (new_value >= 0 && new_value <= 1){
      ack_right_blinkers = new_value;
  }
}

ack_t get_ack_left_blinkers(){
  return ack_left_blinkers;
}

void set_ack_left_blinkers(ack_t new_value){
  if (new_value >= 0 && new_value <= 1){
      ack_left_blinkers = new_value;
  }
}

ack_t get_ack_hazard_lights(){
  return ack_hazard_lights;
}

void set_ack_hazard_lights(ack_t new_value){
  if (new_value >= 0 && new_value <= 1){
      ack_hazard_lights = new_value;
  }
}

timer_bcgv_t get_timer_position_lights(){
  return timer_position_lights;
}

void set_timer_position_lights(timer_bcgv_t new_value){
  timer_position_lights = new_value;
}

timer_bcgv_t get_timer_low_beams_headlights(){
  return timer_low_beams_headlights;
}

void set_timer_low_beams_headlights(timer_bcgv_t new_value){
  timer_low_beams_headlights = new_value;
}

timer_bcgv_t get_timer_high_beams_headlights(){
  return timer_high_beams_headlights;
}

void set_timer_high_beams_headlights(timer_bcgv_t new_value){
  timer_high_beams_headlights = new_value;
}

timer_bcgv_t get_timer_right_blinkers(){
  return timer_right_blinkers;
}

void set_timer_right_blinkers(timer_bcgv_t new_value){
  timer_right_blinkers = new_value;
}

timer_bcgv_t get_timer_left_blinkers(){
  return timer_left_blinkers;
}

void set_timer_left_blinkers(timer_bcgv_t new_value){
  timer_left_blinkers = new_value;
}

timer_bcgv_t get_timer_hazard_lights(){
  return timer_hazard_lights;
}

void set_timer_hazard_lights(timer_bcgv_t new_value){
  timer_hazard_lights = new_value;
}

timer_bcgv_t get_timer_wipers(){
  return timer_wipers;
}

void set_timer_wipers(timer_bcgv_t new_value){
  timer_wipers = new_value;
}

thread_number_t get_thread_number(){
  return thread_number;
}

void set_thread_number(thread_number_t new_value){
  if (new_value >= 1 && new_value <= 100){
      thread_number = new_value;
  }
}

mileage_t get_mileage(){
  return mileage;
}

void set_mileage(mileage_t new_value){
  mileage = new_value;
}

speed_t get_speed(){
  return speed;
}

void set_speed(speed_t new_value){
  if (new_value >= 0 && new_value <= 255){
      speed = new_value;
  }
}

chassis_problems_type_t get_chassis_problems(){
  return chassis_problems;
}

void set_chassis_problems(chassis_problems_type_t new_value){
  chassis_problems = new_value;
}

engine_problems_type_t get_engine_problems(){
  return engine_problems;
}

void set_engine_problems(engine_problems_type_t new_value){
  engine_problems = new_value;
}

tank_level_t get_tank_level(){
  return tank_level;
}

void set_tank_level(tank_level_t new_value){
  if (new_value >= 0 && new_value <= 40){
      tank_level = new_value;
  }
}

revolutions_minute_t get_revolutions_minute(){
  return revolutions_minute;
}

void set_revolutions_minute(revolutions_minute_t new_value){
  if (new_value >= 0 && new_value <= 10000){
      revolutions_minute = new_value;
  }
}

battery_problems_type_t get_battery_problems(){
  return battery_problems;
}

void set_battery_problems(battery_problems_type_t new_value){
  battery_problems = new_value;
}

crc8_t get_crc8(){
  return crc8;
}

void set_crc8(crc8_t new_value){
  if (new_value >= 0 && new_value <= 255){
      crc8 = new_value;
  }
}

