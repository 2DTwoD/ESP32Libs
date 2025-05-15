#ifndef ANALOG_MONITOR_H
#define ANALOG_MONITOR_H

#include "esp_adc/adc_oneshot.h"
#include <esp_log.h>

#include <cstdint>
#include "common_timer.h"
#include "monitor.h"
#include "scale.h"
#include "analog_reader.h"
#include "moving_avg.h"

class AnalogMonitor: private AnalogReaderCommon, MonitorCommon, public IUpdated1ms {
	private:
		Scale<uint16_t, float> *analogScale{nullptr};
        MovAvg avg;
	public:
        using MonitorCommon::get;
        using MonitorCommon::isHighAlarm;
        using MonitorCommon::isHighWarn;
        using MonitorCommon::isLowWarn;
        using MonitorCommon::isLowAlarm;
        using MonitorCommon::setTresDelay;
        using MonitorCommon::setTreshold;
		AnalogMonitor(adc_unit_t adcUnit, adc_channel_t channel,float valueMin = 0.0f, float valueMax = 100.0f,
                      adc_bitwidth_t bitWidth = ADC_BITWIDTH_DEFAULT, adc_atten_t atten = ADC_ATTEN_DB_12,
                      uint8_t avgCap = 150);
        void update1ms() override;
};

#endif //ANALOG_MONITOR_H