# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
include_directories(
        $ENV{IDF_PATH}/components/esp_driver_gpio/include
        $ENV{IDF_PATH}/components/esp_driver_gptimer/include
        $ENV{IDF_PATH}/components/esp_driver_i2c/include
        $ENV{IDF_PATH}/components/esp_wifi/include
        $ENV{IDF_PATH}/components/esp_event/include
        $ENV{IDF_PATH}/components/nvs_flash/include
        $ENV{IDF_PATH}/components/esp_netif/include
        $ENV{IDF_PATH}/components/esp_partition/include
        $ENV{IDF_PATH}/components/esp_adc/include
        $ENV{IDF_PATH}/components/esp_adc/esp32/include
)
project(ESP32Libs)
