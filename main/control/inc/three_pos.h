#ifndef THREE_POS_H
#define THREE_POS_H

#include <cstdint>

#include "math_fun.h"
#include "updater.h"
#include "common_timer.h"

class ThreePosReg: public IUpdated1ms{
	private:
		float in;
		float sp;
		float zeroGist;
		float pulseGist;
		CommonTimer *pauseTimer = nullptr;
		CommonTimer *pulseTimer = nullptr;
		uint8_t out1Mode;
		uint8_t out2Mode;
		bool reverse;
		bool out1;
		bool out2;
		bool startTimer;
		bool direction;
		bool impulseCycleEnded();
		bool halfImpulseCycleEnded();
	public:
		ThreePosReg(float sp = 50.0f, float zeroGist = 5.0f, float pulseGist = 15.0f, uint16_t pauseTime = 1000, uint16_t pulseTime = 1000, bool reverse = false);
		~ThreePosReg();
		void set(float value);
		ThreePosReg& operator=(float value);
		void setSp(float value);
		float getSp();
		void setZeroGist(float value);
		float getZeroGist();
		void setPulseGist(float value);
		float getPulseGist();
		void setPauseTime(uint16_t value);
		uint16_t getPauseTime();
		void setPulseTime(uint16_t value);
		uint16_t getPulseTime();
		void setReverse(bool value);
		bool getReverse();
		bool getOut1();
		bool getOut2();
		void update1ms() override;
};

#endif //THREE_POS_H
