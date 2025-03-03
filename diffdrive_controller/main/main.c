#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "motor_driver.h"

static const char *TAG = "MotorTest";
int duty_pct;

void app_main(void) {
  // Configure motor (adjust GPIOs as needed)
  motor_t motor = {
      .ph_pin = GPIO_NUM_13,
      .en_pin = GPIO_NUM_14,
      .pwm_channel = LEDC_CHANNEL_0,
      .pwm_timer = LEDC_TIMER_0,
  };

  enable_motor(&motor);

  for (duty_pct = 0; duty_pct < 100; duty_pct = duty_pct + 5) {
    motor_set_speed(&motor, duty_pct);
    ESP_LOGI(TAG, "forward up");
    vTaskDelay(200 / portTICK_PERIOD_MS);
  }
  for (duty_pct = 100; duty_pct > 0; duty_pct = duty_pct - 5) {
    motor_set_speed(&motor, duty_pct);
    ESP_LOGI(TAG, "forward down");
    vTaskDelay(200 / portTICK_PERIOD_MS);
  }
  for (duty_pct = 0; duty_pct > -100; duty_pct = duty_pct - 5) {
    motor_set_speed(&motor, duty_pct);
    ESP_LOGI(TAG, "reverse up");
    vTaskDelay(200 / portTICK_PERIOD_MS);
  }
  for (duty_pct = -100; duty_pct < 0; duty_pct = duty_pct + 5) {
    motor_set_speed(&motor, duty_pct);
    ESP_LOGI(TAG, "reverse down");
    vTaskDelay(200 / portTICK_PERIOD_MS);
  }

  // Stop motor
  motor_set_speed(&motor, 0);
  disable_motor(&motor);
}
