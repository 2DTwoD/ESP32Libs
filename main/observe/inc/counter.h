#ifndef COUNTER_H
#define COUNTER_H

#include <cstdint>
#include "common.h"
#include "rf_impulse.h"

class Counter: private RFimpulse{
	private:
		uint32_t out;
	public:
		Counter(IMPULSE dir, uint32_t startOut = 0);
		void add(bool value);
		void sub(bool value);
		uint32_t get();
		Counter& operator+=(bool value);
		Counter& operator-=(bool value);
		uint32_t* getOutRef();
};

#endif //COUNTER_H