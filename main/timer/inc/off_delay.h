#ifndef OFF_DELAY_H
#define OFF_DELAY_H

#include <cstdint>
#include "common_timer.h"
#include "updater.h"

class OffDelayCommon: private CommonTimer, public ITimer{
	private:
		bool startFlag{};
	public:
        using CommonTimer::getCurrentTime;
		explicit OffDelayCommon(uint32_t period);
		void update() override;
		void set(bool value) override;
        void pause(bool value) override;
		bool get() override;
		void reset() override;
        void again() override;
};

class OffDelay: public OffDelayCommon, public IUpdated1ms {
	public:
		explicit OffDelay(uint32_t period);
		void update1ms() override;
};

#endif //OFF_DELAY_H