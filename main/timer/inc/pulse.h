#ifndef PULSE_H
#define PULSE_H

#include <cstdint>
#include "common_timer.h"
#include "updater.h"

class PulseCommon: public ITimer{
	private:
		bool startFlag{false};
		bool waitFlag{false};
	public:
		explicit PulseCommon(uint32_t period);
		void update() override;
		void set(bool value) override;
		void wait(bool value) override;
		bool get() override;
        void again() override;
};

class Pulse: public PulseCommon, public IUpdated1ms {
	public:
		explicit Pulse(uint32_t period);
		void update1ms() override;
        using ITimer::operator=;
        using ITimer::operator+=;
};

class PulseInterrapt: public PulseCommon, public IUpdated1ms {
	public:
		explicit PulseInterrapt(uint32_t period);
		void update1ms() override;
		void set(bool value) override;
        using ITimer::operator=;
        using ITimer::operator+=;
};

#endif //PULSE_H