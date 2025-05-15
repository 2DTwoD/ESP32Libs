#include "monitor.h"

MonitorCommon::MonitorCommon(float valueMin, float valueMax){
    inLimits[0] = valueMin;
    setValueMax(valueMax);
    tresDelays[0] = new CommonTimer(200);
    tresDelays[1] = new CommonTimer(200);
    tresDelays[2] = new CommonTimer(200);
    tresDelays[3] = new CommonTimer(200);
}

MonitorCommon::~MonitorCommon(){
    for(auto & tresDelay : tresDelays){
        delete tresDelay;
    }
}
void MonitorCommon::update() {
    float range = getRange(inLimits);
    float inPerc  = in * 100 / range;
    *tresDelays[0] = inPerc <= tresholds[MON_LL];
    *tresDelays[1] = inPerc <= tresholds[MON_HL];
    *tresDelays[2] = inPerc >= tresholds[MON_LH];
    *tresDelays[3] = inPerc >= tresholds[MON_HH];
    for(auto & tresDelay : tresDelays){
        tresDelay->update();
    }
}
void MonitorCommon::setValueMin(float limit){
	inLimits[0] = min(limit, inLimits[1]);
}
void MonitorCommon::setValueMax(float limit){
	inLimits[1] = max(limit, inLimits[0]);
}
float MonitorCommon::get(){
	return in;
}
void MonitorCommon::set(float value){
	in = limit(value, inLimits[0], inLimits[1]);
}
void MonitorCommon::setTreshold(TRES_TYPE tresType, uint16_t value){
	tresholds[tresType] = value;
}
void MonitorCommon::setTresDelay(TRES_TYPE tresType, uint16_t del){
	tresDelays[tresType]->setPeriod(del);
}
bool MonitorCommon::isHighAlarm(){
	return tresDelays[MON_HH]->finished();
}
bool MonitorCommon::isHighWarn(){
	return tresDelays[MON_LH]->finished();
}
bool MonitorCommon::isLowWarn(){
	return tresDelays[MON_HL]->finished();
}
bool MonitorCommon::isLowAlarm(){
	return tresDelays[MON_LL]->finished();
}
MonitorCommon& MonitorCommon::operator=(float value) {
    set(value);
    return *this;
}

Monitor::Monitor(float valueMin, float valueMax) : MonitorCommon(valueMin, valueMax) {
    Updater::addObj(this);
}

void Monitor::update1ms() {
    MonitorCommon::update();
}
