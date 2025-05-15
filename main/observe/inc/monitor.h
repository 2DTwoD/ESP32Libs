#ifndef MONITOR_H
#define MONITOR_H

#include <cstdint>
#include "updater.h"
#include "common_timer.h"
#include "math_fun.h"
#include "array_fun.h"

enum TRES_TYPE {
	MON_LL = 0,
	MON_HL = 1,
	MON_LH = 2,
	MON_HH = 3
};

class MonitorCommon{
	private:
		float in{0.0};
		void setValueMax(float limit);
		void setValueMin(float limit);
        CommonTimer* tresDelays[4]{nullptr};
        float inLimits[2]{0.0f, 100.0f};
        float tresholds[4]{5.0f, 10.0f, 90.0f, 95.0f};
    protected:
        void update();
	public:
		explicit MonitorCommon(float valueMin = 0.0, float valueMax = 100.0);
		~MonitorCommon();
		void set(float value);
        virtual float get();
		void setTreshold(TRES_TYPE tresType, uint16_t value);
		void setTresDelay(TRES_TYPE tresType, uint16_t del);
		bool isHighAlarm();
		bool isHighWarn();
		bool isLowWarn();
		bool isLowAlarm();
		MonitorCommon& operator=(float value);
};

class Monitor: public MonitorCommon, IUpdated1ms{
public:
    explicit Monitor(float valueMin = 0.0, float valueMax = 100.0);
    void update1ms() override;
};

#endif //MONITOR_H