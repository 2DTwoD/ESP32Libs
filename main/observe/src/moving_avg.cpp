#include "moving_avg.h"

MovAvg::MovAvg(uint8_t size){
	this->size = max((uint8_t) 1, size);
	row = new float[size];
	memset(row, 0, sizeof(float) * size);
	pos = 0;
}
MovAvg::~MovAvg(){
	delete[] row;
}
void MovAvg::set(float value){
	in = value;
	updateSomewhere();
}
float MovAvg::get(){
	return out;
}
void MovAvg::updateSomewhere(){
	row[pos] = in;
	if(++pos >= size){
		pos = 0;
	}
	float avg;
	for(uint8_t i = 0; i < size; i++){
		avg += row[i];
	}
	avg /= size;
	out = avg;
}
MovAvg& MovAvg::operator=(float value){
	set(value);
	return *this;
}
float *const MovAvg::getInRef(){
	return &in;
}
float *const MovAvg::getOutRef(){
	return &out;
}