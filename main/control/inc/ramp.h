#ifndef RAMP_H
#define RAMP_H

#include <cstdint>

#include "updater.h"
#include "math_fun.h"
#include "array_fun.h"

class RampCommon{
private:
    float out{};
    float sp{};
    float step{};
    uint32_t fullRangeTime{};
    float outRange[2]{};
    float outLimits[2]{};
protected:
    float getOutMin();
    float getOutRange();
public:
    explicit RampCommon(uint32_t fullRangeTime = 0);
    RampCommon(uint32_t fullRangeTime, float outMin, float outMax);
    RampCommon(uint32_t fullRangeTime, float outMin, float outMax, float limitMin, float limitMax);
    void update();
    float get() const;
    void set(float value);
    uint32_t getFullRangeTime() const;
    void setFullRangeTime(uint32_t value);
    void setLimMin(float value);
    void setLimMax(float value);
    RampCommon& operator=(float value);
};
class Ramp: public IUpdated1ms, RampCommon{
public:
    using RampCommon::RampCommon;
    void update1ms() override;
};

#endif //RAMP_H