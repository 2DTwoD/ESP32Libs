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
    PwmOutput::set(AnalogWriterPWM::get());
}

uint16_t AnalogWriterPWM::get() {
    return scale->get();
}

AnalogWriterPWM &AnalogWriterPWM::operator=(float value) {
    AnalogWriterPWM::set(value);
    return *this;
}
