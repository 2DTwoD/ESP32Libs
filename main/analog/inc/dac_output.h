#ifndef DAC_OUTPUT_H
#define DAC_OUTPUT_H

#include <cstdint>
#include <driver/dac_oneshot.h>

class DacOutput{
private:
    dac_oneshot_handle_t dacHandle{nullptr};
public:
    DacOutput(dac_channel_t channel);
    void set(uint8_t value);
};

#endif //DAC_OUTPUT_H
