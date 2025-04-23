#ifndef SHTC3_H
#define SHTC3_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "i2c_driver.h"
#include "updater.h"

class SHTC3Sensor: private I2CDriver, public IUpdatedSomewhere{
private:
    uint8_t measureCmd[2] {0x7C, 0xA2};
    float temperature;
    float humidity;
public:
    SHTC3Sensor(i2c_port_num_t port, uint8_t sclPin, uint8_t sdaPin, int32_t timeout = -1, bool pullUp = true);

    void updateSomewhere() override;

    float getTemperature() const;

    float getHumidity() const;
};

#endif //SHTC3_H
