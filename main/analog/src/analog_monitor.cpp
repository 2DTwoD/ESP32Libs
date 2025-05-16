#include "analog_monitor.h"

AnalogMonitor::AnalogMonitor(adc_unit_t adcUnit, adc_channel_t channel,float valueMin, float valueMax,
                             adc_bitwidth_t bitWidth, adc_atten_t atten, uint8_t avgCap):
                             AnalogReaderCommon(adcUnit, channel, valueMin, valueMax, bitWidth, atten, avgCap),
                             MonitorCommon(valueMin, valueMax){
}

void AnalogMonitor::update1ms() {
    AnalogReaderCommon::update();
    MonitorCommon::set(AnalogReaderCommon::get());
    MonitorCommon::update();
}
