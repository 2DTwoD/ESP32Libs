#ifndef MOVING_AVG_H
#define MOVING_AVG_H
#include <cstring>
#include <cstdint>
#include <cstdlib>

#include "math_fun.h"
#include "updater.h"

class MovAvg{
	private:
		float *row = nullptr;
		uint8_t size;
		uint8_t pos;
		float in;
		float out;
        void update();
	public:
		MovAvg(uint8_t size = 1);
		~MovAvg();
		void set(float value);
		float get();

		MovAvg& operator=(float value);
};

#endif //MOVING_AVG_H
