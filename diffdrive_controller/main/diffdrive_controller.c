#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "motor_driver.h"

void app_main(void) {
  // Configure motor (adjust GPIOs as needed)
  motor_config_t motor_cfg = {
      .pwm_pin_id = GPIO_NUM_14,
      .dir_pin_id = GPIO_NUM_4,
      .pwm_channel_num = LEDC_CHANNEL_0,
      .pwm_timer_num = LEDC_TIMER_0,
  };

  motor_init(&motor_cfg);

  // Example: Spin motor forward at 50% speed
  motor_set_speed(50);
  vTaskDelay(2000 / portTICK_PERIOD_MS);

  // Reverse at 30% speed
  motor_set_speed(-30);
  vTaskDelay(2000 / portTICK_PERIOD_MS);

  // Stop motor
  motor_set_speed(0);
}
