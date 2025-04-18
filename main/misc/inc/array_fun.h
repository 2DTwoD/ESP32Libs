#ifndef ARRAY_WORK_H
#define ARRAY_WORK_H

#include <cstdint>
#include "math_fun.h"

float bytesToFloat(const uint8_t *src);
void floatToBytes(float value, uint8_t *dst);

float dWordToFloat(uint32_t dword);
uint32_t floatToDWord(float value);

void copyStr(char *dst, const char* src);

template<typename T>
void copyArrays(const T *const src, T *const dst, uint8_t len){
    for(int i = 0; i < len; i++){
        dst[i] = src[i];
    }
}

template<typename T>
void fillArray(T *const array, uint32_t len, T fillValue){
    for(int i = 0; i < len; i++){
        array[i] = fillValue;
    }
}

template<typename T, typename X>
void insertElementInArray(T *const array, X len, X index, T value){
    if(index >= len) return;
    for(int i = len - 1; i > index; i--){
        array[i] = array[i - 1];
    }
    array[index] = value;
}

template<typename T, typename X>
void insertElementsInArray(T *const dst, X len, X index, const T *const src, X start, X quantity){
    if(index >= len) return;
    quantity = min(quantity, (X)(len - index));
    int i;
    for(i = len - 1; i >= index + quantity; i--){
        dst[i] = dst[i - quantity];
    }
    for(i = 0; i < quantity; i++){
        dst[index + i] = src[start + i];
    }
}

template<typename T, typename X>
void deleteElementsInArray(T *const array, X len, X start, X quantity, T endValue){
    if(start + quantity > len) return;
    for(int i = start; i < len; i++){
        if(i + quantity < len) {
            array[i] = array[i + quantity];
        } else {
            array[i] = endValue;
        }
    }
}

template<typename T, typename X>
void copyPartOfArray(T *const src, X srcLen, X start, X quantity, T *const dst){
    if(start > srcLen || start + quantity > srcLen) return;
    for(int i = 0; i < quantity; i++){
        dst[i] = src[i + start];
    }
}

template<typename T>
T getRange(const T *const limits){
    return limits[1] - limits[0];
}

#endif //ARRAY_WORK_H
