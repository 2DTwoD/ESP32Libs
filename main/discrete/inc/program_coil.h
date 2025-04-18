#ifndef PROGRAMM_COIL_H
#define PROGRAMM_COIL_H

#include "i_switch.h"

class ProgrammCoil: public ISwitch{
private:
    bool out{};
public:
    bool isActive() override;
    virtual void setValue(bool value);
    void set();
    void reset();
    void toggle();
    ProgrammCoil& operator=(bool value);
};

#endif //PROGRAMM_COIL_H
