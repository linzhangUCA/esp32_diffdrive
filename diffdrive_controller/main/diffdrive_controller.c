#include "motor_driver.h"

#define LEFT_MOTOR_PWM_PIN GPIO_NUM_4
#define LEFT_MOTOR_DIR_PIN GPIO_NUM_5
#define LEFT_MOTOR_PWM_CHANNEL LEDC_CHANNEL_0
#define LEFT_MOTOR_PWM_TIMER LEDC_TIMER_0

void app_main() {
  // 初始化左电机
  MotorConfig left_motor = {.dir_pin = LEFT_MOTOR_DIR_PIN,
                            .pwm_pin = LEFT_MOTOR_PWM_PIN,
                            .pwm_channel = LEFT_MOTOR_PWM_CHANNEL,
                            .pwm_timer = LEFT_MOTOR_PWM_TIMER};
  motor_init(&left_motor, 20000); // 20kHz PWM频率

  // 控制演示
  while (1) {
    // 正向50%速度
    motor_set_speed(&left_motor, 50.0, 0);
    vTaskDelay(pdMS_TO_TICKS(2000));

    // 反向75%速度
    motor_set_speed(&left_motor, 75.0, 0);
    vTaskDelay(pdMS_TO_TICKS(2000));

    motor_set_speed(&left_motor, 0.0, 0);
    vTaskDelay(pdMS_TO_TICKS(1000));

    // 正向50%速度
    motor_set_speed(&left_motor, 50.0, 1);
    vTaskDelay(pdMS_TO_TICKS(2000));

    // 反向75%速度
    motor_set_speed(&left_motor, 75.0, 1);
    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}
