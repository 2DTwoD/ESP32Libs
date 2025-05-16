#include "adc_reader.h"

AdcReader::AdcReader(adc_unit_t adcUnit, adc_channel_t channel, adc_bitwidth_t bitWidth, adc_atten_t atten):
                    adcUnit(adcUnit), channel(channel), bitWidth(bitWidth), atten(atten){
    initWithCalibrate();
}

void AdcReader::init() {
    if(adcHandle != nullptr) return;
    adc_oneshot_unit_init_cfg_t init_config = {
            adcUnit,
            ADC_RTC_CLK_SRC_DEFAULT,
            ADC_ULP_MODE_DISABLE
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &adcHandle));

    adc_oneshot_chan_cfg_t config = {
            atten,
            bitWidth
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adcHandle, channel, &config));
}

bool AdcReader::calibrate() {
    const char* TAG = "ADC1 cali";
    esp_err_t ret = ESP_FAIL;
    bool calibrated = false;

    ESP_LOGI(TAG, "calibration scheme version is %s", "Line Fitting");
    adc_cali_line_fitting_config_t cali_config = {
            adcUnit,
            atten,
            bitWidth,
            0
    };
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

bool AdcReader::initWithCalibrate() {
    init();
    return calibrate();
}

bool AdcReader::updateDigits() {
    return adc_oneshot_read(adcHandle, channel, &digits) == ESP_OK;
}

bool AdcReader::updateVoltage() {
    if(adcCaliHandle == nullptr) return false;
    return adc_cali_raw_to_voltage(adcCaliHandle, digits, &voltage) == ESP_OK;
}

int AdcReader::getDigits() const {
    return digits;
}

int AdcReader::getMilliVolts() const {
    return voltage;
}

int AdcReader::getDigitsWithUpdate() {
    if(!updateDigits()){
        ESP_LOGI(TAG, "updateDigits error(unit ADC%d, channel %d)", adcUnit + 1, channel);
    }
    return getDigits();
}

int AdcReader::getMilliVoltsWithUpdate() {
    if(!updateVoltage()){
        ESP_LOGI(TAG, "updateVoltage error(unit ADC%d, channel %d)", adcUnit + 1, channel);
    }
    return getMilliVolts();
}

bool AdcReader::updateAll() {
    return updateDigits() && updateVoltage();
}

