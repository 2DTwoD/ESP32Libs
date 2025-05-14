#ifndef SCALE_H
#define SCALE_H

#include "updater.h"
#include "math_fun.h"

template<typename T, typename R>
class Scale{
	private:
		T in;
		T minIn;
		T maxIn;
		R out;
		R minOut;
		R maxOut;
        void update(){
            out = (maxOut - minOut) * (in - minIn) / (maxIn - minIn) + minOut;
        }
	public:
		Scale(T minIn, T maxIn, R minOut, R maxOut): minIn(minIn), maxIn(maxIn), minOut(minOut), maxOut(maxOut){
		}
        virtual ~Scale()= default;
		void set(T value){
			in = limit(value, minIn, maxIn);
			update();
		}
		R get(){
			return out;
		}

		Scale<T, R>& operator=(T value){
			set(value);
			return *this;
		}
};

#endif //SCALE_H
