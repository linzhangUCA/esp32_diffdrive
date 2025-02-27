#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "motor_driver.h"

void app_main(void) {
  // Configure motor (adjust GPIOs as needed)
  motor_config_t motor_cfg = {
      .pwm_pin = GPIO_NUM_14,
      .dir_pin = GPIO_NUM_4,
      .pwm_channel = LEDC_CHANNEL_0,
      .pwm_timer = LEDC_TIMER_0,
  };

  motor_init(&motor_cfg);
  int speed_p;
  for (speed_p = 0; speed_p < 100; ++speed_p) {
    motor_set_speed(speed_p);
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
  for (speed_p = 100; speed_p > -100; --speed_p) {
    motor_set_speed(speed_p);
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }

  // Stop motor
  motor_set_speed(0);
}
