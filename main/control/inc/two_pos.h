#ifndef TWO_POS_H
#define TWO_POS_H

#include <cstdint>

#include "math_fun.h"
#include "updater.h"
#include "common_timer.h"

class TwoPosCommon{
	protected:
		float in{0.0f};
		float sp{50.0f};
		bool reverse{false};
		bool out{false};
	public:
		TwoPosCommon(float sp = 50.0f, bool reverse = false);
		void set(float value);
		float get();
		void setSp(float value);
		float getSp();
		void setReverse(bool value);
		bool getReverse();
};

class TwoPosGist: public TwoPosCommon, public IUpdatedSomewhere{
	private:
		float lowGist{0.0f};
		float highGist{0.0f};
	public:
        explicit TwoPosGist(float sp = 50.0f, float lowGist = 0.0f, float highGist = 0.0f, bool reverse = false);
		void set(float value);
		void setLowGist(float value);
		float getLowGist();
		void setHighGist(float value);
		float getHighGist();
		void updateSomewhere() override;
		TwoPosGist& operator=(float value);
};

class TwoPosTim: public TwoPosCommon, public IUpdated1ms{
	private:
		CommonTimer *lowTimer{nullptr};
		CommonTimer *highTimer{nullptr};
	public:
		TwoPosTim(float sp = 50.0f, uint16_t lowTime = 0, uint16_t highTim = 0, bool reverse = false);
	    ~TwoPosTim();
		void setLowTime(uint16_t value);
		uint16_t getLowTime();
		void setHighTime(uint16_t value);
		uint16_t getHighTime();
		void update1ms() override;
		TwoPosTim& operator=(float value);
};

#endif //TWO_POS_H
