#ifndef ON_DELAY_H
#define ON_DELAY_H
#include <cstdint>
#include "common_timer.h"
#include "updater.h"

class OnDelayCommon: private CommonTimer, public ITimer {
	public:
        using CommonTimer::getCurrentTime;
		explicit OnDelayCommon(uint32_t period);
		void update() override;
		void set(bool value) override;
		void pause(bool value) override;
		bool get() override;
		void reset() override;
        void again() override;
};

class OnDelay: public OnDelayCommon, public IUpdated1ms {
	public:
		explicit OnDelay(uint32_t period);
		void update1ms() override;
};

#endif //ON_DELAY_H