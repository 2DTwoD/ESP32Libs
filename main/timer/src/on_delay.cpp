#include "on_delay.h"

//OnDelayCommon
OnDelayCommon::OnDelayCommon(uint32_t period): CommonTimer(period){
}
void OnDelayCommon::update(){
	CommonTimer::update();
}
void OnDelayCommon::set(bool value){
	CommonTimer::setStart(value);
}
void OnDelayCommon::pause(bool value){
	CommonTimer::setPause(value);
}
bool OnDelayCommon::get(){
	return CommonTimer::finished();
}
void OnDelayCommon::reset(){
	CommonTimer::reset();
}

void OnDelayCommon::again() {
    CommonTimer::prepareAndStart();
}

//OnDelay
OnDelay::OnDelay(uint32_t period): OnDelayCommon(period){
    Updater::addObj(this);
}
void OnDelay::update1ms(){
	OnDelayCommon::update();
}