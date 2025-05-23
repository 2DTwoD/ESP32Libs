#ifndef COIL_H
#define COIL_H

#include "driver/gpio.h"
#include "hal/gpio_types.h"

#include "i_switch.h"
#include "program_coil.h"

class Coil: public ProgrammCoil{
	private:
		gpio_num_t pinNumber;
  	public:

    explicit Coil(gpio_num_t pinNumber);

    explicit Coil(int pinNumber);

    bool isActive() override;

    void setValue(bool value) override;

    using ProgrammCoil::operator=;
};

#endif //COIL_H