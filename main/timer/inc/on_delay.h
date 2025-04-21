#ifndef ON_DELAY_H
#define ON_DELAY_H
#include <cstdint>
#include "common_timer.h"
#include "updater.h"

class OnDelayCommon: public ITimer {
	public:
        using CommonTimer::getCurrentTime;
		explicit OnDelayCommon(uint32_t period);
		void update() override;
		void set(bool value) override;
		bool get() override;
        void wait(bool value) override;
        void again() override;
};

class OnDelay: public OnDelayCommon, public IUpdated1ms {
	public:
		explicit OnDelay(uint32_t period);
		void update1ms() override;
        using ITimer::operator=;
        using ITimer::operator+=;
};

#endif //ON_DELAY_H