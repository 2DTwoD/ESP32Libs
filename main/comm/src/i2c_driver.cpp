#include "i2c_driver.h"

I2CDriver::I2CDriver(i2c_port_num_t port, uint8_t sclPin, uint8_t sdaPin, int32_t timeout = -1, bool pullUp) {
    bus_handle = new i2c_master_bus_handle_t();
    I2CParams params = {
            port,
            (gpio_num_t)sclPin,
            (gpio_num_t)sdaPin,
            pullUp
    };
    if(init(&params) != COMM_OK){
        ESP_LOGI(errorTag, "I2CDriver constructor error");
    }
    this->timeout = timeout;
    slaves = new ArrayList<i2c_master_dev_handle_t*>();
}

CommStatus I2CDriver::init(I2CParams* commParams) {
    i2c_master_bus_config_t i2c_master_config{};
    i2c_master_config.clk_source = I2C_CLK_SRC_DEFAULT;
    i2c_master_config.i2c_port = commParams->port;
    i2c_master_config.scl_io_num = commParams->sclPin;
    i2c_master_config.sda_io_num = commParams->sdaPin;
    i2c_master_config.flags.enable_internal_pullup = commParams->pullUp;
    i2c_master_config.glitch_ignore_cnt = 7;
    i2c_master_config.intr_priority = 0;
    i2c_master_config.trans_queue_depth = 0;
    if(i2c_new_master_bus(&i2c_master_config, bus_handle) != ESP_OK){
        ESP_LOGI(errorTag, "init error");
        return COMM_ERROR;
    }
    return COMM_OK;
}

CommStatus I2CDriver::read(uint8_t *bytes, uint16_t len) {
    if(i2c_master_receive(*slaves->get(slaveIndex), bytes, len, timeout) != ESP_OK){
        return COMM_ERROR;
    }
    return COMM_OK;
}

CommStatus I2CDriver::readSlave(uint8_t *bytes, uint16_t len, uint16_t slaveIndex) {
    if(selectSlave(slaveIndex) != COMM_OK){
        return COMM_ERROR;
    }
    return read(bytes, len);
}

CommStatus I2CDriver::write(uint8_t *const bytes, uint16_t len) {
    if(i2c_master_transmit(*slaves->get(slaveIndex), bytes, len, timeout) != ESP_OK){
        return COMM_ERROR;
    }
    return COMM_OK;
}

CommStatus I2CDriver::writeSlave(uint8_t *const bytes, uint16_t len, uint16_t slaveIndex) {
    if(selectSlave(slaveIndex) != COMM_OK){
        return COMM_ERROR;
    }
    return write(bytes, len);
}

CommStatus I2CDriver::addSlave(uint8_t address, uint32_t speed,i2c_addr_bit_len_t addrLen) {
    if(bus_handle == nullptr){
        return COMM_ERROR;
    }
    i2c_device_config_t i2c_device_config{};
    i2c_device_config.dev_addr_length = addrLen;
    i2c_device_config.device_address = address;
    i2c_device_config.scl_speed_hz = speed;
    i2c_device_config.scl_wait_us = 0;
    i2c_device_config.flags.disable_ack_check = 0;
    auto *dev_handle = new i2c_master_dev_handle_t();
    if(i2c_master_bus_add_device(*bus_handle, &i2c_device_config, dev_handle) != ESP_OK){
        ESP_LOGI(errorTag, "addSlave error");
        return COMM_ERROR;
    }
    slaves->add(dev_handle);
    return COMM_OK;
}


CommStatus I2CDriver::selectSlave(uint16_t index) {
    if(slaves == nullptr || slaves->size() == 0){
        return COMM_ERROR;
    }
    slaveIndex = limit(index, (uint16_t)0, slaves->size());
    return COMM_OK;
}

uint16_t I2CDriver::getSlaveIndex() const {
    return slaveIndex;
}

I2CDriver::~I2CDriver() {
    if(bus_handle != nullptr){
        i2c_del_master_bus(*bus_handle);
        delete bus_handle;
    }
    if(slaves != nullptr){
        slaves->forEach([](auto handle){
            delete handle;
        });
        delete slaves;
    }
}
