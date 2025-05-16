#include "analog_writer_pwm.h"

AnalogWriterPWM::AnalogWriterPWM(uint8_t pin, ledc_channel_t ledcChannel, ledc_timer_t timer, float inputMin,
                                 float inputMax, ledc_timer_bit_t dutyRes, uint32_t freq):
                                 PwmOutput(pin, ledcChannel,timer, dutyRes, freq) {
    uint16_t capMax = 0;
    for(int i = 0; i < dutyRes; i++){
        capMax |= 1 << i;
    }
    scale = new Scale<float, uint16_t>(inputMin, inputMax, 0, capMax);
}

AnalogWriterPWM::~AnalogWriterPWM() {
    delete scale;
}

void AnalogWriterPWM::set(float value) {
    scale->set(value);
    PwmOutput::set(get());
}

uint16_t AnalogWriterPWM::get() {
    return scale->get();
}

AnalogWriterPWM &AnalogWriterPWM::operator=(float value) {
    set(value);
    return *this;
}

AnalogWriterPwmWithRamp::AnalogWriterPwmWithRamp(uint8_t pin, ledc_channel_t ledcChannel, ledc_timer_t timer,
                                                 float inputMin, float inputMax, uint32_t fullRangeTime,
                                                 ledc_timer_bit_t dutyRes, uint32_t freq):
                                                 AnalogWriterPWM(pin, ledcChannel, timer, inputMin,
                                                                 inputMax, dutyRes, freq),
                                                 RampCommon(fullRangeTime, inputMin, inputMax){

}

void AnalogWriterPwmWithRamp::update1ms() {
    RampCommon::update();
    AnalogWriterPWM::set(RampCommon::get());
}

void AnalogWriterPwmWithRamp::set(float value) {
    RampCommon::set(value);
}
