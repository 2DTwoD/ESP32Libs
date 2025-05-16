#ifndef DAC_OUTPUT_H
#define DAC_OUTPUT_H

#include <cstdint>
#include "dac_oneshot.h"
#include <hal/dac_types.h>

class DacOutput{
private:
    dac_oneshot_handle_t dacHandle{nullptr};
public:
    DacOutput();
    void set(uint16_t value);
};

#endif //DAC_OUTPUT_H
