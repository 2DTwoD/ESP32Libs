#ifndef WIFI_AP_TCP_SERVER_H
#define WIFI_AP_TCP_SERVER_H

#include <cstdint>
#include <cstring>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_mac.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"
#include "array_fun.h"

struct IPv4addr{
    uint8_t oct1{0};
    uint8_t oct2{0};
    uint8_t oct3{0};
    uint8_t oct4{0};
};

class WiFiApTcpServer{
private:
    char *ssid{};
    char *password{};
    uint16_t port;
    IPv4addr apAddress{};
    IPv4addr apMask{};
    uint8_t maxConnections{10};
    inline static const char *TAG = "WiFiApTcpServer";
    static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                   int32_t event_id, void* event_data);
    static void tcp_server_task(void *pvParameters);
public:
    WiFiApTcpServer(const char* ssid, const char* password, uint16_t port,
                    IPv4addr apAddress = {192, 168, 10, 10},
                    IPv4addr apMask = {255, 255, 255, 0},
                    uint8_t maxConnections = 10);
    void start();
};

#endif //WIFI_AP_TCP_SERVER_H
