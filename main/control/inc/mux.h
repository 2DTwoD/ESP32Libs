#ifndef MUX_H
#define MUX_H

#include <cstdint>
#include <cstdarg>

#include "math_fun.h"

template<typename T>
class Mux{
	private:
		uint8_t capacity{};
		uint8_t channel{};
		T** inArray;
	public:
		explicit Mux(uint8_t capacity){
			this->capacity = max((uint8_t)1, capacity);
			inArray = new T*[capacity];
		}
		~Mux(){
			delete[] inArray;
		}
		T get(){
			return *inArray[channel];
		}
		void set(uint8_t channel){
			this->channel = min((uint8_t)(capacity - 1), channel);
		}
		void reg(T *const pointer, uint8_t channel){
			if(channel > capacity - 1){
				return;
			}
			inArray[channel] = pointer;
		}
		void reg(int n, ...){
			n = min((int) capacity, n);
			va_list factor;
			va_start(factor, n);
			for(int i = 0; i < n; i++){
				inArray[i] = va_arg(factor, T*);
			}
			va_end(factor);
		}
};
#endif //MUX_H