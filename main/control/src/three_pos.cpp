#include "three_pos.h"
ThreePosReg::ThreePosReg(float sp, float zeroGist, float pulseGist, uint16_t pauseTime, uint16_t pulseTime, bool reverse): sp(sp), zeroGist(zeroGist), reverse(reverse){
	pauseTimer = new CommonTimer(pauseTime);
	pulseTimer = new CommonTimer(pulseTime);
	setPulseGist(pulseGist);
}
ThreePosReg::~ThreePosReg(){
	delete pauseTimer;
	delete pulseTimer;
}
void ThreePosReg::set(float value){
	in = value;
}
ThreePosReg& ThreePosReg::operator=(float value){
	set(value);
	return *this;
}
void ThreePosReg::setSp(float value){
	sp = value;
}
float ThreePosReg::getSp(){
	return sp;
}
void ThreePosReg::setZeroGist(float value){
	zeroGist = min(abs(value), pulseGist);
}
float ThreePosReg::getZeroGist(){
	return zeroGist;
}
void ThreePosReg::setPulseGist(float value){
	pulseGist = max(abs(value), zeroGist);
}
float ThreePosReg::getPulseGist(){
	return pulseGist;
}
void ThreePosReg::setPauseTime(uint16_t value){
	pauseTimer->setPeriod(value);
}
uint16_t ThreePosReg::getPauseTime(){
	return pauseTimer->getPeriod();
}
void ThreePosReg::setPulseTime(uint16_t value){
	pulseTimer->setPeriod(value);
}
uint16_t ThreePosReg::getPulseTime(){
	return pulseTimer->getPeriod();
}
void ThreePosReg::setReverse(bool value){
	reverse = value;
}
bool ThreePosReg::getReverse(){
	return reverse;
}
bool ThreePosReg::getOut1(){
	if(reverse){
		return out2;
	}
	return out1;
}
bool ThreePosReg::getOut2(){
	if(reverse){
		return out1;
	}
	return out2;
}
bool ThreePosReg::impulseCycleEnded(){
	return pulseTimer->finished();
}
bool ThreePosReg::halfImpulseCycleEnded(){
	return pauseTimer->finished() && pulseTimer->notStarted();
}
void ThreePosReg::update1ms(){
	if(startTimer){
		*pauseTimer = pulseTimer->notFinished();
	}
	*pulseTimer = pauseTimer->finished();
	
	pauseTimer->update();
	pulseTimer->update();
	
	if(impulseCycleEnded() || halfImpulseCycleEnded()){
		startTimer = false;
	}
	if(!startTimer || halfImpulseCycleEnded()){
		direction = in < sp;
	}
	
	float sub = abs(in - sp);
	if(sub < pulseGist){
		out1 = direction && pulseTimer->inWork();
		out2 = !direction && pulseTimer->inWork();
		if(sub > zeroGist){
			startTimer = true;
		} else {
			if(halfImpulseCycleEnded()){
				pulseTimer->finish();
			}
		}
	} else {
		out1 = direction && pauseTimer->isFree();
		out2 = !direction && pauseTimer->isFree();
	}
}