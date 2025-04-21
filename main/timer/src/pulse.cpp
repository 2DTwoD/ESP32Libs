#include "pulse.h"

//PulseCommon
PulseCommon::PulseCommon(uint32_t period): ITimer(period){
}
void PulseCommon::update(){
	if(CommonTimer::finished()){
		CommonTimer::stop();
	}
	CommonTimer::update();
}
void PulseCommon::set(bool value){
	if(value){
		if(!startFlag){
			startFlag = true;
			CommonTimer::start();
		}
	} else {
		if(CommonTimer::isFree()){
            waitFlag = false;
			startFlag = false;
		}
	}
}
void PulseCommon::wait(bool value) {
    waitFlag = value;
    CommonTimer::setPause(value);
}
bool PulseCommon::get(){
	return CommonTimer::inWork() || waitFlag;
}

void PulseCommon::again() {
    startFlag = false;
    waitFlag = false;
    CommonTimer::stop();
}

//Pulse
Pulse::Pulse(uint32_t period): PulseCommon(period){
    Updater::addObj(this);
}
void Pulse::update1ms(){
	PulseCommon::update();
}

//PulseInterrapt
PulseInterrapt::PulseInterrapt(uint32_t period): PulseCommon(period){
    Updater::addObj(this);
}
void PulseInterrapt::update1ms(){
	PulseCommon::update();
}
void PulseInterrapt::set(bool value){
	PulseCommon::set(value);
	if(!value) {
		PulseCommon::again();
	}
}