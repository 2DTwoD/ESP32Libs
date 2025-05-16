#include "dac_output.h"

DacOutput::DacOutput() {
    dac_oneshot_handle_t dacHandle;
    dac_oneshot_config_t dacConfig = {
            DAC_CHAN_0
    };
    ESP_ERROR_CHECK(dac_oneshot_new_channel(&dacConfig, &dacHandle));
}

void DacOutput::set(uint16_t value) {
    ESP_ERROR_CHECK(dac_oneshot_output_voltage(handle, value));
}
