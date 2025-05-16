#include "simple_input_delayed.h"

SimpleInputDelayed::SimpleInputDelayed(gpio_num_t pinNumber, SimpleInputPull pull,
                                       uint32_t period): SimpleInput(pinNumber,
                                                                     pull), OnDelay(period) {
}

SimpleInputDelayed::SimpleInputDelayed(int pinNumber,
                                       SimpleInputPull pull,
                                       uint32_t period): SimpleInputDelayed((gpio_num_t)pinNumber,
                                                                            pull,
                                                                            period)  {}

SimpleInputDelayed::SimpleInputDelayed(gpio_num_t pinNumber,
                                       uint32_t period): SimpleInputDelayed(pinNumber,
                                                                            PULL_UP,
                                                                            period) {}

SimpleInputDelayed::SimpleInputDelayed(int pinNumber,
                                       uint32_t period): SimpleInputDelayed((gpio_num_t)pinNumber,
                                                                            period)  {}

bool SimpleInputDelayed::isActive() {
    return OnDelay::get();
}

void SimpleInputDelayed::update1ms() {
    OnDelay::set(SimpleInput::isActive());
    OnDelay::update1ms();
}
