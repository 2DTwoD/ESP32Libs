#ifndef ADC_READER_H
#define ADC_READER_H

#include "updater.h"

#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include <esp_log.h>

class AdcReader: IUpdatedSomewhere{
private:
    adc_oneshot_unit_handle_t adcHandle{nullptr};
    adc_unit_t adcUnit;
    adc_channel_t channel;
    adc_bitwidth_t bitWidth;
    adc_atten_t atten;
    adc_cali_handle_t adcCaliHandle{nullptr};
    int digits;
    int voltage;
public:
    AdcReader(adc_unit_t adcUnit, adc_channel_t channel,
              adc_bitwidth_t bitWidth = ADC_BITWIDTH_DEFAULT, adc_atten_t atten = ADC_ATTEN_DB_12);
    bool calibrate();
    int getDigits() const;
    int getDigitsWithUpdate();
    int getMilliVolts() const;
    int getMilliVoltsWithUpdate();
    void updateSomewhere() override;
};

////Continuous measure
////initialization
//adc_continuous_handle_t adc_handle = NULL;
//
//adc_continuous_handle_cfg_t adc_config;
//adc_config.max_store_buf_size = 1024;
//adc_config.conv_frame_size = 256;
//ESP_ERROR_CHECK(adc_continuous_new_handle(&adc_config, &adc_handle));
//
//adc_continuous_config_t dig_cfg;
//dig_cfg.sample_freq_hz = 20 * 1000;
//dig_cfg.conv_mode = ADC_CONV_SINGLE_UNIT_1;
//dig_cfg.format = ADC_DIGI_OUTPUT_FORMAT_TYPE1;
//
//adc_digi_pattern_config_t adc_pattern;
//dig_cfg.pattern_num = 1;
//adc_pattern.atten = ADC_ATTEN_DB_12;
//adc_pattern.channel = 7 & 0x7;
//adc_pattern.unit = ADC_UNIT_1;
//adc_pattern.bit_width = ADC_BITWIDTH_12;
//
//dig_cfg.adc_pattern = &adc_pattern;
//ESP_ERROR_CHECK(adc_continuous_config(adc_handle, &dig_cfg));
////start
//ESP_ERROR_CHECK(adc_continuous_start(adc_handle));
//
//esp_err_t ret;
//uint32_t ret_num = 0;
//uint8_t result[256] = {0};
//memset(result, 0xcc, 256);
//while(1){
//ret = adc_continuous_read(adc_handle, result, 256, &ret_num, 0);
//if (ret == ESP_OK) {
//uint32_t res = 0;
//uint16_t count = 0;
//for (int i = 0; i < ret_num; i += SOC_ADC_DIGI_RESULT_BYTES) {
//adc_digi_output_data_t *p = (adc_digi_output_data_t*)&result[i];
//uint32_t chan_num = p->type1.channel;
//uint32_t data = p->type1.data;
///* Check the channel number validation, the data is invalid if the channel num exceed the maximum channel */
//res += data;
//count++;
//}
//if(count != 0){
//ESP_LOGI("ADC1", "continious measure: %lu", res / count);
//}
//}
//vTaskDelay(500 / portTICK_PERIOD_MS);
//}

#endif //ADC_READER_H