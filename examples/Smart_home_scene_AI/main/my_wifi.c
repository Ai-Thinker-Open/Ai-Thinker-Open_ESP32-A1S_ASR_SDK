#include "system_lib.h"

#include "my_smartconfig.h"
#include "my_nvs.h"
// #include "my_mqtt_tcp.h"
// #include "my_tcp_client.h"
#include "my_tcp_server.h"

#include "my_wifi.h"

#define MAX_COUNT 5
bool initEnSmartConfig = false;
static int wifi_reconnection_count = 0;
static const char *TAG = "my_wifi";

void wifi_reconnection_task(void *parm);
void get_wifi_rssi_task(void *parm);

esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch (event->event_id)
    {
    case SYSTEM_EVENT_WIFI_READY:
        printf("ESP32 WiFi准备好了\n");
        break;
    case SYSTEM_EVENT_SCAN_DONE:
        printf("完成扫描AP\n");
        break;
    case SYSTEM_EVENT_STA_START:
        printf("ESP32工作站开始\n");
        if (initEnSmartConfig == true)
            xTaskCreate(smartconfig_example_task, "smartconfig_example_task", 4096, NULL, 3, NULL);
        break;
    case SYSTEM_EVENT_STA_STOP:
        printf("ESP32工作站停止\n");
        break;
    case SYSTEM_EVENT_STA_CONNECTED:
        printf("ESP32工作站连接到AP\n");
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED: //连接失败回调
        xTaskCreate(wifi_reconnection_task, "wifi_reconnection_task", 2048, NULL, 3, NULL);
        break;
    case SYSTEM_EVENT_STA_AUTHMODE_CHANGE:
        printf("改变了由ESP32工作站连接AP的auth模式\n");
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        printf("ESP32工作站从已连接的AP获得IP\n");
        wifi_reconnection_count = 0;
        //mqtt_app_start();
        //xTaskCreate(tcp_client_task, "tcp_client", 4096, NULL, 5, NULL);
        //xTaskCreate(get_wifi_rssi_task, "get_wifi_rssi_task", 4096, NULL, 5, NULL);
       
        break;
    case SYSTEM_EVENT_STA_LOST_IP:
        wifi_reconnection_count = 0;
        sendEventGroupHandle(ESPTOUCH_DONE_AP_BIT1);
        ESP_ERROR_CHECK(esp_wifi_connect());
        printf("ESP32工作站丢失IP, IP重置为0\n");
        break;
    case SYSTEM_EVENT_STA_WPS_ER_SUCCESS:
        printf("ESP32工作站wps成功在注册模式\n");
        break;
    case SYSTEM_EVENT_STA_WPS_ER_FAILED:
        printf("ESP32工作站wps在注册模式失败\n");
        break;
    case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT:
        printf("ESP32工作站wps超时在注册模式\n");
        break;
    case SYSTEM_EVENT_STA_WPS_ER_PIN:
        printf("ESP32工作站wps pin码在注册模式\n");
        break;
    case SYSTEM_EVENT_AP_START:
        printf("ESP32 soft-AP开始\n");
        break;
    case SYSTEM_EVENT_AP_STOP:
        printf("ESP32 soft-AP停止\n");
        break;
    case SYSTEM_EVENT_AP_STACONNECTED:
        printf("一个连接到ESP32软件ap的设备\n");
        break;
    case SYSTEM_EVENT_AP_STADISCONNECTED:
        printf("一个站与ESP32软件ap断开连接\n");
        break;
    case SYSTEM_EVENT_AP_STAIPASSIGNED:
        printf("ESP32软ap分配一个IP到一个连接的站\n");
        break;
    case SYSTEM_EVENT_AP_PROBEREQRECVED:
        printf("接收软ap接口中的探测请求包\n");
        break;
    case SYSTEM_EVENT_GOT_IP6:
        printf(" ESP32工作站或ap或以太网接口v6IP地址优先\n");
        break;
    case SYSTEM_EVENT_ETH_START:
        printf("ESP32以太网开始\n");
        break;
    case SYSTEM_EVENT_ETH_STOP:
        printf("ESP32以太网停止\n");
        break;
    case SYSTEM_EVENT_ETH_CONNECTED:
        printf("ESP32以太网物理层连接\n");
        break;
    case SYSTEM_EVENT_ETH_DISCONNECTED:
        printf("ESP32以太网物理层向下链接\n");
        break;
    case SYSTEM_EVENT_ETH_GOT_IP:
        printf(" ESP32以太网从连接的AP获得IP\n");
        break;

    default:
        printf("未知回调event->event_id：%d\n", event->event_id);
        break;
    }
    return ESP_OK;
}

