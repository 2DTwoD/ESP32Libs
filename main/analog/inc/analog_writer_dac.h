#ifndef ANALOG_WRITER_DAC_H
#define ANALOG_WRITER_DAC_H

#include "dac_output.h"
#include "scale.h"
#include "ramp.h"

class AnalogWriterDAC: private DacOutput{
private:
    Scale<float, uint8_t> scale;
public:
    explicit AnalogWriterDAC(dac_channel_t channel, float inputMin = 0.0f, float inputMax = 100.0f);

    virtual void set(float value);
    uint8_t get();
    AnalogWriterDAC& operator=(float value);
};

class AnalogWriterDacWithRamp: private AnalogWriterDAC, RampCommon, public IUpdated1ms{
public:
    using AnalogWriterDAC::get;
    using AnalogWriterDAC::operator=;
    explicit AnalogWriterDacWithRamp(dac_channel_t channel, float inputMin = 0.0f, float inputMax = 100.0f,
                            uint32_t fullRangeTime = 3000);

    void update1ms() override;
    void set(float value) override;
};


#endif //ANALOG_WRITER_DAC_H
