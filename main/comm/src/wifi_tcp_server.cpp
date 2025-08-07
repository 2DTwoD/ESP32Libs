#include <lwip/sockets.h>
#include <dhcpserver/dhcpserver.h>

#include "wifi_tcp_server.h"
#include "common.h"


WiFiTcpServer::WiFiTcpServer(WiFiType type, const char* ssid, const char* password, uint16_t port,
                             uint8_t maxConnections, const char* tmpSsid, uint16_t maxTry,
                             IPv4addr apAddress, IPv4addr apMask):
                                 type(type), ssid((char*) ssid), password((char*)password), tmpSsid((char*)tmpSsid),
                                 port(port), apAddress(apAddress), apMask(apMask) {
    if(strlen(ssid) < 1 || strlen(ssid) > 32){
        ESP_LOGW(TAG, "SSID must be between 1 and 32 characters long");
    }
    if(strlen(tmpSsid) < 1 || strlen(tmpSsid) > 32){
        ESP_LOGW(TAG, "tmpSSID must be between 1 and 32 characters long");
    }
    if(strlen(password) < 8){
        ESP_LOGW(TAG, "The password must contain at least 8 characters");
    }
    WiFiTcpServer::maxConn = maxConnections;
    WiFiTcpServer::maxTry = maxTry;
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    start();
}

void WiFiTcpServer::start() {
    if(xTaskCreate(startServerTask,
                   "wifi_st_t",
                   4096,
                   (void*)this,
                   5,
                   nullptr) != pdPASS){
        ESP_LOGE(TAG, "Start server task not created");
        return;
    }
}

void WiFiTcpServer::startClientSocket(int client_socket) {
    WiFiTcpServerTaskParams params;
    params.client_socket = client_socket;
    params.server = this;
    char taskName[15];
    sprintf(taskName, "wifi_cst_%d", client_socket);
    if(xTaskCreate(clientSocketTask,
                   taskName,
                   4096,
                   (void*)&params,
                   5,
                   nullptr) != pdPASS){
        ESP_LOGE(TAG, "Client socket task not created");
    }
}

void WiFiTcpServer::startServerTask(void *pvParameters) {
    auto *server = (WiFiTcpServer*) pvParameters;
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifiEventHandler,
                                                        pvParameters,
                                                        nullptr));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &ipEventHandler,
                                                        pvParameters,
                                                        nullptr));
    switch (server->getType()) {
        case WIFI_AP_TYPE:
            server->startAP(false);
            break;
        case WIFI_STA_TYPE:
        case WIFI_STA_AP_TYPE:
            server->startSTA();
    }
    //Start TCP server
    sockaddr_in destAddr{};
    //Change hostname to network byte order
    destAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    //Define address family as Ipv4
    destAddr.sin_family = AF_INET;
    //Define PORT
    destAddr.sin_port = htons(server->getPort());
    while (true) {
        //Create TCP socket
        int socket_id = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (socket_id < 0) {
            ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
            OsDelay(1000);
            continue;
        }
        ESP_LOGI(TAG, "Socket created");
        //Bind a socket to a specific IP + port
        if (bind(socket_id, (struct sockaddr *)&destAddr, sizeof(destAddr)) != 0) {
            ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
            OsDelay(1000);
            continue;
        }
        ESP_LOGI(TAG, "Socket binded");

        //Begin listening for clients on socket
        if (listen(socket_id, 3) != 0) {
            ESP_LOGE(TAG, "Error occured during listen: errno %d", errno);
            OsDelay(1000);
            continue;
        }
        ESP_LOGI(TAG, "Socket listening");
        while(true) {
            //Large enough for IPv4
            sockaddr_in sourceAddr{};
            uint addrLen = sizeof(sourceAddr);
            //Accept connection to incoming client
            int client_socket = accept(socket_id, (sockaddr *)&sourceAddr, (socklen_t *)(&addrLen));
            if (client_socket < 0) {
                ESP_LOGE(TAG, "Unable to accept connection: errno %d", errno);
                break;
            }
            ESP_LOGI(TAG, "Socket accepted");
            server->startClientSocket(client_socket);
        }
    }
}

void WiFiTcpServer::clientSocketTask(void *pvParameters) {
    // char array to store received data
    char rx_buffer[128];
    // immediate bytes received
    int bytes_received;
    int client_socket = (*(WiFiTcpServerTaskParams*)pvParameters).client_socket;
    WiFiTcpServer* server = (*(WiFiTcpServerTaskParams*)pvParameters).server;
    while(true) {
        //Clear rx_buffer, and fill with zero's
        bzero(rx_buffer, sizeof(rx_buffer));
        OsDelay(500);
        while(true) {
            ESP_LOGI(TAG, "Waiting for data");
            bytes_received = recv(client_socket, rx_buffer, sizeof(rx_buffer) - 1, 0);
            ESP_LOGI(TAG, "Received Data");
            //Error occured during receiving or connection closed
            if (bytes_received <= 0) {
                ESP_LOGI(TAG, "Connection closed");
                closesocket(client_socket);
                vTaskDelete(nullptr);
                break;
            //Data received
            } else {
                //Null-terminate whatever we received and treat like a string
                rx_buffer[bytes_received] = 0;
                ESP_LOGI(TAG, "%s", rx_buffer);
                //After receive action
                server->afterReceiveAction(client_socket, (uint8_t *)rx_buffer,
                                                      sizeof(rx_buffer));
                //Clear rx_buffer, and fill with zero's
                bzero(rx_buffer, sizeof(rx_buffer));
            }
        }
    }
}

