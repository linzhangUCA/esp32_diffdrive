#include "encoder.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "motor_driver.h"

static const char *TAG = "MotorTest";
int duty_pct;

void app_main(void) {
  // Configure motor (adjust GPIOs as needed)
  motor_t motor = {
      .pwm_pin = GPIO_NUM_10,
      .in1_pin = GPIO_NUM_11,
      .in2_pin = GPIO_NUM_12,
      .pwm_channel = LEDC_CHANNEL_0,
      .pwm_timer = LEDC_TIMER_0,
  };
  enable_motor(&motor);

  // Initialize encoder
  encoder_config_t enc_cfg = {
      .enc_a_pin = GPIO_NUM_13,
      .enc_b_pin = GPIO_NUM_14,
      // .pcnt_unit = NULL,
      .counts_per_rev = 4096,
  };
  pcnt_unit_handle_t encoder;
  int pulse_count = 0;
  encoder = encoder_init(&enc_cfg);

  // Ramp up forward
  for (duty_pct = 0; duty_pct < 100; duty_pct = duty_pct + 5) {
    motor_set_speed(&motor, duty_pct);
    pcnt_unit_get_count(encoder, &pulse_count);
    ESP_LOGI(TAG, "Pulse count: %d", pulse_count);
    vTaskDelay(200 / portTICK_PERIOD_MS);
  }
  // Ramp down forward
  for (duty_pct = 100; duty_pct > 0; duty_pct = duty_pct - 5) {
    motor_set_speed(&motor, duty_pct);
    pcnt_unit_get_count(encoder, &pulse_count);
    ESP_LOGI(TAG, "Pulse count: %d", pulse_count);
    vTaskDelay(200 / portTICK_PERIOD_MS);
  }
  // Ramp up reverse
  for (duty_pct = 0; duty_pct > -100; duty_pct = duty_pct - 5) {
    motor_set_speed(&motor, duty_pct);
    pcnt_unit_get_count(encoder, &pulse_count);
    ESP_LOGI(TAG, "Pulse count: %d", pulse_count);
    vTaskDelay(200 / portTICK_PERIOD_MS);
  }
  // Ramp down reverse
  for (duty_pct = -100; duty_pct < 0; duty_pct = duty_pct + 5) {
    motor_set_speed(&motor, duty_pct);
    pcnt_unit_get_count(encoder, &pulse_count);
    ESP_LOGI(TAG, "Pulse count: %d", pulse_count);
    vTaskDelay(200 / portTICK_PERIOD_MS);
  }

  // Stop then disable motor
  motor_brake(&motor);
  disable_motor(&motor);
}