void wifiRst()
{
    wifi_config_t *wifi_config = malloc(sizeof(wifi_config_t));
    strcpy((char *)wifi_config->sta.ssid, "");
    strcpy((char *)wifi_config->sta.password, "");
    nvs_write_blob("wifi_config", wifi_config, sizeof(wifi_config_t));
    printf("正在复位。。。\n");
    vTaskDelay(3000 / portTICK_PERIOD_MS);
    esp_restart(); //软件复位
}

void initWifiToAp()
{
    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    //ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    wifi_config_t *sta_config = malloc(sizeof(wifi_config_t));
    nvs_read_blob("wifi_config", sta_config);
    if (strlen((char *)sta_config->sta.ssid) > 0 && strlen((char *)sta_config->sta.password) > 0)
    {
        printf("SSID:%s\n", sta_config->sta.ssid);
        printf("PASSWORD:%s\n", sta_config->sta.password);
        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, sta_config));
        initEnSmartConfig = false;
    }
    else
    {
        printf("未配置AP！！！\n");
        printf("SSID:%s\nlen:%d\n", sta_config->sta.ssid, strlen((char *)sta_config->sta.ssid));
        initEnSmartConfig = true;
    }
    ESP_ERROR_CHECK(esp_wifi_start());
    if (strlen((char *)sta_config->sta.ssid) > 0 && strlen((char *)sta_config->sta.password) > 0)
        ESP_ERROR_CHECK(esp_wifi_connect());
}

void initWifiApMode()
{
    // esp_wifi_set_mode(WIFI_MODE_AP);
    // tcpip_adapter_ip_info_t tcpip_adapter_ip_info;

    // esp_wifi_stop();

    // IP4_ADDR(&tcpip_adapter_ip_info.ip, 192, 168, 137, 1);
    // IP4_ADDR(&tcpip_adapter_ip_info.gw, 192, 168, 137, 1);
    // IP4_ADDR(&tcpip_adapter_ip_info.netmask, 255, 255, 255, 0);
    // tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_AP, &tcpip_adapter_ip_info);
    tcpip_adapter_init();
#define MY_SSID "r91"
#define MY_PASSWORD "13973619466"

    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    wifi_config_t wifi_config = {
        .ap = {
            .ssid = MY_SSID,
            .ssid_len = strlen(MY_SSID),
            .password = MY_PASSWORD,
            .max_connection = 4,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK},
    };
    if (strlen(MY_PASSWORD) == 0)
    {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_softap finished.SSID:%s password:%s",
             MY_SSID, MY_PASSWORD);
}

void wifi_reconnection_task(void *parm)
{
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    wifi_reconnection_count++;
    if (wifi_reconnection_count > MAX_COUNT)
    {
        ESP_ERROR_CHECK(esp_wifi_disconnect());
        xTaskCreate(smartconfig_example_task, "smartconfig_example_task", 4096, NULL, 3, NULL);
        printf("等待连接失败，尝试重新配网\n");
        wifi_reconnection_count = 0;
    }
    else
    {
        ESP_ERROR_CHECK(esp_wifi_connect()); //再次连接
        printf("ESP32工作站与AP断开连接%d次\n", wifi_reconnection_count);
    }
    vTaskDelete(NULL);
}

void get_wifi_rssi_task(void *parm)
{
    float rssi_average = 0.0f;
    wifi_ap_record_t ap_info;
    for (;;)
    {
        // esp_wifi_get_max_tx_power(&power);
        // printf("%ddBm\n",power);
        esp_wifi_sta_get_ap_info(&ap_info);
        //printf("RSSI=%d\n",ap_info.rssi);
        if (ap_info.rssi != 31)
        {

            if (rssi_average)
            {
                rssi_average += ap_info.rssi;
                rssi_average /= 2;
                printf("RSSI=%d---rssi_average=%0.1f\n", ap_info.rssi, rssi_average + 0.05f);
            }
            else
            {
                rssi_average += ap_info.rssi;
                printf("RSSI=%d---rssi_average=%0.1f\n", ap_info.rssi, rssi_average + 0.05f);
            }
        }
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}