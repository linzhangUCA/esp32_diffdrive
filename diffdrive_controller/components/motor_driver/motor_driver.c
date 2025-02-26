#include "motor_driver.h"
#include "esp_log.h"

static const char *TAG = "MotorDriver";
static motor_config_t g_config;

void motor_init(const motor_config_t *config) {
    // Save configuration
    g_config = *config;

    // Configure DIR pin as output
    gpio_reset_pin(g_config.dir_pin);
    gpio_set_direction(g_config.dir_pin, GPIO_MODE_OUTPUT);

    // Configure PWM timer
    ledc_timer_config_t timer_cfg = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_10_BIT, // 10-bit resolution (0-1023)
        .timer_num = g_config.pwm_timer,
        .freq_hz = 20000, // 20 kHz (typical for motor drivers to avoid audible noise)
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ESP_ERROR_CHECK(ledc_timer_config(&timer_cfg));

    // Configure PWM channel
    ledc_channel_config_t pwm_cfg = {
        .gpio_num = g_config.pwm_pin,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = g_config.pwm_channel,
        .timer_sel = g_config.pwm_timer,
        .duty = 0,
        .hpoint = 0,
    };
    ESP_ERROR_CHECK(ledc_channel_config(&pwm_cfg));

    ESP_LOGI(TAG, "Motor driver initialized");
}

void motor_set_direction(bool forward) {
    gpio_set_level(g_config.dir_pin, forward ? 1 : 0);
}

void motor_set_speed(int speed_percent) {
    // Clamp speed to [-100, 100]
    if (speed_percent > 100) speed_percent = 100;
    if (speed_percent < -100) speed_percent = -100;

    // Set direction based on sign
    motor_set_direction(speed_percent >= 0);

    // Convert percentage to duty cycle (0-1023)
    uint32_t duty = (abs(speed_percent) * 1023) / 100;

    // Update PWM duty
    ledc_set_duty(LEDC_LOW_SPEED_MODE, g_config.pwm_channel, duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, g_config.pwm_channel);
}
