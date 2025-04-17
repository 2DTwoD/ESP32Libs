
#ifndef I_SWITCH_H
#define I_SWITCH_H

class ISwitch {
public:
    virtual bool isActive() = 0;
    bool isNotActive() {
        return !isActive();
    }
};

#endif //I_SWITCH_H
