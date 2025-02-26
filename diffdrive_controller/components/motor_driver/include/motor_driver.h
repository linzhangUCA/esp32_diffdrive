#pragma once
#include "driver/ledc.h"
#include "driver/gpio.h"

typedef struct {
    gpio_num_t pwm_pin_id;    // PWM pin (e.g., GPIO_NUM_1)
    gpio_num_t dir_pin_id;    // Direction pin (e.g., GPIO_NUM_2)
    ledc_channel_t pwm_channel_num; // LEDC channel (e.g., LEDC_CHANNEL_0)
    ledc_timer_t pwm_timer_num;     // LEDC timer (e.g., LEDC_TIMER_0)
} motor_config_t;

// Initialize motor driver
void motor_init(const motor_config_t *config);

// Set motor speed and direction
void motor_set_speed(int speed_percent); // Speed: -100% (reverse) to +100% (forward)
void motor_set_direction(bool forward);  // True = forward, False = reverse
