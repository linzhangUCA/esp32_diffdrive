#include "encoder.h"
#include "esp_check.h"
#include "esp_err.h"
#include "esp_log.h"
#include <math.h>

static const char *TAG = "Encoder";

esp_err_t enable_encoder(encoder_config_t *encoder_cfg) {
  // 1. Configure PCNT unit
  pcnt_unit_config_t unit_config = {
      .high_limit = 32767,
      .low_limit = -32767,
      .flags.accum_count = true, // enable counter accumulation
  };
  pcnt_unit_handle_t pcnt_unit = NULL;
  ESP_RETURN_ON_ERROR(pcnt_new_unit(&unit_config, &pcnt_unit), TAG,
                      "configure new pulse counter unit failed");

  // 2. COnfigure glitch
  pcnt_glitch_filter_config_t filter_config = {
      .max_glitch_ns = 1000,
  };
  ESP_RETURN_ON_ERROR(pcnt_unit_set_glitch_filter(pcnt_unit, &filter_config),
                      TAG, "configure pcnt unit glitch filter failed");

  // 3. Configure PCNT channels
  pcnt_chan_config_t chan_a_config = {
      .edge_gpio_num = encoder_cfg->enc_a_pin,
      .level_gpio_num = encoder_cfg->enc_b_pin,
  };
  pcnt_channel_handle_t pcnt_chan_a = NULL;
  ESP_RETURN_ON_ERROR(pcnt_new_channel(pcnt_unit, &chan_a_config, &pcnt_chan_a),
                      TAG, "install pcnt channel A failed");
  pcnt_chan_config_t chan_b_config = {
      .edge_gpio_num = encoder_cfg->enc_b_pin,
      .level_gpio_num = encoder_cfg->enc_a_pin,
  };
  pcnt_channel_handle_t pcnt_chan_b = NULL;
  ESP_RETURN_ON_ERROR(pcnt_new_channel(pcnt_unit, &chan_b_config, &pcnt_chan_b),
                      TAG, "install pcnt channel B failed");

  // 4. Set edge/level actions for quadrature encoding
  ESP_LOGI(TAG, "set edge and level actions for pcnt channels");
  ESP_RETURN_ON_ERROR(pcnt_channel_set_edge_action(
                          pcnt_chan_a, PCNT_CHANNEL_EDGE_ACTION_DECREASE,
                          PCNT_CHANNEL_EDGE_ACTION_INCREASE),
                      TAG, "set channel A edge action failed");
  ESP_RETURN_ON_ERROR(
      pcnt_channel_set_level_action(pcnt_chan_a, PCNT_CHANNEL_LEVEL_ACTION_KEEP,
                                    PCNT_CHANNEL_LEVEL_ACTION_INVERSE),
      TAG, "set channel A level action failed");
  ESP_RETURN_ON_ERROR(pcnt_channel_set_edge_action(
                          pcnt_chan_b, PCNT_CHANNEL_EDGE_ACTION_INCREASE,
                          PCNT_CHANNEL_EDGE_ACTION_DECREASE),
                      TAG, "set channel B edge action failed");
  ESP_RETURN_ON_ERROR(
      pcnt_channel_set_level_action(pcnt_chan_b, PCNT_CHANNEL_LEVEL_ACTION_KEEP,
                                    PCNT_CHANNEL_LEVEL_ACTION_INVERSE),
      TAG, "set channel B level action failed");

  // Set watch point
  ESP_RETURN_ON_ERROR(
      pcnt_unit_add_watch_point(pcnt_unit, unit_config.high_limit), TAG,
      "add high limit watch point failed");
  ESP_RETURN_ON_ERROR(
      pcnt_unit_add_watch_point(pcnt_unit, unit_config.low_limit), TAG,
      "add low limit watch point failed");

  // Enable and start PCNT unit
  ESP_RETURN_ON_ERROR(pcnt_unit_enable(pcnt_unit), TAG,
                      "enable pcnt unit failed");
  ESP_RETURN_ON_ERROR(pcnt_unit_clear_count(pcnt_unit), TAG,
                      "clear pcnt counts failed");
  ESP_RETURN_ON_ERROR(pcnt_unit_start(pcnt_unit), TAG,
                      "start pcnt unit failed");

  // Save the PCNT unit handle in the config
  encoder_cfg->pcnt_unit = pcnt_unit;

  // return pcnt_unit;
  ESP_LOGI(TAG, "enabled");
  return ESP_OK;
}

// esp_err_t encoder_get_count(pcnt_unit_handle_t pcnt_unit, int *count) {
//   return pcnt_unit_get_count(pcnt_unit, count);
// }

float encoder_get_ang_vel(encoder_config_t *encoder_cfg, float dt) {
  static int prev_count = 0;
  int curr_count;
  pcnt_unit_get_count(encoder_cfg->pcnt_unit, &curr_count);

  int delta_count = curr_count - prev_count;
  prev_count = curr_count;

  float rad_per_count = (2 * M_PI) / encoder_cfg->counts_per_rev;
  return (delta_count * rad_per_count) / dt;
}
