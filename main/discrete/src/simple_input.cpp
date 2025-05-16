#include "simple_input.h"

SimpleInput::SimpleInput(gpio_num_t pinNumber, SimpleInputPull pull): pinNumber(pinNumber){

    gpio_pullup_t pullUpEn = GPIO_PULLUP_DISABLE;
    gpio_pulldown_t pullDownEn = GPIO_PULLDOWN_DISABLE;
    if(pull == PULL_UP){
        pullUpEn = GPIO_PULLUP_ENABLE;
        reverse = true;
    } else if(pull == PULL_DOWN){
        pullDownEn = GPIO_PULLDOWN_ENABLE;
    }

    gpio_config_t config{};
    config.pin_bit_mask = 1UL << pinNumber;
    config.mode = GPIO_MODE_INPUT;
    config.pull_up_en = pullUpEn;
    config.pull_down_en = pullDownEn;
    config.intr_type = GPIO_INTR_DISABLE;

    gpio_config(&config);
}

SimpleInput::SimpleInput(int pinNumber, SimpleInputPull pull): SimpleInput((gpio_num_t)pinNumber, pull){
}

SimpleInput::SimpleInput(gpio_num_t pinNumber): SimpleInput(pinNumber, PULL_UP) {
}

SimpleInput::SimpleInput(int pinNumber): SimpleInput(pinNumber, PULL_UP) {
}

bool SimpleInput::isActive(){
    return (gpio_get_level(pinNumber) > 0) ^ reverse;
}
