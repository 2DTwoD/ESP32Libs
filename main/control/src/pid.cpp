#include "pid.h"
PIDreg::PIDreg(uint16_t t, float sp, float kp, float ti, float td,
               float db, float upOutLim, float downOutLim, bool inverse):
               t(t), sp(sp), kp(kp), ti(ti), td(td), db(db),
               upOutLim(upOutLim), downOutLim(downOutLim), inverse(inverse){
	updateKoef();
	reset();
}
uint16_t PIDreg::getT() const{
	return t;
}
void PIDreg::setPv(float value){
	pv = value;
	updateSomewhere();
}
float PIDreg::getPv() const{
	return pv;
}
void PIDreg::setSp(float value){
	sp = value;
}
float PIDreg::getSp() const{
	return sp;
}
void PIDreg::setKp(float value){
	kp = value;
	updateKoef();
}
float PIDreg::getKp() const{
	return kp;
}
void PIDreg::setTi(float value){
	ti = value;
	updateKoef();
}
float PIDreg::getTi() const{
	return ti;
}
void PIDreg::setTd(float value){
	td = value;
	updateKoef();
}
float PIDreg::getTd() const{
	return td;
}
void PIDreg::setDb(float value){
	db = abs(value);
}
float PIDreg::getDb() const{
	return db;
}
void PIDreg::setUpOutLim(float value){
	upOutLim = max(value, downOutLim);
}
float PIDreg::getUpOutLim() const{
	return upOutLim;
}
void PIDreg::setDownOutLim(float value){
	downOutLim = min(value, upOutLim);
}
float PIDreg::getDownOutLim() const{
	return downOutLim;
}
void PIDreg::setOut(float value){
	if(AUTO) return;
	out = limit(value, downOutLim, upOutLim);
}
float PIDreg::getOut() const{
	return out;
}
void PIDreg::setAuto(bool value){
	AUTO = value;
}
bool PIDreg::isAUTO() const{
	return AUTO;
}
void PIDreg::setInverse(bool value){
	inverse = value;
}
bool PIDreg::isInverse() const{
	return inverse;
}
void PIDreg::updateSomewhere(){
	if(!AUTO) return;
	float deviation = inverse? (pv - sp): (sp - pv);
	if(abs(deviation) < db) return;
	e2 = e1;
	e1 = e0;
	e0 = deviation;
	out += g0 * e0 + g1 * e1 + g2 * e2;
	out = limit(out, downOutLim, upOutLim);
}
void PIDreg::reset(){
	out = 0.0;
	e0 = 0.0;
	e1 = 0.0;
	e2 = 0.0;
}
void PIDreg::updateKoef(){
	if(t == 0.0f) return;
	if(ti == 0.0f){
		g0 = kp * (1.0f + td / t);
	} else {
		g0 = kp * (1.0f + t / ti + td / t);
	}
	g1 = kp * (-1.0f - 2.0f * td / t);
	g2 = kp * td / t;
}
PIDreg& PIDreg::operator=(float value){
	setPv(value);
	return *this;
}

