#ifndef MOVING_AVG_H
#define MOVING_AVG_H
#include <cstring>
#include <cstdint>
#include <cstdlib>

#include "math_fun.h"
#include "interfaces.h"

class MovAvg: public IUpdatedSomewhere{
	private:
		float *row = nullptr;
		uint8_t size;
		uint8_t pos;
		float in;
		float out;
	public:
		MovAvg(uint8_t size = 1);
		~MovAvg();
		void set(float value);
		float get();
		void updateSomewhere() override;
		MovAvg& operator=(float value);
		float *const getInRef();
		float *const getOutRef();
};

#endif //MOVING_AVG_H
