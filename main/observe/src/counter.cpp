#include "counter.h"

Counter::Counter(IMPULSE dir, uint32_t startOut): RFimpulse(dir), out(startOut){
}
void Counter::add(bool value){
	RFimpulse::set(value);
	if(RFimpulse::get()){
		out++;
	}
}
void Counter::sub(bool value){
	RFimpulse::set(value);
	if(RFimpulse::get()){
		out--;
	}
}
uint32_t Counter::get(){
	return out;
}
Counter& Counter::operator+=(bool value){
	add(value);
	return *this;
}
Counter& Counter::operator-=(bool value){
	sub(value);
	return *this;
}

uint32_t* Counter::getOutRef(){
	return &out; 
}
