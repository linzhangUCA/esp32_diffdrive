#include "encoder.h"

static const char *TAG = "Encoder";

esp_err_t encoder_init(const encoder_config_t *config) {
    // 1. Configure PCNT unit
    ESP_LOGD(TAG, "install pcnt unit");
    pcnt_unit_config_t unit_config = {
        .high_limit = 32767,
        .low_limit = -32767,
        .accum_count = true,  // enable counter accumulation
    };
    pcnt_unit_handle_t pcnt_unit = NULL;
    ESP_ERROR_CHECK(pcnt_new_unit(&unit_config, &pcnt_unit));

    // 2. COnfigure glitch
    ESP_LOGD(TAG, "set glitch filter");
    pcnt_glitch_filter_config_t filter_config = {
        .max_glitch_ns = 1000,
    };
    ESP_ERROR_CHECK(pcnt_unit_set_glitch_filter(pcnt_unit, &filter_config));

    // 3. Configure PCNT channels
    ESP_LOGD(TAG, "install pcnt channels");
    pcnt_chan_config_t chan_a_config = {
        .edge_gpio_num = config->enc_a_pin,
        .level_gpio_num = config->enc_b_pin,
    };
    pcnt_channel_handle_t pcnt_chan_a = NULL;
    ESP_ERROR_CHECK(pcnt_new_channel(pcnt_unit, &chan_a_config, &pcnt_chan_a));
    pcnt_chan_config_t chan_b_config = {
        .edge_gpio_num = config->enc_b_pin,
        .level_gpio_num = config->enc_a_pin,
    };
    pcnt_channel_handle_t pcnt_chan_b = NULL;
    ESP_ERROR_CHECK(pcnt_new_channel(pcnt_unit, &chan_b_config, &pcnt_chan_b));

    // Set edge/level actions for quadrature encoding
    ESP_LOGI(TAG, "set edge and level actions for pcnt channels");
    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_chan_a, PCNT_CHANNEL_EDGE_ACTION_DECREASE, PCNT_CHANNEL_EDGE_ACTION_INCREASE));
    ESP_ERROR_CHECK(pcnt_channel_set_level_action(pcnt_chan_a, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE));
    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_chan_b, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_DECREASE));
    ESP_ERROR_CHECK(pcnt_channel_set_level_action(pcnt_chan_b, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE));

    // 3. Enable and start the PCNT unit
    ret = pcnt_unit_enable(pcnt_unit);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to enable PCNT unit: %s", esp_err_to_name(ret));
        return ret;
    }
    ret = pcnt_unit_start(pcnt_unit);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start PCNT unit: %s", esp_err_to_name(ret));
        return ret;
    }

    // Save the PCNT unit handle in the config
    config->pcnt_unit = pcnt_unit;

    ESP_LOGI(TAG, "Encoder initialized");
}

esp_err_t encoder_get_count(pcnt_unit_handle_t pcnt_unit, int *count) {
    return pcnt_unit_get_count(pcnt_unit, count);
}

float encoder_get_velocity(pcnt_unit_handle_t pcnt_unit, float delta_time_sec) {
    static int prev_count = 0;
    int curr_count;
    esp_err_t ret = encoder_get_count(pcnt_unit, &curr_count);
    if (ret != ESP_OK) return 0.0;

    int delta_count = curr_count - prev_count;
    prev_count = curr_count;

    float counts_per_rev = 4096.0; // 1024 PPR * 4 (quadrature)
    float rad_per_count = (2 * M_PI) / counts_per_rev;
    return (delta_count * rad_per_count) / delta_time_sec;
}
