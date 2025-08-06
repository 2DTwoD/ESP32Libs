#include <lwip/sockets.h>
#include <dhcpserver/dhcpserver.h>
//#include <dhcpserver/dhcpserver_options.h>
#include "wifi_tcp_server.h"
#include "common.h"


WiFiTcpServer::WiFiTcpServer(WiFiType type, const char* ssid, const char* password, uint16_t port,
                             uint8_t maxConnections, const char* tmpSsid, IPv4addr apAddress, IPv4addr apMask):
                                 type(type), ssid((char*) ssid), password((char*)password), tmpSsid((char*)tmpSsid),
                                 port(port), apAddress(apAddress), apMask(apMask) {
    if(strlen(password) < 8){
        ESP_LOGW(TAG, "The password must contain at least 8 characters");
    }
    WiFiTcpServer::maxConn = maxConnections;
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
}

void WiFiTcpServer::tcpServerTask(void *pvParameters) {
    WiFiTcpServerTaskParams taskParams = *(WiFiTcpServerTaskParams*)pvParameters;
    sockaddr_in destAddr{};
    //Change hostname to network byte order
    destAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    //Define address family as Ipv4
    destAddr.sin_family = AF_INET;
    //Define PORT
    destAddr.sin_port = htons(taskParams.port);
    while (true) {
        //Create TCP socket
        int socket_id = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (socket_id < 0) {
            ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Socket created");
        //Bind a socket to a specific IP + port
        if (bind(socket_id, (struct sockaddr *)&destAddr, sizeof(destAddr)) != 0) {
            ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Socket binded");

        //Begin listening for clients on socket
        if (listen(socket_id, 3) != 0) {
            ESP_LOGE(TAG, "Error occured during listen: errno %d", errno);
            break;
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
            taskParams.server->startClientSocket(client_socket);
        }
    }
    vTaskDelete(nullptr);
}

void WiFiTcpServer::clientSocketTask(void *pvParameters) {
    // char array to store received data
    char rx_buffer[128];
    // immediate bytes received
    int bytes_received;
    WiFiTcpServerTaskParams taskParams = *(WiFiTcpServerTaskParams*)pvParameters;
    while(true) {
        //Clear rx_buffer, and fill with zero's
        bzero(rx_buffer, sizeof(rx_buffer));
        OsDelay(500);
        while(true) {
            ESP_LOGI(TAG, "Waiting for data");
            bytes_received = recv(taskParams.client_socket, rx_buffer, sizeof(rx_buffer) - 1, 0);
            ESP_LOGI(TAG, "Received Data");
            //Error occured during receiving or connection closed
            if (bytes_received <= 0) {
                ESP_LOGI(TAG, "Connection closed");
                closesocket(taskParams.client_socket);
                vTaskDelete(nullptr);
                break;
            //Data received
            } else {
                //Null-terminate whatever we received and treat like a string
                rx_buffer[bytes_received] = 0;
                ESP_LOGI(TAG, "%s", rx_buffer);
                //After receive action
                taskParams.server->afterReceiveAction(taskParams.client_socket, (uint8_t *)rx_buffer, sizeof(rx_buffer));
                //Clear rx_buffer, and fill with zero's
                bzero(rx_buffer, sizeof(rx_buffer));
            }
        }
    }
    vTaskDelete(nullptr);
}

void WiFiTcpServer::startTCPserver() {
//    if(tcpServerTaskHandle != nullptr) return;
    WiFiTcpServerTaskParams params;
    params.port = port;
    params.server = this;

    if(xTaskCreate(tcpServerTask,
                   "tcp_server",
                   4096,
                   (void*)&params,
                   5,
                   &tcpServerTaskHandle) != pdPASS){
        ESP_LOGE(TAG, "TCP server task not created");
    }
}

void WiFiTcpServer::startClientSocket(int client_socket) {
    WiFiTcpServerTaskParams params;
    params.client_socket = client_socket;
    params.server = this;
    char taskName[10];
    sprintf(taskName, "cst_%d", client_socket);
    if(xTaskCreate(clientSocketTask,
                   taskName,
                   4096,
                   (void*)&params,
                   5,
                   &tcpServerTaskHandle) != pdPASS){
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
    switch (server->type) {
        case WIFI_AP_TYPE:
            server->startAP(false);
            break;
        case WIFI_STA_TYPE:
            server->startSTA();
            break;
        case WIFI_AP_STA_TYPE:
            break;
    }

    ESP_ERROR_CHECK(esp_wifi_start());
    server->started = true;
    vTaskDelete(nullptr);
}

void WiFiTcpServer::wifiEventHandler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    ESP_LOGI("Wifi event catch", "%ld", event_id);
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
        if(server->tcpServerStarted()){
            server->stopTCPserver();
        }
        if (staRetryNum < maxConn) {
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
    auto server = (WiFiTcpServer*)arg;
    ESP_LOGI("IP event catch", "%ld", event_id);
    //STA Events
    if (event_id == IP_EVENT_STA_GOT_IP) {
        auto event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "STA connected, got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        staRetryNum = 0;
        server->startTCPserver();
    } else if (event_id == IP_EVENT_AP_STAIPASSIGNED){
//        server->startTCPserver();
    }
}

void WiFiTcpServer::stopTCPserver() {
    vTaskDelete(tcpServerTaskHandle);
    tcpServerTaskHandle = nullptr;
}

bool WiFiTcpServer::tcpServerStarted() {
    return tcpServerTaskHandle != nullptr;
}

void WiFiTcpServer::startAP(bool staFail) {
    if(staFail) ESP_ERROR_CHECK(esp_wifi_stop());
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
    startTCPserver();
}

void WiFiTcpServer::startSTA() {

    esp_netif_create_default_wifi_sta();

    wifi_config_t wifi_config{};
    strcpy((char*) wifi_config.sta.ssid, ssid);
    strcpy((char*) wifi_config.sta.password, password);
    wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
//    wifi_config.sta.sae_pwe_h2e = WPA3_SAE_PWE_HUNT_AND_PECK;
//    strcpy((char*) wifi_config.sta.sae_h2e_identifier, "");

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));

    ESP_LOGI(TAG, "wifi_init_sta finished.");
}

void WiFiTcpServer::afterReceiveAction(int client_socket, uint8_t *receivedData, uint16_t len) {
    sendData(client_socket, receivedData, len);
}

void WiFiTcpServer::sendData(int client_socket, uint8_t *sendingData, uint16_t len) {
    send(client_socket, sendingData, len, 0);
}

void WiFiTcpServer::start() {
    if(started){
        ESP_LOGI(TAG, "WiFi server already started");
        return;
    }
    if(xTaskCreate(startServerTask,
                   "start_task",
                   4096,
                   (void*)this,
                   5,
                   nullptr) != pdPASS){
        ESP_LOGE(TAG, "start server task not created");
    }
}

WiFiType WiFiTcpServer::getType() {
    return type;
}
