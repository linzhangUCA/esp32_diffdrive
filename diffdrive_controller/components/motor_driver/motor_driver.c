#include "motor_driver.h"
#include "esp_err.h"
#include "esp_log.h"

#define TAG "MOTOR_DRIVER"

void motor_init(MotorConfig *config, uint32_t pwm_freq_hz) {
    // 参数有效性检查
    ESP_ERROR_CHECK(gpio_reset_pin(config->dir_pin));
    ESP_ERROR_CHECK(gpio_set_direction(config->dir_pin, GPIO_MODE_OUTPUT));

    // LEDC定时器配置
    ledc_timer_config_t timer_conf = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_10_BIT, // 10位分辨率（0-1023）
        .timer_num = config->pwm_timer,
        .freq_hz = pwm_freq_hz,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&timer_conf));

    // LEDC通道配置
    ledc_channel_config_t ch_conf = {
        .gpio_num = config->pwm_pin,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = config->pwm_channel,
        .timer_sel = config->pwm_timer,
        .duty = 0,
        .hpoint = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ch_conf));

    config->initialized = 1;
    ESP_LOGI(TAG, "Motor initialized on GPIO%d", config->pwm_pin);
}

void motor_set_speed(MotorConfig *config, float duty, uint8_t direction) {
    if (!config->initialized) {
        ESP_LOGE(TAG, "Motor not initialized!");
        return;
    }

    // 限制占空比范围
    duty = (duty < 0) ? 0 : (duty > 100) ? 100 : duty;
    
    // 设置方向
    ESP_ERROR_CHECK(gpio_set_level(config->dir_pin, direction));
    
    // 计算实际占空比值（10位分辨率）
    uint32_t duty_val = (uint32_t)((duty / 100.0f) * (1 << 10));
    
    // 更新PWM占空比
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, 
                                config->pwm_channel, 
                                duty_val));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, 
                                   config->pwm_channel));
    
    ESP_LOGD(TAG, "Set duty: %.1f%%, Direction: %s", 
             duty, direction ? "REVERSE" : "FORWARD");
}
