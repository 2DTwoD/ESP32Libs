# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
include_directories($ENV{IDF_PATH}/components/esp_driver_gpio/include)
include_directories($ENV{IDF_PATH}/components/esp_driver_gptimer/include)
project(ESP32Libs)
