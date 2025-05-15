#ifndef PWM_OUTPUT_H
#define PWM_OUTPUT_H

#include <hal/ledc_types.h>
#include "ledc.h"
#include "esp_err.h"

class PwmOutput{
private:
    ledc_channel_t ledcChannel;
public:
    PwmOutput(uint8_t pin, ledc_channel_t ledcChannel, ledc_timer_t timer,
              ledc_timer_bit_t dutyRes = LEDC_TIMER_12_BIT, uint32_t freq = 4000);
    void set(uint16_t value);
};

#endif //PWM_OUTPUT_H
