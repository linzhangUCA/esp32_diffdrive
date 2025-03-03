#pragma once
#include <stdint.h>

#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_err.h"

/**
 * @brief PH/EN Motor Configuration (Phase: direction; Enable: PWM)
 */
typedef struct {
  gpio_num_t ph_pin;          // Direction pin (e.g., GPIO_NUM_2)
  gpio_num_t en_pin;          // PWM pin (e.g., GPIO_NUM_1)
  ledc_channel_t pwm_channel; // LEDC channel (e.g., LEDC_CHANNEL_0)
  ledc_timer_t pwm_timer;     // LEDC timer (e.g., LEDC_TIMER_0)
} motor_t;

/**
 * @brief Enable motor
 *
 * @param motor: Motor struct
 *
 * @return
 *      - ESP_OK: Enable motor successfully
 *      - ESP_ERR_INVALID_ARG: Enable motor failed because of invalid parameters
 *      - ESP_FAIL: Enable motor failed because other error occurred
 */
esp_err_t enable_motor(const motor_t *motor);

/**
 * @brief Disable motor
 *
 * @param motor: Motor struct
 *
 * @return
 *      - ESP_OK: Disable motor successfully
 *      - ESP_ERR_INVALID_ARG: Disable motor failed because of invalid
 * parameters
 *      - ESP_FAIL: Disable motor failed because other error occurred
 */
void disable_motor(const motor_t *motor);

/**
 * @brief Set speed for motor
 *
 * @param motor: Motor struct
 * @param speed: Motor speed (dutycycle), -32,768 (reverse) to 32,767 (forward)
 *
 * @return
 *      - ESP_OK: Set motor speed successfully
 *      - ESP_ERR_INVALID_ARG: Set motor speed failed because of invalid
 * parameters
 *      - ESP_FAIL: Set motor speed failed because other error occurred
 */
esp_err_t motor_set_speed(motor_t *motor, int16_t duty_percent);

/**
 * @brief Set motor direction to forward
 *
 * @param motor: Motor struct
 *
 * @return
 *      - ESP_OK: Forward motor successfully
 *      - ESP_FAIL: Forward motor failed because some other error occurred
 */
esp_err_t motor_forward(motor_t *motor);

/**
 * @brief Reverse motor direction
 *
 * @param strip: Motor struct
 *
 * @return
 *      - ESP_OK: Reverse motor successfully
 *      - ESP_FAIL: Reverse motor failed because some other error occurred
 */
esp_err_t motor_reverse(motor_t *motor);
