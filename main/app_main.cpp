#include <esp_log.h>
#include <esp_adc/adc_continuous.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "coil.h"
#include "simple_input.h"
#include "on_delay.h"
#include "pulse.h"
#include "off_delay.h"
#include "simple_input_delayed.h"
#include "task_common.h"

#include "wifi_ap_tcp_server.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

//WiFiApTcpServer tcpServer("hiwifi", "12345678", 3333);


extern "C" void app_main(void) {
    //initialization
    adc_continuous_handle_t adc_handle = NULL;

    adc_continuous_handle_cfg_t adc_config;
    adc_config.max_store_buf_size = 1024;
    adc_config.conv_frame_size = 256;
    ESP_ERROR_CHECK(adc_continuous_new_handle(&adc_config, &adc_handle));

    adc_continuous_config_t dig_cfg;
    dig_cfg.sample_freq_hz = 20 * 1000;
    dig_cfg.conv_mode = ADC_CONV_SINGLE_UNIT_1;
    dig_cfg.format = ADC_DIGI_OUTPUT_FORMAT_TYPE1;

    adc_digi_pattern_config_t adc_pattern;
    dig_cfg.pattern_num = 1;
    adc_pattern.atten = ADC_ATTEN_DB_12;
    adc_pattern.channel = 7 & 0x7;
    adc_pattern.unit = ADC_UNIT_1;
    adc_pattern.bit_width = ADC_BITWIDTH_12;

    dig_cfg.adc_pattern = &adc_pattern;
    ESP_ERROR_CHECK(adc_continuous_config(adc_handle, &dig_cfg));
    //start
    ESP_ERROR_CHECK(adc_continuous_start(adc_handle));

//    OnDelay onDelay(100);
//    Coil coil(2);
//    onDelay = true;
    esp_err_t ret;
    uint32_t ret_num = 0;
    uint8_t result[256] = {0};
    memset(result, 0xcc, 256);
    while(1){
        ret = adc_continuous_read(adc_handle, result, 256, &ret_num, 0);
        if (ret == ESP_OK) {
            uint32_t res = 0;
            uint16_t count = 0;
            for (int i = 0; i < ret_num; i += SOC_ADC_DIGI_RESULT_BYTES) {
                adc_digi_output_data_t *p = (adc_digi_output_data_t*)&result[i];
                uint32_t chan_num = p->type1.channel;
                uint32_t data = p->type1.data;
                /* Check the channel number validation, the data is invalid if the channel num exceed the maximum channel */
                res += data;
                count++;
            }
            if(count != 0){
                ESP_LOGI("ADC1", "continious measure: %lu", res / count);
            }
        }
//        if(onDelay.get()){
//            coil.toggle();
//            onDelay.again();
//        }
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}


//Oneshoot
//adc_oneshot_unit_handle_t adc1_handle;
//adc_oneshot_unit_init_cfg_t init_config;
//init_config.unit_id = ADC_UNIT_1;
//init_config.ulp_mode = ADC_ULP_MODE_DISABLE;
//ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &adc1_handle));
////-------------ADC1 Config---------------//
//adc_oneshot_chan_cfg_t config;
//config.bitwidth = ADC_BITWIDTH_DEFAULT;
//config.atten = ADC_ATTEN_DB_12;
//ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_7, &config));
//
////Calibration
//adc_cali_handle_t adc1_cali_chan0_handle = nullptr;
//adc_calibration_init(ADC_UNIT_1, ADC_CHANNEL_7, ADC_ATTEN_DB_12, &adc1_cali_chan0_handle);
//
//int result = 0;
//int voltage = 0;
//while(1){
//adc_oneshot_read(adc1_handle, ADC_CHANNEL_7, &result);
//adc_cali_raw_to_voltage(adc1_cali_chan0_handle, result, &voltage);
//ESP_LOGI("ADC1", "raw measure: %d, voltage: %d", result, voltage);
//}

static bool adc_calibration_init(adc_unit_t unit, adc_channel_t channel, adc_atten_t atten, adc_cali_handle_t *out_handle)
{
    const char* TAG = "ADC1 calib";
    adc_cali_handle_t handle = NULL;
    esp_err_t ret = ESP_FAIL;
    bool calibrated = false;

    ESP_LOGI(TAG, "calibration scheme version is %s", "Line Fitting");
    adc_cali_line_fitting_config_t cali_config;
    cali_config.unit_id = unit;
    cali_config.atten = atten;
    cali_config.bitwidth = ADC_BITWIDTH_DEFAULT;
    ret = adc_cali_create_scheme_line_fitting(&cali_config, &handle);
    if (ret == ESP_OK) {
        calibrated = true;
    }

    *out_handle = handle;
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "Calibration Success");
    } else if (ret == ESP_ERR_NOT_SUPPORTED || !calibrated) {
        ESP_LOGW(TAG, "eFuse not burnt, skip software calibration");
    } else {
        ESP_LOGE(TAG, "Invalid arg or no memory");
    }

    return calibrated;
}
