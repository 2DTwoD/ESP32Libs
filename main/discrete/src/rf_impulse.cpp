#include "rf_impulse.h"

RFimpulse::RFimpulse(IMPULSE dir): dir(dir){
}

void RFimpulse::set(bool value){
	switch(dir){
		case RFI_RISE:
			if(value && !prev){
				out = true;
			} 
			break;
		case RFI_FALL:
			if(!value && prev){
				out = true;
			}
			break;
		default:
			if(value != prev){
				out = true;
			} 
	}
	prev = value;
}

bool RFimpulse::get(){
	bool result = out;
	out = false;
	return result;
}

RFimpulse& RFimpulse::operator=(bool value){
	set(value);
	return *this;
}