#include "motor_driver.h"

void app_main(void) {
    // Configure motor (adjust GPIOs as needed)
    motor_config_t motor_cfg = {
        .pwm_pin = GPIO_NUM_1,
        .dir_pin = GPIO_NUM_2,
        .pwm_channel = LEDC_CHANNEL_0,
        .pwm_timer = LEDC_TIMER_0,
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
