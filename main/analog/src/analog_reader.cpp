#include "analog_reader.h"

AnalogReaderCommon::AnalogReaderCommon(adc_unit_t adcUnit, adc_channel_t channel, float valueMin, float valueMax,
                           adc_bitwidth_t bitWidth, adc_atten_t atten, uint8_t avgCap):
                           AdcReader(adcUnit, channel, bitWidth, atten),
                           avg(avgCap) {
    AdcReader::initWithCalibrate();
    int capacity = bitWidth;
    if(bitWidth == ADC_BITWIDTH_DEFAULT){
        capacity = SOC_ADC_DIGI_MAX_BITWIDTH;
    }
    uint16_t adcMax = 0;
    for(int i = 0; i < capacity; i++){
        adcMax |= 1 << i;
    }
    analogScale = new Scale((uint16_t)0, adcMax, valueMin, valueMax);
}

AnalogReaderCommon::~AnalogReaderCommon() {
    delete analogScale;
}

void AnalogReaderCommon::update() {
    analogScale->set(AdcReader::getDigitsWithUpdate());
    avg.set(analogScale->get());
}

float AnalogReaderCommon::get() {
    return avg.get();
}

AnalogReader::AnalogReader(adc_unit_t adcUnit, adc_channel_t channel, float valueMin, float valueMax,
                           adc_bitwidth_t bitWidth, adc_atten_t atten) :
                           AnalogReaderCommon(adcUnit, channel, valueMin, valueMax, bitWidth, atten) {
}

void AnalogReader::update1ms() {
    AnalogReaderCommon::update();
}