#ifndef ANALOG_OUTPUT_PWM_H
#define ANALOG_OUTPUT_PWM_H

#include "pwm_output.h"

#include <cstdint>
#include <hal/ledc_types.h>

#include "scale.h"
#include "ramp.h"

class AnalogWriterPWM: private PwmOutput{
private:
    Scale<float, uint16_t> *scale = nullptr;
public:
    AnalogWriterPWM(uint8_t pin, ledc_channel_t ledcChannel, ledc_timer_t timer,
                    float inputMin = 0.0f, float inputMax = 100.0f,
                    ledc_timer_bit_t dutyRes = LEDC_TIMER_12_BIT, uint32_t freq = 4000);
    virtual ~AnalogWriterPWM();

    virtual void set(float value);
    uint16_t get();
    AnalogWriterPWM& operator=(float value);
};

class AnalogWriterPwmWithRamp: private AnalogWriterPWM, RampCommon, public IUpdated1ms{
public:
    using AnalogWriterPWM::get;
    using AnalogWriterPWM::operator=;
    AnalogWriterPwmWithRamp(uint8_t pin, ledc_channel_t ledcChannel, ledc_timer_t timer,
                    float inputMin = 0.0f, float inputMax = 100.0f, uint32_t fullRangeTime = 3000,
                    ledc_timer_bit_t dutyRes = LEDC_TIMER_12_BIT, uint32_t freq = 4000);

    void update1ms() override;
    void set(float value) override;
};

#endif //ANALOG_OUTPUT_PWM_H
