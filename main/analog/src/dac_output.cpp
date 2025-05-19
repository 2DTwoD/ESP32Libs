#include "dac_output.h"

DacOutput::DacOutput(dac_channel_t channel) {
    dac_oneshot_config_t dacConfig = {
            channel
    };
    ESP_ERROR_CHECK(dac_oneshot_new_channel(&dacConfig, &dacHandle));
}

void DacOutput::set(uint8_t value) {
    if(dacHandle == nullptr) return;
    ESP_ERROR_CHECK(dac_oneshot_output_voltage(dacHandle, value));
}
