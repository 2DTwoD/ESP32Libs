#ifndef ANALOG_OUTPUT_PWM_H
#define ANALOG_OUTPUT_PWM_H

#include <cstdint>
#include <hal/ledc_types.h>
#include "scale.h"
#include "pwm_output.h"

class AnalogWriterPWM: private PwmOutput{
private:
    Scale<float, uint16_t> *scale = nullptr;
public:
    AnalogWriterPWM(uint8_t pin, ledc_channel_t ledcChannel, ledc_timer_t timer,
                    float inputMin = 0.0f, float inputMax = 100.0f,
                    ledc_timer_bit_t dutyRes = LEDC_TIMER_12_BIT, uint32_t freq = 4000);
    virtual ~AnalogWriterPWM();
    void set(float value);
    uint16_t get();
    AnalogWriterPWM& operator=(float value);
};

#endif //ANALOG_OUTPUT_PWM_H
