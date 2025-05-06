#include <esp_log.h>
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

extern "C" void app_main(void) {
//    tcpServer.start();
//    Coil coil(2);
//    SimpleInputDelayed simpleInput(13, 1000);
//    Pulse pulse(2000);
//    xTaskCreate(testTask, "testTask", configMINIMAL_STACK_SIZE * 20, nullptr,
//                tskIDLE_PRIORITY + 1, nullptr);


    adc_oneshot_unit_handle_t adc1_handle;
    adc_oneshot_unit_init_cfg_t init_config;
    init_config.unit_id = ADC_UNIT_1;
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &adc1_handle));
    //-------------ADC1 Config---------------//
    adc_oneshot_chan_cfg_t config;
    config.atten = ADC_ATTEN_DB_12;
    config.bitwidth = ADC_BITWIDTH_DEFAULT;
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_7, &config));

    //Calibration
    adc_cali_handle_t adc1_cali_chan0_handle = nullptr;
    adc_calibration_init(ADC_UNIT_1, ADC_CHANNEL_7, ADC_ATTEN_DB_12, &adc1_cali_chan0_handle);

    int result = 0;
    while(1){
        adc_oneshot_read(adc1_handle, ADC_CHANNEL_7, &result);
        ESP_LOGI("ADC1", "raw measure: %d", result);



//        pulse = simpleInput.isActive();
//        coil = pulse.get();
        vTaskDelay(200 / portTICK_PERIOD_MS);
    }
}
