#pragma once
#include "driver/gpio.h"
#include "driver/pulse_cnt.h"
#include "esp_log.h"

typedef struct {
  gpio_num_t enc_a_pin;         // Encoder A phase GPIO (e.g., GPIO_NUM_4)
  gpio_num_t enc_b_pin;         // Encoder B phase GPIO (e.g., GPIO_NUM_5)
  // pcnt_unit_handle_t pcnt_unit; // PCNT unit handle
  int16_t counts_per_rev;       // Encoder CCP (PPR * 4)
} encoder_config_t;

// Initialize encoder
pcnt_unit_handle_t encoder_init(const encoder_config_t *config);

// Get current encoder count (ticks)
// esp_err_t encoder_get_count(pcnt_unit_handle_t pcnt_unit, int *count);

// Calculate angular velocity (rad/s)
// float encoder_get_velocity(pcnt_unit_handle_t pcnt_unit, float delta_time_sec);
