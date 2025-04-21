#include "off_delay.h"

//OffDelayCommon
OffDelayCommon::OffDelayCommon(uint32_t period): ITimer(period){
}
void OffDelayCommon::update(){
	if(CommonTimer::finished()){
		CommonTimer::stop();
	}
	CommonTimer::update();
}
void OffDelayCommon::set(bool value){
	if(!value){
		if(startFlag){
			CommonTimer::prepareAndStart();
		}
	} else {
		CommonTimer::stop();
	}
	startFlag = value;
}
bool OffDelayCommon::get(){
    return startFlag || CommonTimer::inWork();
}
void OffDelayCommon::wait(bool value) {
    CommonTimer::setPause(value);
}
void OffDelayCommon::again(){
	CommonTimer::finish();
}


//OffDelay
OffDelay::OffDelay(uint32_t period): OffDelayCommon(period){
    Updater::addObj(this);
}
void OffDelay::update1ms(){
	OffDelayCommon::update();
}
