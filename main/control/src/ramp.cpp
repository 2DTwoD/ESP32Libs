#include "ramp.h"

RampCommon::RampCommon(uint32_t fullRangeTime, float outMin, float outMax, float limitMin, float limitMax){
	if(outMin > outMax){
		toggle(&outMin, &outMax);
	}
	if(limitMin > limitMax){
		toggle(&limitMin, &limitMax);
	}
	outRange[0] = outMin;
	outRange[1] = outMax;
	outLimits[0] = limitMin;
	setLimMax(limitMax);
	setFullRangeTime(fullRangeTime);
}
RampCommon::RampCommon(uint32_t fullRangeTime, float outMin, float outMax): RampCommon(fullRangeTime, outMin, outMax, outMin, outMax){
}

RampCommon::RampCommon(uint32_t fullRangeTime): RampCommon(fullRangeTime, 0.0f, 100.0f, 0.0f, 100.0f){
}
float RampCommon::getOutMin(){
	return outRange[0];
}
float RampCommon::getOutRange(){
	return getRange(outRange);
}
void RampCommon::update(){
	if(step == 0){
		out = sp;
	} else {
		if(out < sp){
			out += step;
			if (out > sp) out = sp;
		} else if(out > sp){
			out -= step;
			if (out < sp) out = sp;
		}
	}
	out = limit(out, outLimits[0], outLimits[1]);
}
float RampCommon::get() const{
	return out;
}
void RampCommon::set(float value){
	sp = limit(value, outRange[0], outRange[1]);
}
uint32_t RampCommon::getFullRangeTime() const{
	return fullRangeTime;
}
void RampCommon::setFullRangeTime(uint32_t value){
	if(value > 0){
		step = getOutRange() / value;
	} else {
		step = 0;
	}
	this->fullRangeTime = value;
}
void RampCommon::setLimMin(float value){
	outLimits[0] = min(value, outLimits[1]);
}
void RampCommon::setLimMax(float value){
	outLimits[1] = max(value, outLimits[0]);
}
RampCommon& RampCommon::operator=(float value){
	set(value);
	return *this;
}

void Ramp::update1ms() {
    RampCommon::update();
}
