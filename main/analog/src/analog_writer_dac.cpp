#include "analog_writer_dac.h"

AnalogWriterDAC::AnalogWriterDAC(dac_channel_t channel, float inputMin, float inputMax):
                                 DacOutput(channel), scale(inputMin, inputMax, 0, 255){

}

void AnalogWriterDAC::set(float value) {
    scale.set(value);
    DacOutput::set(scale.get());
}

uint8_t AnalogWriterDAC::get() {
    return scale.get();
}

AnalogWriterDAC &AnalogWriterDAC::operator=(float value) {
    set(value);
    return *this;
}

AnalogWriterDacWithRamp::AnalogWriterDacWithRamp(dac_channel_t channel, float inputMin, float inputMax,
                                                 uint32_t fullRangeTime):
                                                 AnalogWriterDAC(channel, inputMin, inputMax),
                                                 RampCommon(fullRangeTime, inputMin, inputMax) {
}

void AnalogWriterDacWithRamp::update1ms() {
    RampCommon::update();
    AnalogWriterDAC::set(RampCommon::get());
}

void AnalogWriterDacWithRamp::set(float value) {
    RampCommon::set(value);
}
