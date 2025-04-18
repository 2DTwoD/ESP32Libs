#include "math_fun.h"

uint32_t round(float value){
    if((uint32_t)(value * 100.0) % 100 > 50) {
        return (uint32_t) value + 1;
    }
    return (uint32_t) value;
}