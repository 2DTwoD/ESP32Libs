#ifndef PID_H
#define PID_H

#include <cstdint>

#include "updater.h"
#include "math_fun.h"

class PIDreg: public IUpdatedSomewhere{
	private:
		float pv{0.0f};
		uint16_t t{20};
	
		float sp{0.0f};
		float kp{1.0f};
		float ti{10.0f};
		float td{0.0f};
		float db{0.0f};
		float upOutLim{100.0f};
		float downOutLim{0.0f};
		bool AUTO{false};
		bool inverse{false};
	
		float out{0.0f};
		float e0{0.0f};
		float e1{0.0f};
		float e2{0.0f};
		float g0{0.0f};
		float g1{0.0f};
		float g2{0.0f};
	public:

    PIDreg(uint16_t t, float sp, float kp = 1.0f,
               float ti = 10.0f, float td = 0.0f, float db = 0.0f,
               float upOutLim = 100.0f, float downOutLim = 0.0f, bool inverse = false);
		uint16_t getT() const;
		void setPv(float);
		float getPv() const;
		void setSp(float);
		float getSp() const;
		void setKp(float);
		float getKp() const;
		void setTi(float);
		float getTi() const;
		void setTd(float);
		float getTd() const;
		void setDb(float);
		float getDb() const;
		void setUpOutLim(float);
		float getUpOutLim() const;
		void setDownOutLim(float);
		float getDownOutLim() const;
		void setOut(float);
		float getOut() const;
		void setAuto(bool);
		bool isAUTO() const;
		void setInverse(bool);
		bool isInverse() const;
		void updateSomewhere() override;
		void reset();
		void updateKoef();
		PIDreg& operator=(float value);
};

#endif //PID_H
