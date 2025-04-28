#include <lwip/sockets.h>
#include <dhcpserver/dhcpserver.h>
#include <dhcpserver/dhcpserver_options.h>
#include "wifi_ap_tcp_server.h"


WiFiApTcpServer::WiFiApTcpServer(const char *ssid, const char *password, uint16_t port,
                                 IPv4addr apAddress, IPv4addr apMask, uint8_t maxConnections)
                                 : ssid((char*) ssid), password((char*)password), port(port),
                                   apAddress(apAddress), apMask(apMask), maxConnections(maxConnections) {
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
}

void WiFiApTcpServer::wifiEventhandler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        auto event = (wifi_event_ap_staconnected_t*) event_data;

        ESP_LOGI(TAG, "station " MACSTR" join, AID=%d",
                 MAC2STR(event->mac), event->aid);
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        auto event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(TAG, "station " MACSTR" leave, AID=%d, reason=%d",
                 MAC2STR(event->mac), event->aid, event->reason);
    }
}

void WiFiApTcpServer::start() {
    ESP_LOGI(TAG, "ESP_WIFI_MODE_AP");
    //Initialize WiFi
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_t *netif = esp_netif_create_default_wifi_ap();

    //Configure AP IP
    ESP_ERROR_CHECK(esp_netif_dhcps_stop(netif));
    esp_netif_ip_info_t ip_info;
    IP4_ADDR(&ip_info.ip, apAddress.oct1, apAddress.oct2, apAddress.oct3, apAddress.oct4);
    IP4_ADDR(&ip_info.gw, apAddress.oct1, apAddress.oct2, apAddress.oct3, apAddress.oct4);
    IP4_ADDR(&ip_info.netmask, apMask.oct1, apMask.oct2, apMask.oct3, apMask.oct4);
    ESP_ERROR_CHECK(esp_netif_set_ip_info(netif, &ip_info));
    //Configure DHCP pool
//    dhcps_lease_t dhcps_poll;
//    dhcps_poll.enable = true;
//    IP4_ADDR(&dhcps_poll.start_ip, dhcpPoolBegin.oct1, dhcpPoolBegin.oct2, dhcpPoolBegin.oct3, dhcpPoolBegin.oct4);
//    IP4_ADDR(&dhcps_poll.end_ip, dhcpPoolEnd.oct1, dhcpPoolEnd.oct2, dhcpPoolEnd.oct3, dhcpPoolEnd.oct4);
//    esp_netif_dhcps_option(netif, ESP_NETIF_OP_SET, ESP_NETIF_REQUESTED_IP_ADDRESS,
//                           &dhcps_poll, sizeof(dhcps_poll));
    esp_netif_dhcps_start(netif);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifiEventhandler,
                                                        nullptr,
                                                        nullptr));

    wifi_config_t wifi_config = {};
    strcpy((char *)wifi_config.ap.ssid, ssid);
    strcpy((char *)wifi_config.ap.password, password);
    wifi_config.ap.ssid_len = strlen(ssid);
    wifi_config.ap.channel = 6;
    wifi_config.ap.authmode = WIFI_AUTH_WPA2_PSK;
    wifi_config.ap.max_connection = maxConnections;

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_LOGI(TAG, "wifi_init_softap finished.");
    ESP_ERROR_CHECK(esp_wifi_start());


    WiFiApTcpServerTaskParams params;
    params.port = port;
    params.wiFiApTcpServer = this;

    if(xTaskCreate(tcpServerTask,
                   "tcp_server",
                   4096,
                   (void*)&params,
                   5,
                   nullptr) != pdPASS){
        ESP_LOGI(TAG, "TCP server task not created");
    }

}

void WiFiApTcpServer::tcpServerTask(void *pvParameters) {
    char rx_buffer[128];	// char array to store received data
    int bytes_received;		// immediate bytes received
    WiFiApTcpServerTaskParams taskParams = *(WiFiApTcpServerTaskParams*)pvParameters;
    while (1) {
        struct sockaddr_in destAddr;
        destAddr.sin_addr.s_addr = htonl(INADDR_ANY); //Change hostname to network byte order
        destAddr.sin_family = AF_INET;		//Define address family as Ipv4
        destAddr.sin_port = htons(taskParams.port); 	//Define PORT

        /* Create TCP socket*/
        int socket_id = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (socket_id < 0) {
            ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Socket created");

        /* Bind a socket to a specific IP + port */
        if (bind(socket_id, (struct sockaddr *)&destAddr, sizeof(destAddr)) != 0) {
            ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Socket binded");

        /* Begin listening for clients on socket */
        if (listen(socket_id, 3) != 0) {
            ESP_LOGE(TAG, "Error occured during listen: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Socket listening");

        while (1) {
            struct sockaddr_in sourceAddr; // Large enough for IPv4
            uint addrLen = sizeof(sourceAddr);
            /* Accept connection to incoming client */
            int client_socket = accept(socket_id, (struct sockaddr *)&sourceAddr, (socklen_t *)(&addrLen));
            if (client_socket < 0) {
                ESP_LOGE(TAG, "Unable to accept connection: errno %d", errno);
                break;
            }
            ESP_LOGI(TAG, "Socket accepted");
            // Clear rx_buffer, and fill with zero's
            bzero(rx_buffer, sizeof(rx_buffer));
            vTaskDelay(500 / portTICK_PERIOD_MS);
            while(1) {
                ESP_LOGI(TAG, "Waiting for data");
                bytes_received = recv(client_socket, rx_buffer, sizeof(rx_buffer) - 1, 0);
                ESP_LOGI(TAG, "Received Data");
                // Error occured during receiving
                if (bytes_received < 0) {
                    ESP_LOGI(TAG, "Waiting for data");
                    vTaskDelay(100 / portTICK_PERIOD_MS);
                // Connection closed
                } else if (bytes_received == 0) {
                    ESP_LOGI(TAG, "Connection closed");
                    break;
                // Data received
                } else {
                    rx_buffer[bytes_received] = 0; // Null-terminate whatever we received and treat like a string
                    ESP_LOGI(TAG, "%s", rx_buffer);
                    //Send back
                    taskParams.wiFiApTcpServer->afterReceiveAction(client_socket, (uint8_t *)rx_buffer, sizeof(rx_buffer));
                    // Clear rx_buffer, and fill with zero's
                    bzero(rx_buffer, sizeof(rx_buffer));
                }
            }
        }
    }
    vTaskDelete(nullptr);
}

void WiFiApTcpServer::afterReceiveAction(int client_socket, uint8_t *receivedData, uint16_t len) {
    send(client_socket, receivedData, len, 0);
}
