#pragma once
#include "driver/ledc.h"
#include "driver/gpio.h"

typedef struct {
    // 方向控制引脚
    gpio_num_t dir_pin;
    // PWM控制参数
    ledc_channel_t pwm_channel;
    ledc_timer_t pwm_timer;
    // 电机配置校验
    uint8_t initialized;
} MotorConfig;

/**
 * @brief 初始化电机驱动
 * @param config 包含PWM通道、定时器和方向引脚的配置结构体
 * @param pwm_freq_hz PWM频率（推荐10-20kHz）
 */
void motor_init(MotorConfig *config, uint32_t pwm_freq_hz);

/**
 * @brief 设置电机速度和方向
 * @param config 已初始化的电机配置
 * @param duty 占空比（0~100%）
 * @param direction 方向（0=正向，1=反向）
 */
void motor_set_speed(MotorConfig *config, float duty, uint8_t direction);
