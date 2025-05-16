#include "coil.h"

Coil::Coil(gpio_num_t pinNumber): pinNumber(pinNumber){

    gpio_config_t config{};
    config.pin_bit_mask = 1UL << pinNumber;
    config.mode = GPIO_MODE_INPUT_OUTPUT;
    config.pull_up_en = GPIO_PULLUP_DISABLE;
    config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    config.intr_type = GPIO_INTR_DISABLE;

    gpio_config(&config);
}

Coil::Coil(int pinNumber): Coil((gpio_num_t)pinNumber){
}

bool Coil::isActive(){
	return gpio_get_level(pinNumber) > 0;
}

void Coil::setValue(bool value){
	gpio_set_level(pinNumber, value);
}



