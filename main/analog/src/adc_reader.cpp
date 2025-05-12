#include "adc_reader.h"

AdcReader::AdcReader(adc_unit_t adcUnit, adc_channel_t channel, adc_bitwidth_t bitWidth, adc_atten_t atten):
                                    adcUnit(adcUnit), channel(channel), bitWidth(bitWidth), atten(atten){
    adc_oneshot_unit_init_cfg_t init_config;
    init_config.unit_id = adcUnit;
    init_config.ulp_mode = ADC_ULP_MODE_DISABLE;
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &adcHandle));
    adc_oneshot_chan_cfg_t config;
    config.bitwidth = bitWidth;
    config.atten = atten;
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adcHandle, channel, &config));
    calibrate();
}

bool AdcReader::calibrate() {
    const char* TAG = "ADC1 cali";
    esp_err_t ret = ESP_FAIL;
    bool calibrated = false;

    ESP_LOGI(TAG, "calibration scheme version is %s", "Line Fitting");
    adc_cali_line_fitting_config_t cali_config;
    cali_config.unit_id = adcUnit;
    cali_config.atten = atten;
    cali_config.bitwidth = bitWidth;
    ret = adc_cali_create_scheme_line_fitting(&cali_config, &adcCaliHandle);
    if (ret == ESP_OK) {
        calibrated = true;
    }
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "Calibration Success");
    } else if (ret == ESP_ERR_NOT_SUPPORTED || !calibrated) {
        ESP_LOGW(TAG, "eFuse not burnt, skip software calibration");
    } else {
        ESP_LOGE(TAG, "Invalid arg or no memory");
    }
    return calibrated;
}

void AdcReader::updateSomewhere() {
    adc_oneshot_read(adcHandle, channel, &digits);
    adc_cali_raw_to_voltage(adcCaliHandle, digits, &voltage);
}

int AdcReader::getDigits() const {
    return digits;
}

int AdcReader::getMilliVolts() const {
    return voltage;
}

int AdcReader::getDigitsWithUpdate() {
    updateSomewhere();
    return getDigits();
}

int AdcReader::getMilliVoltsWithUpdate() {
    updateSomewhere();
    return getMilliVolts();
}

