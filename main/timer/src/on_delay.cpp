#include "on_delay.h"

//OnDelayCommon
OnDelayCommon::OnDelayCommon(uint32_t period): ITimer(period){
}
void OnDelayCommon::update(){
	CommonTimer::update();
}
void OnDelayCommon::set(bool value){
	CommonTimer::setStart(value);
}
bool OnDelayCommon::get(){
	return CommonTimer::finished();
}
void OnDelayCommon::wait(bool value){
    CommonTimer::setPause(value);
}
void OnDelayCommon::again() {
	CommonTimer::reset();
}

//OnDelay
OnDelay::OnDelay(uint32_t period): OnDelayCommon(period){
//    Updater::addObj(this);
}
void OnDelay::update1ms(){
	OnDelayCommon::update();
}