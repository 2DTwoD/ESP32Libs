#ifndef PULSE_H
#define PULSE_H

#include <cstdint>
#include "common_timer.h"
#include "updater.h"

class PulseCommon: private CommonTimer, public ITimer{
	private:
		bool startFlag{false};
	public:
        using CommonTimer::getCurrentTime;
		explicit PulseCommon(uint32_t period);
		void update() override;
		void set(bool value) override;
		void pause(bool value) override;
		bool get() override;
		void reset() override;
        void again() override;
};

class Pulse: public PulseCommon, public IUpdated1ms {
	public:
		explicit Pulse(uint32_t period);
		void update1ms() override;
};

class PulseInterrapt: public PulseCommon, public IUpdated1ms {
	public:
		explicit PulseInterrapt(uint32_t period);
		void update1ms() override;
		void set(bool value) override;
};

#endif //PULSE_H