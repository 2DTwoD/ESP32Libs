#ifndef ANALOG_MONITOR_H
#define ANALOG_MONITOR_H

#include "esp_adc/adc_oneshot.h"
#include <esp_log.h>

#include <cstdint>
#include "common_timer.h"
#include "monitor.h"
#include "scale.h"
#include "adc_reader.h"

class AnalogMonitor: public Monitor, AdcReader{
	private:
		Scale<uint16_t, float> *analogScale{nullptr};
	public:
		AnalogMonitor(adc_unit_t adcUnit, adc_channel_t channel,float valueMin = 0.0f, float valueMax = 100.0f,
                      adc_bitwidth_t bitWidth = ADC_BITWIDTH_DEFAULT, adc_atten_t atten = ADC_ATTEN_DB_12);
		~AnalogMonitor();
		void set(uint16_t value);
		AnalogMonitor& operator=(uint16_t value);
};

#endif //ANALOG_MONITOR_H