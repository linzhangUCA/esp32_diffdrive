#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "encoder.h"

static const char *TAG = "Encoder";

void app_main(void) {
  // Configure encoder
  encoder_config_t enc_cfg = {
      .enc_a_pin = GPIO_NUM_4,
      .enc_b_pin = GPIO_NUM_5,
      // .pcnt_unit = NULL,
      .counts_per_rev = 48,
  };

  pcnt_unit_handle_t encoder;
  int i;
  int pulse_count = 0;

  // Initialize encoder
  encoder=encoder_init(&enc_cfg);

  // Read encoder counts
  for (i = 0; i < 200; i++) {
    pcnt_unit_get_count(encoder, &pulse_count);
    ESP_LOGI(TAG, "Pulse count: %d", pulse_count);
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
  // int pulse_count;
  // ESP_ERROR_CHECK(pcnt_unit_get_count(pcnt_unit, &pulse_count));
  // ESP_LOGI(TAG, "Pulse count: %d", pulse_count);

}
