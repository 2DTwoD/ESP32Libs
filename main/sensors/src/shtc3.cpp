#include "shtc3.h"

SHTC3Sensor::SHTC3Sensor(i2c_port_num_t port, uint8_t sclPin, uint8_t sdaPin, int32_t timeout, bool pullUp) : I2CDriver(
        port, sclPin, sdaPin, timeout, pullUp) {
    addSlave(0x70, 100000);
}

void SHTC3Sensor::updateSomewhere() {
    uint8_t data[6];
    write(measureCmd, 2);
    vTaskDelay(20 / portTICK_PERIOD_MS);
    read(data, 6);
    uint16_t temp_raw = (data[0] << 8) | data[1];
    uint16_t hum_raw = (data[3] << 8) | data[4];
    temperature = -45 + 175 * ((float)temp_raw / 65535.0);
    humidity = 100 * ((float)hum_raw / 65535.0);
}

float SHTC3Sensor::getTemperature() const {
    return temperature;
}

float SHTC3Sensor::getHumidity() const {
    return humidity;
}
