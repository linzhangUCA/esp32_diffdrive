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
  ESP_RETURN_ON_ERROR(gpio_reset_pin(motor->in1_pin), TAG,
                      "reset IN1 pin failed");
  ESP_RETURN_ON_ERROR(gpio_reset_pin(motor->in2_pin), TAG,
                      "reset IN2 pin failed");
  ESP_RETURN_ON_ERROR(ledc_stop(LEDC_LOW_SPEED_MODE, motor->pwm_channel, 0),
                      TAG, "stop enable pin pwm channel failed");
  ESP_RETURN_ON_ERROR(gpio_reset_pin(motor->pwm_pin), TAG,
                      "reset enable pin failed");
  // ESP_RETURN_ON_ERROR(gpio_set_direction(motor->ph_pin, GPIO_MODE_OUTPUT),
  // TAG,
  //                     "set pahse pin direction failed");
  // ESP_RETURN_ON_ERROR(gpio_set_direction(motor->en_pin, GPIO_MODE_OUTPUT),
  // TAG,
  //                     "set enable pin direction failed");
  // gpio_dump_io_configuration(stdout,
  //                            (1ULL << motor->ph_pin) | (1ULL <<
  //                            motor->en_pin));
  ESP_LOGI(TAG, "disabled");
  return ESP_OK;
}

esp_err_t enable_motor(const motor_t *motor) {
  // Reset GPIO pins
  ESP_RETURN_ON_ERROR(gpio_reset_pin(motor->in1_pin), TAG,
                      "reset IN1 pin failed");
  ESP_RETURN_ON_ERROR(gpio_reset_pin(motor->in2_pin), TAG,
                      "reset IN2 pin failed");
  ESP_RETURN_ON_ERROR(gpio_reset_pin(motor->pwm_pin), TAG,
                      "reset PWM pin failed");
  // Set IN1 & IN2 to output mode
  ESP_RETURN_ON_ERROR(gpio_set_direction(motor->in1_pin, GPIO_MODE_OUTPUT), TAG,
                      "set IN1 to output failed");
  ESP_RETURN_ON_ERROR(gpio_set_direction(motor->in2_pin, GPIO_MODE_OUTPUT), TAG,
                      "set IN2 to output failed");
  // Configure PWM timer for pwm pin
  ledc_timer_config_t pwm_timer_cfg = {
      .duty_resolution = LEDC_TIMER_10_BIT, // 10-bit resolution (0-1023)
      .freq_hz = 10000,                     // 5 kHz
      .speed_mode = LEDC_LOW_SPEED_MODE,
      .timer_num = LEDC_TIMER_0,
      .clk_cfg = LEDC_AUTO_CLK,
  };
  ESP_RETURN_ON_ERROR(ledc_timer_config(&pwm_timer_cfg), TAG,
                      "ledc timer config failed");
  // Configure PWM channel for pwm pin
  ledc_channel_config_t pwm_channel_cfg = {
      .gpio_num = motor->pwm_pin,
      .speed_mode = LEDC_LOW_SPEED_MODE,
      .channel = motor->pwm_channel,
      .timer_sel = motor->pwm_timer,
      .duty = 0,
      .hpoint = 0,
  };
  ESP_RETURN_ON_ERROR(ledc_channel_config(&pwm_channel_cfg), TAG,
                      "ledc channel config failed");
  ESP_LOGI(TAG, "enabled");
  return ESP_OK;
}

esp_err_t motor_forward(motor_t *motor) {
  ESP_RETURN_ON_ERROR(gpio_set_level(motor->in1_pin, 0), TAG,
                      "set IN1 low failed");
  ESP_RETURN_ON_ERROR(gpio_set_level(motor->in2_pin, 1), TAG,
                      "set IN2 high failed");
  ESP_LOGI(TAG, "Set motor forward");
  return ESP_OK;
}

esp_err_t motor_reverse(motor_t *motor) {
  ESP_RETURN_ON_ERROR(gpio_set_level(motor->in1_pin, 1), TAG,
                      "set IN1 low failed");
  ESP_RETURN_ON_ERROR(gpio_set_level(motor->in2_pin, 0), TAG,
                      "set IN2 high failed");
  ESP_LOGI(TAG, "Set motor reverse");
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
  ESP_LOGI(TAG, "Set motor speed to %d", duty_percent);
  return ESP_OK;
}
