#include "analog_monitor.h"
//#include "soc_caps.h"

AnalogMonitor::AnalogMonitor(adc_unit_t adcUnit, adc_channel_t channel,float valueMin, float valueMax,
                             adc_bitwidth_t bitWidth, adc_atten_t atten):
                             Monitor(valueMin, valueMax),
                             AdcReader(adcUnit, channel, bitWidth, atten){
    int capacity = bitWidth;
//    if(bitWidth == ADC_BITWIDTH_DEFAULT){
//        capacity = SOC_ADC_DIGI_MAX_BITWIDTH;
//    }
	uint16_t adcMax = 0;
	for(int i = 0; i < capacity; i++){
		adcMax |= 1 << i;
	}
	analogScale = new Scale((uint16_t)0, adcMax, valueMin, valueMax);
}

AnalogMonitor::~AnalogMonitor(){
	delete analogScale;
}

void AnalogMonitor::set(uint16_t value) {
	*analogScale = value;
	Monitor::set(analogScale->get());
}

AnalogMonitor& AnalogMonitor::operator=(uint16_t value){
	set(value);
	return *this;
}