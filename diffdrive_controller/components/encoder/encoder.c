#include "encoder.h"

static const char *TAG = "Encoder";

esp_err_t encoder_init(const encoder_config_t *config) {
    esp_err_t ret;

    // 1. Configure PCNT unit
    ESP_LOGI(TAG, "install pcnt unit");
    pcnt_unit_config_t unit_config = {
        .high_limit = 32767,
        .low_limit = -32767,
        .accum_count = true,  // enable counter accumulation
    };
    pcnt_unit_handle_t pcnt_unit = NULL;
    ret = pcnt_new_unit(&unit_config, &pcnt_unit);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create PCNT unit: %s", esp_err_to_name(ret));
        return ret;
    }

    // 2. COnfigure glitch
    ESP_LOGI(TAG, "set glitch filter");
    pcnt_glitch_filter_config_t filter_config = {
        .max_glitch_ns = 1000,
    };
    ret = pcnt_unit_set_glitch_filter(pcnt_unit, &filter_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set up glitch filter", esp_err_to_name(ret));
        return ret;
    }

    // 3. Configure PCNT channel (A phase)
    pcnt_chan_config_t chan_a_config = {
        .edge_gpio_num = config->enc_a_pin,
        .level_gpio_num = config->enc_b_pin,
    };
    pcnt_channel_handle_t pcnt_chan_a = NULL;
    ret = pcnt_new_channel(pcnt_unit, &chan_a_config, &pcnt_chan_a);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create PCNT channel A: %s", esp_err_to_name(ret));
        return ret;
    }

    // Set edge/level actions for quadrature encoding
    pcnt_channel_set_edge_action(pcnt_chan_a, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_HOLD);
    pcnt_channel_set_level_action(pcnt_chan_a, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE);

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
    return ESP_OK;
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
