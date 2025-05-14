#include "monitor.h"

MonitorGeneral::MonitorGeneral(float valueMin, float valueMax){
    inLimits[0] = valueMin;
    setValueMax(valueMax);
    tresDelays[0] = new CommonTimer(200);
    tresDelays[1] = new CommonTimer(200);
    tresDelays[2] = new CommonTimer(200);
    tresDelays[3] = new CommonTimer(200);
}

MonitorGeneral::~MonitorGeneral(){
    for(auto & tresDelay : tresDelays){
        delete tresDelay;
    }
}
void MonitorGeneral::update() {
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
void MonitorGeneral::setValueMin(float limit){
	inLimits[0] = min(limit, inLimits[1]);
}
void MonitorGeneral::setValueMax(float limit){
	inLimits[1] = max(limit, inLimits[0]);
}
float MonitorGeneral::get(){
	return in;
}
void MonitorGeneral::set(float value){
	in = limit(value, inLimits[0], inLimits[1]);
}
void MonitorGeneral::setTreshold(TRES_TYPE tresType, uint16_t value){
	tresholds[tresType] = value;
}
void MonitorGeneral::setTresDelay(TRES_TYPE tresType, uint16_t del){
	tresDelays[tresType]->setPeriod(del);
}
bool MonitorGeneral::isHighAlarm(){
	return tresDelays[MON_HH]->finished();
}
bool MonitorGeneral::isHighWarn(){
	return tresDelays[MON_LH]->finished();
}
bool MonitorGeneral::isLowWarn(){
	return tresDelays[MON_HL]->finished();
}
bool MonitorGeneral::isLowAlarm(){
	return tresDelays[MON_LL]->finished();
}
MonitorGeneral& MonitorGeneral::operator=(float value) {
    set(value);
    return *this;
}

Monitor::Monitor(float valueMin, float valueMax) : MonitorGeneral(valueMin, valueMax) {
    Updater::addObj(this);
}

void Monitor::update1ms() {
    MonitorGeneral::update();
}
