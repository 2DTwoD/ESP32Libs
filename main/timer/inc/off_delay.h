#ifndef OFF_DELAY_H
#define OFF_DELAY_H

#include <cstdint>
#include "common_timer.h"
#include "updater.h"

class OffDelayCommon: public ITimer{
	private:
		bool startFlag{};
	public:
        using CommonTimer::getCurrentTime;
		explicit OffDelayCommon(uint32_t period);
		void update() override;
		void set(bool value) override;
		bool get() override;
        void wait(bool value) override;
        void again() override;
};

class OffDelay: public OffDelayCommon, public IUpdated1ms {
	public:
		explicit OffDelay(uint32_t period);
		void update1ms() override;
        using ITimer::operator=;
        using ITimer::operator+=;
};

#endif //OFF_DELAY_H