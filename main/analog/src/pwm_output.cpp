#include "pwm_output.h"

PwmOutput::PwmOutput(uint8_t pin, ledc_channel_t ledcChannel,ledc_timer_t timer, ledc_timer_bit_t dutyRes,
                     uint32_t freq): ledcChannel(ledcChannel) {
    ledc_timer_config_t ledc_timer = {
            LEDC_LOW_SPEED_MODE,
            dutyRes,
            timer,
            freq,
            LEDC_AUTO_CLK,
            false
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));
    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel = {
            pin,
            LEDC_LOW_SPEED_MODE,
            ledcChannel,
            LEDC_INTR_DISABLE,
            timer,
            0,
            0,
            LEDC_SLEEP_MODE_NO_ALIVE_NO_PD,
            {1}
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
}

void PwmOutput::set(uint16_t value) {
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, ledcChannel, value));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, ledcChannel));
}

