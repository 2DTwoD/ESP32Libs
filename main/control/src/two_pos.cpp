#include "two_pos.h"

//TwoPosCommon
TwoPosCommon::TwoPosCommon(float sp, bool reverse): sp(sp), reverse(reverse){
}
void TwoPosCommon::set(float value){
	in = value;
}
float TwoPosCommon::get(){
	return out;
}
void TwoPosCommon::setSp(float value){
	sp = value;
}
float TwoPosCommon::getSp(){
	return sp;
}
void TwoPosCommon::setReverse(bool value){
	reverse = value;
}
bool TwoPosCommon::getReverse(){
	return reverse;
}


//TwoPosGist
TwoPosGist::TwoPosGist(float sp, float lowGist, float highGist, bool reverse): TwoPosCommon(sp, reverse){
	setLowGist(lowGist);
	setHighGist(highGist);
}
void TwoPosGist::set(float value){
	TwoPosCommon::set(value);
	updateSomewhere();
}
void TwoPosGist::setLowGist(float value){
	lowGist = max(0.0f, value);
}
float TwoPosGist::getLowGist(){
	return lowGist;
}
void TwoPosGist::setHighGist(float value){
	highGist = max(0.0f, value);
}
float TwoPosGist::getHighGist(){
	return highGist;
}
void TwoPosGist::updateSomewhere(){
	if(in < sp - lowGist){
		out = !reverse;
	} else if (in > sp + highGist){
		out = reverse;
	}
}
TwoPosGist& TwoPosGist::operator=(float value){
	set(value);
	return *this;
}

//TwoPosTim
TwoPosTim::TwoPosTim(float sp, uint16_t lowTime, uint16_t highTime, bool reverse): TwoPosCommon(sp, reverse){
	lowTimer = new CommonTimer(lowTime);
	highTimer = new CommonTimer(highTime);
}
TwoPosTim::~TwoPosTim(){
	delete lowTimer;
	delete highTimer;
}
void TwoPosTim::setLowTime(uint16_t value){
	lowTimer->setPeriod(value);
}
uint16_t TwoPosTim::getLowTime(){
	return lowTimer->getPeriod();
}
void TwoPosTim::setHighTime(uint16_t value){
	highTimer->setPeriod(value);
}
uint16_t TwoPosTim::getHighTime(){
	return highTimer->getPeriod();
}
void TwoPosTim::update1ms(){
	lowTimer->setStart(in < sp);
	highTimer->setStart(in > sp);
	lowTimer->update();
	highTimer->update();
	if(lowTimer->finished()){
		out = !reverse;
	} else if (highTimer->finished()){
		out = reverse;
	} 
}
TwoPosTim& TwoPosTim::operator=(float value){
	set(value);
	return *this;
}