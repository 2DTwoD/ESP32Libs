#ifndef SIMPLE_INPUT_DELAYED_H
#define SIMPLE_INPUT_DELAYED_H

#include "simple_input.h"
#include "on_delay.h"
#include "updater.h"

class SimpleInputDelayed: public SimpleInput, private OnDelay{
public:
    SimpleInputDelayed(gpio_num_t pinNumber, SimpleInputPull pull, uint32_t period);

    SimpleInputDelayed(int pinNumber, SimpleInputPull pull, uint32_t period);

    SimpleInputDelayed(int pinNumber, uint32_t period);

    SimpleInputDelayed(gpio_num_t pinNumber, uint32_t period);

    bool isActive() override;

    void update1ms() override;
};

#endif //SIMPLE_INPUT_DELAYED_H
