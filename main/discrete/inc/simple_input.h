#ifndef SIMPLE_INPUT_H
#define SIMPLE_INPUT_H

#include "i_switch.h"
#include "driver/gpio.h"

enum SimpleInputPull{
    NO_PULL = 0,
    PULL_UP = 1,
    PULL_DOWN = 2
};

class SimpleInput: public ISwitch{
    private:
        gpio_num_t pinNumber;
        bool reverse{false};
	public:
        SimpleInput(gpio_num_t pinNumber, SimpleInputPull pull);

        SimpleInput(int pinNumber, SimpleInputPull pull);

        explicit SimpleInput(gpio_num_t pinNumber);

        explicit SimpleInput(int pinNumber);
	
		bool isActive() override;
};


#endif //SIMPLE_INPUT_H