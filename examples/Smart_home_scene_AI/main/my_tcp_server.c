#include "system_lib.h"

#include "my_tcp_server.h"

static const char *TAG = "my_tcp_server";
static pTcp_Fun Fun = NULL;
static int socks[4] = {-1, -1, -1, -1};

void set_udp_receive_cb(pTcp_Fun fun)
{
    Fun = fun;
}

bool tcp_server_send(u16_t port, char *dat, size_t len)
{
    if (dat == pdFALSE && len <= 0 && port < 0)
    {
        ESP_LOGE(TAG, "dat == pdFALSE && len <= 0");
        return false;
    }
    if (port == 0)
    {
        for (size_t i = 0; i < 4; i++)
        {
            if (socks[i] < 0)
            {
                ESP_LOGE(TAG, "socks[%d]<0", i);
                continue;
            }
            int err = send(socks[i], dat, len, 0);
            if (err < 0)
            {
                ESP_LOGE(TAG, "Error occured during sending: errno %d", errno);
                continue;
            }
        }
    }
    else


    
    {
        if (socks[port % 10] < 0)
        {
            ESP_LOGE(TAG, "socks[%d]<0", port % 10);
            return false;
        }
        int err = send(socks[port % 10], dat, len, 0);
        if (err < 0)
        {
            ESP_LOGE(TAG, "Error occured during sending: errno %d", errno);
            return false;
        }
    }

    return true;
}

void tcp_server_task(void *pvParameters)
{
    char rx_buffer[128];
    char addr_str[16];
    if (pvParameters == NULL)
    {
        ESP_LOGI(TAG, "pvParameters==NULL Task END");
        return;
    }

    if ((u16_t)pvParameters < 0 || (u16_t)pvParameters > 65535)
    {
        ESP_LOGI(TAG, "(u16_t)pvParameters<0||(u16_t)pvParameters>65535 Task END");
        return;
    }

    u16_t port = (u16_t)pvParameters;
    while (1)
    {
        struct sockaddr_in destAddr;
        destAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        destAddr.sin_family = AF_INET;
        destAddr.sin_port = htons(port);
        inet_ntoa_r(destAddr.sin_addr, addr_str, sizeof(addr_str) - 1);

        int listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
        if (listen_sock < 0)
        {
            ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Socket created");

        int err = bind(listen_sock, (struct sockaddr *)&destAddr, sizeof(destAddr));
        if (err != 0)
        {
            ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Socket binded");

        err = listen(listen_sock, 1);
        if (err != 0)
        {
            ESP_LOGE(TAG, "Error occured during listen: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Socket listening");

        while (1)
        {
            struct sockaddr_in6 sourceAddr; // Large enough for both IPv4 or IPv6
            u16_t addrLen = sizeof(sourceAddr);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            int sock = accept(listen_sock, (struct sockaddr *)&sourceAddr, &addrLen);
            if (sock < 0)
            {
                ESP_LOGE(TAG, "Unable to accept connection: errno %d", errno);
                //vTaskDelay(3000 / portTICK_PERIOD_MS);
                continue;
            }
            ESP_LOGI(TAG, "Socket accepted");

            int err = send(sock, "Hello_TCP_Client", strlen("Hello_TCP_Client"), 0);
            if (err < 0)
            {
                ESP_LOGE(TAG, "Error occured during sending: errno %d", errno);
                continue;
            }
            ESP_LOGI(TAG, "Socket send");

            while (1)
            {
                socks[port % 10] = sock;
                int len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
                // Error occured during receiving
                if (len < 0)
                {
                    ESP_LOGE(TAG, "recv failed: errno %d", errno);
                    break;
                }
                // Connection closed
                else if (len == 0)
                {
                    ESP_LOGI(TAG, "Connection closed");
                    break;
                }
                // Data received
                else
                {
                    //Get the sender's ip address as string
                    if (sourceAddr.sin6_family == PF_INET)
                    {
                        inet_ntoa_r(((struct sockaddr_in *)&sourceAddr)->sin_addr.s_addr, addr_str, sizeof(addr_str) - 1);

                        ESP_LOGI(TAG, "Received %d bytes from %s:", len, addr_str);
                    }

                    rx_buffer[len] = 0;
                    if (Fun != NULL)
                        Fun(sock, port, rx_buffer, len);
                    //ESP_LOGI(TAG, "%s", rx_buffer);
                }
            }

            if (sock != -1)
            {
                ESP_LOGE(TAG, "Shutting down socket and restarting...");
                shutdown(sock, 0);
                close(sock);
                sock = -1;
                socks[port % 10] = sock;
            }
        }
    }

    vTaskDelete(NULL);
}