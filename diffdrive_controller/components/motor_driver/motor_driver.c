#include "motor_driver.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_check.h"
#include "esp_err.h"
#include "esp_log.h"
#include "hal/gpio_types.h"
#include "hal/ledc_types.h"

static const char *TAG = "MotorDriver";

esp_err_t disable_motor(const motor_t *motor) {
  ESP_RETURN_ON_ERROR(gpio_reset_pin(motor->ph_pin), TAG,
                      "reset phase pin failed");
  ESP_RETURN_ON_ERROR(ledc_stop(LEDC_LOW_SPEED_MODE, motor->pwm_channel, 0),
                      TAG, "stop enable pin pwm channel failed");
  ESP_RETURN_ON_ERROR(gpio_reset_pin(motor->en_pin), TAG,
                      "reset enable pin failed");
  ESP_RETURN_ON_ERROR(gpio_set_direction(motor->ph_pin, GPIO_MODE_OUTPUT), TAG,
                      "set pahse pin direction failed");
  ESP_RETURN_ON_ERROR(gpio_set_direction(motor->en_pin, GPIO_MODE_OUTPUT), TAG,
                      "set enable pin direction failed");
  // gpio_dump_io_configuration(stdout,
  //                            (1ULL << motor->ph_pin) | (1ULL <<
  //                            motor->en_pin));
  return ESP_OK;
  ESP_LOGI(TAG, "disabled");
}

esp_err_t enable_motor(const motor_t *motor) {
  // Configure DIR pin as output
  gpio_config_t ph_pin_conf = {
      .intr_type = GPIO_INTR_DISABLE, // Disable interrupt
      .mode = GPIO_MODE_OUTPUT,       // Set as output mode
      .pin_bit_mask = motor->ph_pin,  // Mark phase pin
      .pull_up_en = 0,                // Disable pull-up
      .pull_down_en = 0,              // Disable pull-down
  };
  ESP_RETURN_ON_ERROR(gpio_config(&ph_pin_conf), TAG,
                      "config phase pin failed");
  // ESP_RETURN_ON_ERROR(gpio_reset_pin(motor->ph_pin), TAG,
  //                                           "reset gpio failed");
  ESP_RETURN_ON_ERROR(gpio_set_direction(motor->ph_pin, GPIO_MODE_OUTPUT), TAG,
                      "set phase pin direction failed");

  // Configure PWM timer for pwm pin
  ledc_timer_config_t pwm_timer = {
      .duty_resolution = LEDC_TIMER_10_BIT, // 10-bit resolution (0-1023)
      .freq_hz = 5000,                      // 5 kHz
      .speed_mode = LEDC_LOW_SPEED_MODE,
      .timer_num = LEDC_TIMER_0,
      .clk_cfg = LEDC_AUTO_CLK,
  };
  ESP_RETURN_ON_ERROR(ledc_timer_config(&pwm_timer), TAG,
                      "ledc timer config failed");

  // Configure PWM channel for pwm pin
  ledc_channel_config_t pwm_channel = {
      .gpio_num = motor->en_pin,
      .speed_mode = LEDC_LOW_SPEED_MODE,
      .channel = motor->pwm_channel,
      .timer_sel = motor->pwm_timer,
      .duty = 0,
      .hpoint = 0,
  };
  ESP_RETURN_ON_ERROR(ledc_channel_config(&pwm_channel), TAG,
                      "ledc channel config failed");

  return ESP_OK;
  ESP_LOGI(TAG, "enabled");
}

esp_err_t motor_forward(motor_t *motor) {
  ESP_RETURN_ON_ERROR(gpio_set_level(motor->ph_pin, 0), TAG,
                      "set forward failed");
  return ESP_OK;
}

esp_err_t motor_reverse(motor_t *motor) {
  ESP_RETURN_ON_ERROR(gpio_set_level(motor->ph_pin, 1), TAG,
                      "set reverse failed");
  return ESP_OK;
}

esp_err_t motor_set_speed(motor_t *motor, int16_t duty_percent) {
  // Set directions, and clamp duty_percent between [-100, 100]
  if (duty_percent >= 0) {
    if (duty_percent > 100) {
      duty_percent = 100;
    }
    motor_forward(motor);
  } else {
    if (duty_percent < -100) {
      duty_percent = -100;
    }
    motor_reverse(motor);
  }

  // Convert percentage to duty cycle (0-1023)
  uint32_t duty = (abs(duty_percent) * 1023) / 100;

  // Update PWM duty
  ESP_RETURN_ON_ERROR(
      ledc_set_duty(LEDC_LOW_SPEED_MODE, motor->pwm_channel, duty), TAG,
      "set dutycycle failed");
  ESP_RETURN_ON_ERROR(ledc_update_duty(LEDC_LOW_SPEED_MODE, motor->pwm_channel),
                      TAG, "update dutycycle failed");
  return ESP_OK;
}
