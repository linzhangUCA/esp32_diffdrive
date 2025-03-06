#pragma once
#include "driver/gpio.h"
#include "driver/pulse_cnt.h"
#include "esp_log.h"

typedef struct {
  gpio_num_t enc_a_pin;         // Encoder A phase GPIO (e.g., GPIO_NUM_4)
  gpio_num_t enc_b_pin;         // Encoder B phase GPIO (e.g., GPIO_NUM_5)
  pcnt_unit_handle_t pcnt_unit; // PCNT unit handle
  int16_t counts_per_rev;       // Encoder CPR (PPR * 4)
} encoder_config_t;

/**
 * @brief Enable encoder
 *
 * @param motor: Encoder struct
 *
 * @return
 *      - ESP_OK: Enable encoder successfully
 *      - ESP_ERR_INVALID_ARG: Enable encoder failed because of invalid
 * parameters
 *      - ESP_FAIL: Enable encoder failed because other error occurred
 */
esp_err_t enable_encoder(encoder_config_t *encoder);

// Calculate angular velocity (rad/s)
float encoder_get_ang_vel(encoder_config_t *encoder, float dt);
