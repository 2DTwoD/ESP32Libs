#ifndef WIFI_TCP_SERVER_H
#define WIFI_TCP_SERVER_H

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

struct IPv4addr{
    uint8_t oct1{0};
    uint8_t oct2{0};
    uint8_t oct3{0};
    uint8_t oct4{0};
};

enum WiFiType{
    WIFI_AP_TYPE = 0,
    WIFI_STA_TYPE = 1,
    WIFI_STA_AP_TYPE = 2
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
    static inline IPv4addr staAddress{0,0,0,0};
    static inline IPv4addr staMask{0,0,0,0};
    static inline uint8_t maxConn{3};
    static inline uint16_t maxTry{25};

    static inline const char *TAG = "WiFiTcpServer";
    static inline uint8_t staRetryNum{0};

    void start();
    void startClientSocket(int client_socket);
    static void startServerTask(void *pvParameters);
    static void clientSocketTask(void *pvParameters);
    static void wifiEventHandler(void* arg, esp_event_base_t event_base,
                                 int32_t event_id, void* event_data);
    static void ipEventHandler(void* arg, esp_event_base_t event_base,
                                 int32_t event_id, void* event_data);
    static void getAddress(IPv4addr& dst, uint32_t src);
    void startAP(bool staFail);
    void startSTA();
    virtual void afterReceiveAction(int client_socket, uint8_t *receivedData, uint16_t len);
protected:
    void sendData(int client_socket, uint8_t *sendingData, uint16_t len);
public:
    WiFiTcpServer(WiFiType type, const char* ssid, const char* password, uint16_t port,
                  uint8_t maxConnections = 3,
                  const char* tmpSsid = "ESP32apTmp",
                  uint16_t maxTry = 25,
                  IPv4addr apAddress = {192, 168, 10, 1},
                  IPv4addr apMask = {255, 255, 255, 0});
    WiFiType getType() const;
    uint16_t getPort() const;
    IPv4addr getAPip();
    IPv4addr getAPmask();
    IPv4addr getSTAip();
    IPv4addr getSTAmask();
    void printAPipMask() const;
    void printSTAipMask() const;
};


struct WiFiTcpServerTaskParams{
    int client_socket{0};
    WiFiTcpServer* server{nullptr};
};

#endif //WIFI_TCP_SERVER_H
