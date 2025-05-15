#ifndef ANALOG_READER_H
#define ANALOG_READER_H

#include "updater.h"
#include "adc_reader.h"
#include "moving_avg.h"
#include "scale.h"

class AnalogReaderCommon: private AdcReader {
private:
    Scale<uint16_t, float> *analogScale{nullptr};
    MovAvg avg;
protected:
    void update();
public:
    AnalogReaderCommon(adc_unit_t adcUnit, adc_channel_t channel, float valueMin = 0.0f, float valueMax = 100.0f,
                  adc_bitwidth_t bitWidth = ADC_BITWIDTH_DEFAULT, adc_atten_t atten = ADC_ATTEN_DB_12, uint8_t avgCap = 150);
    virtual ~AnalogReaderCommon();
    float get();
};

class AnalogReader: public AnalogReaderCommon, IUpdated1ms {
public:
    using AnalogReaderCommon::get;
    AnalogReader(adc_unit_t adcUnit, adc_channel_t channel, float valueMin = 0.0f, float valueMax = 100.0f,
                       adc_bitwidth_t bitWidth = ADC_BITWIDTH_DEFAULT, adc_atten_t atten = ADC_ATTEN_DB_12);
    void update1ms() override;
};


#endif //ANALOG_READER_H
