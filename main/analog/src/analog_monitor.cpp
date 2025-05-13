#include "analog_monitor.h"

AnalogMonitor::AnalogMonitor(adc_unit_t adcUnit, adc_channel_t channel,float valueMin, float valueMax,
                             adc_bitwidth_t bitWidth, adc_atten_t atten):
                             AdcReader(adcUnit, channel, bitWidth, atten),
                             Monitor(valueMin, valueMax),
                             avg(100){
    Updater::addObj(this);
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

AnalogMonitor::~AnalogMonitor(){
	delete analogScale;
}

void AnalogMonitor::update1ms() {
    analogScale->set(AdcReader::getDigitsWithUpdate());
    avg.set(analogScale->get());
    Monitor::set(avg.get());
    Monitor::update1ms();
}

