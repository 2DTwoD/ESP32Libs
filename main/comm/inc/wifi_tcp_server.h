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
    WIFI_STA_TYPE = 1,
    WIFI_AP_STA_TYPE = 2
};

class WiFiTcpServer{
private:
    WiFiType type;
    char *ssid;
    char *password;
    char *tmpSsid;
    uint16_t port;
    IPv4addr apAddress;
    IPv4addr apMask;
    bool started{false};
    static inline uint8_t maxConn{3};

    static inline const char *TAG = "WiFiApTcpServer";
    static inline TaskHandle_t tcpServerTaskHandle{nullptr};
    static inline uint8_t staRetryNum{0};

    static void tcpServerTask(void *pvParameters);
    static void clientSocketTask(void *pvParameters);
    static void startServerTask(void *pvParameters);
    static void wifiEventHandler(void* arg, esp_event_base_t event_base,
                                 int32_t event_id, void* event_data);
    static void ipEventHandler(void* arg, esp_event_base_t event_base,
                                 int32_t event_id, void* event_data);
    void startTCPserver();
    void startClientSocket(int client_socket);
    void stopTCPserver();
    bool tcpServerStarted();
    void startAP(bool staFail);
    void startSTA();
    void sendData(int client_socket, uint8_t *sendingData, uint16_t len);
    virtual void afterReceiveAction(int client_socket, uint8_t *receivedData, uint16_t len);
public:
    WiFiTcpServer(WiFiType type, const char* ssid, const char* password, uint16_t port,
                  uint8_t maxConnections = 3,
                  const char* tmpSsid = "ESP32apTmp",
                  IPv4addr apAddress = {192, 168, 10, 1},
                  IPv4addr apMask = {255, 255, 255, 0});
    void start();
    WiFiType getType();
};


struct WiFiTcpServerTaskParams{
    uint16_t port{0};
    int client_socket{0};
    WiFiTcpServer* server{nullptr};
};

#endif //WIFI_AP_TCP_SERVER_H
