#include "array_fun.h"


float bytesToFloat(const uint8_t *const src){
    float result;
    auto *ptr = (uint8_t*)(&result);
    for(int8_t i = 0; i < 4; i++){
        ptr[3 - i] = src[i];
    }
    return result;
}

void floatToBytes(float value, uint8_t *const dst){
    auto *ptr = (uint8_t*) &value;
    for(int8_t i = 0; i < 4; i++){
        dst[i] = ptr[3 - i];
    }
}

float dWordToFloat(uint32_t dword){
    float result;
    auto *floatPtr = (uint8_t*) &result;
    auto *dwordPtr = (uint8_t*) &dword;
    for(int8_t i = 0; i < 4; i++){
        floatPtr[i] = dwordPtr[i];
    }
    return result;
}

uint32_t floatToDWord(float value){
    uint32_t result;
    auto *dwordPtr = (uint8_t*) &result;
    auto *floatPtr = (uint8_t*) &value;
    for(int8_t i = 0; i < 4; i++){
        dwordPtr[i] = floatPtr[i];
    }
    return result;
}

void copyStr(char *dst, const char* src){
    while(*src != 0){
        *dst = *src;
        dst++;
        src++;
    }
}