#ifndef MATH_FUN_H
#define MATH_FUN_H
#include <cstdint>

template<typename T>
T max(T val1, T val2){
    if(val1 > val2){
        return val1;
    }
    return val2;
}

template<typename T>
T min(T val1, T val2){
    if(val1 < val2){
        return val1;
    }
    return val2;
}

template<typename T>
void toggle(T *const val1, T *const val2){
    T toggle;
    toggle = *val1;
    *val1 = *val2;
    *val2 = toggle;
}

template<typename T>
T limit(T value, T valMin, T valMax){
    if(valMin > valMax){
        toggle(&valMin, &valMax);
    }
    if(value < valMin){
        value = valMin;
    } else if(value > valMax){
        value = valMax;
    }
    return value;
}

template<typename T>
T abs(T value){
    if(value < 0){
        value *= -1;
    }
    return value;
}

uint32_t round(float value);

#endif //MATH_FUN_H
