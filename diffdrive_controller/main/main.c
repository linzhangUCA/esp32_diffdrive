#include "encoder.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "motor_driver.h"

static const char *TAG = "MotorTest";
int duty_pct;

void app_main(void) {
  // Configure motor (adjust GPIOs as needed)
  motor_config_t motor_cfg = {
      .pwm_pin = GPIO_NUM_10,
      .in1_pin = GPIO_NUM_11,
      .in2_pin = GPIO_NUM_12,
      .pwm_channel = LEDC_CHANNEL_0,
      .pwm_timer = LEDC_TIMER_0,
  };
  enable_motor(&motor_cfg);

  // Initialize encoder
  encoder_config_t encoder_cfg = {
      .enc_a_pin = GPIO_NUM_13,
      .enc_b_pin = GPIO_NUM_14,
      .pcnt_unit = NULL,
      .counts_per_rev = 4096,
  };
  int pulse_count = 0;
  float ang_vel = 0.0;
  enable_encoder(&encoder_cfg);

  // Ramp up forward
  for (duty_pct = 0; duty_pct < 100; duty_pct = duty_pct + 5) {
    motor_set_speed(&motor_cfg, duty_pct);
    vTaskDelay(200 / portTICK_PERIOD_MS);
    ang_vel = encoder_get_ang_vel(&encoder_cfg, 0.2);
    ESP_LOGI(TAG, "angular velocity: %f", ang_vel);
  }
  // Ramp down forward
  for (duty_pct = 100; duty_pct > 0; duty_pct = duty_pct - 5) {
    motor_set_speed(&motor_cfg, duty_pct);
    pcnt_unit_get_count(encoder_cfg.pcnt_unit, &pulse_count);
    vTaskDelay(200 / portTICK_PERIOD_MS);
    ang_vel = encoder_get_ang_vel(&encoder_cfg, 0.2);
    ESP_LOGI(TAG, "angular velocity: %f", ang_vel);
  }
  // Ramp up reverse
  for (duty_pct = 0; duty_pct > -100; duty_pct = duty_pct - 5) {
    motor_set_speed(&motor_cfg, duty_pct);
    pcnt_unit_get_count(encoder_cfg.pcnt_unit, &pulse_count);
    vTaskDelay(200 / portTICK_PERIOD_MS);
    ang_vel = encoder_get_ang_vel(&encoder_cfg, 0.2);
    ESP_LOGI(TAG, "angular velocity: %f", ang_vel);
  }
  // Ramp down reverse
  for (duty_pct = -100; duty_pct < 0; duty_pct = duty_pct + 5) {
    motor_set_speed(&motor_cfg, duty_pct);
    pcnt_unit_get_count(encoder_cfg.pcnt_unit, &pulse_count);
    vTaskDelay(200 / portTICK_PERIOD_MS);
    ang_vel = encoder_get_ang_vel(&encoder_cfg, 0.2);
    ESP_LOGI(TAG, "angular velocity: %f", ang_vel);
  }

  // Stop then disable motor
  motor_brake(&motor_cfg);
  disable_motor(&motor_cfg);
}