void WiFiTcpServer::wifiEventHandler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    ESP_LOGI(TAG, "Wifi event catch: %ld", event_id);
    auto server = (WiFiTcpServer*)arg;
    //AP Events
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        auto event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(TAG, "station " MACSTR" join, AID=%d",
                 MAC2STR(event->mac), event->aid);
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        auto event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(TAG, "station " MACSTR" leave, AID=%d, reason=%d",
                 MAC2STR(event->mac), event->aid, event->reason);
    //STA Events
    } else if (event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_id == WIFI_EVENT_STA_DISCONNECTED || event_id == WIFI_EVENT_STA_BEACON_TIMEOUT) {
        if (staRetryNum < maxTry) {
            ESP_LOGI(TAG, "retry to connect to the AP");
            esp_wifi_connect();
            if(server->getType() != WIFI_STA_TYPE) staRetryNum++;
        } else {
            server->startAP(true);
        }
        ESP_LOGI(TAG,"connect to the AP fail");
    }
}

void WiFiTcpServer::ipEventHandler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    ESP_LOGI(TAG, "IP event catch: %ld", event_id);
    //STA Events
    if (event_id == IP_EVENT_STA_GOT_IP) {
        auto event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "STA connected, got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        staRetryNum = 0;
        getAddress(staAddress, event->ip_info.ip.addr);
        getAddress(staMask, event->ip_info.netmask.addr);
    }
}

void WiFiTcpServer::startAP(bool staFail) {
    ESP_ERROR_CHECK(esp_wifi_stop());
    ESP_LOGI(TAG, "ESP_WIFI_MODE_AP");
    //Initialize WiFi
    esp_netif_t *netif = esp_netif_create_default_wifi_ap();
    //Configure AP IP
    ESP_ERROR_CHECK(esp_netif_dhcps_stop(netif));
    esp_netif_ip_info_t ip_info;
    IP4_ADDR(&ip_info.ip, apAddress.oct1, apAddress.oct2, apAddress.oct3, apAddress.oct4);
    IP4_ADDR(&ip_info.gw, apAddress.oct1, apAddress.oct2, apAddress.oct3, apAddress.oct4);
    IP4_ADDR(&ip_info.netmask, apMask.oct1, apMask.oct2, apMask.oct3, apMask.oct4);
    ESP_ERROR_CHECK(esp_netif_set_ip_info(netif, &ip_info));
    esp_netif_dhcps_start(netif);

    wifi_config_t wifi_config = {};
    strcpy((char *)wifi_config.ap.ssid, staFail? tmpSsid: ssid);
    strcpy((char *)wifi_config.ap.password, password);
    wifi_config.ap.ssid_len = strlen(ssid);
    wifi_config.ap.channel = 6;
    wifi_config.ap.authmode = WIFI_AUTH_WPA2_PSK;
    wifi_config.ap.max_connection = maxConn;

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_LOGI(TAG, "wifi_init_softap finished.");
    ESP_ERROR_CHECK(esp_wifi_start());
}

void WiFiTcpServer::startSTA() {
    ESP_ERROR_CHECK(esp_wifi_stop());
    esp_netif_create_default_wifi_sta();

    wifi_config_t wifi_config{};
    strcpy((char*) wifi_config.sta.ssid, ssid);
    strcpy((char*) wifi_config.sta.password, password);
    wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));

    ESP_LOGI(TAG, "wifi_init_sta finished.");
    ESP_ERROR_CHECK(esp_wifi_start());
}

void WiFiTcpServer::afterReceiveAction(int client_socket, uint8_t *receivedData, uint16_t len) {
    sendData(client_socket, receivedData, len);
}

void WiFiTcpServer::sendData(int client_socket, uint8_t *sendingData, uint16_t len) {
    send(client_socket, sendingData, len, 0);
}

WiFiType WiFiTcpServer::getType() const {
    return type;
}

uint16_t WiFiTcpServer::getPort() const {
    return port;
}

void WiFiTcpServer::getAddress(IPv4addr& dst, uint32_t src) {
    dst.oct4 = (src >> 24) & 0xFF;
    dst.oct3 = (src >> 16) & 0xFF;
    dst.oct2 = (src >> 8) & 0xFF;
    dst.oct1 = src & 0xFF;
}

IPv4addr WiFiTcpServer::getAPip() {
    return apAddress;
}

IPv4addr WiFiTcpServer::getAPmask() {
    return apMask;
}

IPv4addr WiFiTcpServer::getSTAip() {
    return staAddress;
}

IPv4addr WiFiTcpServer::getSTAmask() {
    return staMask;
}

void WiFiTcpServer::printAPipMask() const {
    ESP_LOGI(TAG, "AP ip: %d.%d.%d.%d, AP mask: %d.%d.%d.%d",
             apAddress.oct1, apAddress.oct2, apAddress.oct3, apAddress.oct4,
             apMask.oct1, apMask.oct2, apMask.oct3, apMask.oct4);
}

void WiFiTcpServer::printSTAipMask() const {
    ESP_LOGI(TAG, "STA ip: %d.%d.%d.%d, STA mask: %d.%d.%d.%d",
             staAddress.oct1, staAddress.oct2, staAddress.oct3, staAddress.oct4,
             staMask.oct1, staMask.oct2, staMask.oct3, staMask.oct4);
}

