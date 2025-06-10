#ifndef I_COMM_DRIVER_H
#define I_COMM_DRIVER_H

#include <cstdint>
#include "driver/i2c_types.h"
#include "driver/i2c_master.h"

enum CommStatus{
    COMM_OK = 0,
    COMM_ERROR = 1
};

class ICommRW{
public:
    virtual CommStatus read(uint8_t* bytes, uint16_t len) = 0;
    virtual CommStatus write(uint8_t *const bytes, uint16_t len) = 0;
};

template <typename T>
class ICommDriver: public ICommRW{
public:
    virtual CommStatus init(T* commPars) = 0;
};



#endif //I_COMM_DRIVER_H
