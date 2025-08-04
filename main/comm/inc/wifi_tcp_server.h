#ifndef WIFI_AP_TCP_SERVER_H
#define WIFI_AP_TCP_SERVER_H

#include <cstdint>
#include <cstring>
#include <functional>
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

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

struct IPv4addr{
    uint8_t oct1{0};
    uint8_t oct2{0};
    uint8_t oct3{0};
    uint8_t oct4{0};
};

enum WiFiType{
    WIFI_AP_TYPE = 0,
    WIFI_STA_TYPE = 1
};

class WiFiTcpServer{
private:
    WiFiType type{};
    char *ssid{};
    char *password{};
    uint16_t port;
    IPv4addr apAddress{};
    IPv4addr apMask{};
    uint8_t maxConnections{10};
    bool started{false};

    static inline const char *TAG = "WiFiApTcpServer";
    static inline EventGroupHandle_t s_wifi_event_group = xEventGroupCreate();
    static inline uint8_t staRetryNum = 0;
    static inline uint8_t maxStaRetryNum = 5;

    static void tcpServerTask(void *pvParameters);
    static void wifiEventHandler(void* arg, esp_event_base_t event_base,
                                 int32_t event_id, void* event_data);
    void startAP();
    void startSTA();
public:
    WiFiTcpServer(WiFiType type, const char* ssid, const char* password, uint16_t port,
                    IPv4addr apAddress = {192, 168, 10, 10},
                    IPv4addr apMask = {255, 255, 255, 0},
                    uint8_t maxConnections = 10);
    void start();
    void sendData(int client_socket, uint8_t *sendingData, uint16_t len);
    virtual void afterReceiveAction(int client_socket, uint8_t *receivedData, uint16_t len);
};


struct WiFiApTcpServerTaskParams{
    uint16_t port{0};
    WiFiTcpServer* wiFiApTcpServer{nullptr};
};

#endif //WIFI_AP_TCP_SERVER_H
