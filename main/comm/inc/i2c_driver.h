#ifndef I2C_DRIVER_H
#define I2C_DRIVER_H

#include <esp_log.h>

#include "i_comm_driver.h"
#include "array_list.h"

struct I2CParams{
    i2c_port_num_t port{I2C_NUM_0};
    gpio_num_t sclPin{GPIO_NUM_22};
    gpio_num_t sdaPin{GPIO_NUM_21};
    bool pullUp{true};
};

class I2CDriver: public ICommDriver<I2CParams>{
private:
    i2c_master_bus_handle_t* bus_handle;
    const char* errorTag = "I2CDriver ERROR";
    ArrayList<i2c_master_dev_handle_t*> *slaves{nullptr};
    int32_t timeout{-1};
    uint16_t slaveIndex{0};
public:
    virtual ~I2CDriver();

public:
    I2CDriver(i2c_port_num_t port, uint8_t sclPin, uint8_t sdaPin, int32_t timeout, bool pullUp = true);

    CommStatus init(I2CParams* commParams) override;

    CommStatus read(uint8_t *bytes, uint16_t len) override;

    CommStatus readSlave(uint8_t *bytes, uint16_t len, uint16_t slaveIndex);

    CommStatus write(uint8_t *const bytes, uint16_t len) override;

    CommStatus writeSlave(uint8_t *const bytes, uint16_t len, uint16_t slaveIndex);

    CommStatus selectSlave(uint16_t index);

    uint16_t getSlaveIndex() const;

    CommStatus addSlave(uint8_t address, uint32_t speed, i2c_addr_bit_len_t addrLen = I2C_ADDR_BIT_LEN_7);
};

#endif //I2C_DRIVER_H
