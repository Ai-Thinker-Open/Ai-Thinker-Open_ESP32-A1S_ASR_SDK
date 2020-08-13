#include "system_lib.h"

#include "my_smartconfig.h"
#include "my_nvs.h"

EventGroupHandle_t s_wifi_event_group=pdFALSE;
static const char *TAG = "my_smartconfig";
static bool  isSmartConfig=false;


bool getIsSmartConfig(){
return isSmartConfig;
}

void setIsSmartConfig(bool stu){
    isSmartConfig=stu;
}

void sendEventGroupHandle(EventBits_t uxBitsToSet){
    if(s_wifi_event_group!=pdFALSE){
        xEventGroupSetBits(s_wifi_event_group, uxBitsToSet);
    }else{
        ESP_LOGI(TAG, "消息队列未被创建！！！");
    }
}

static void sc_callback(smartconfig_status_t status, void *pdata)
{
    switch (status)
    {
        case SC_STATUS_WAIT:
            ESP_LOGI(TAG, "SC_STATUS_WAIT");
            break;
        case SC_STATUS_FIND_CHANNEL:
            ESP_LOGI(TAG, "SC_STATUS_FINDING_CHANNEL");
            break;
        case SC_STATUS_GETTING_SSID_PSWD:
            ESP_LOGI(TAG, "SC_STATUS_GETTING_SSID_PSWD");
            break;
        case SC_STATUS_LINK:
            ESP_LOGI(TAG, "SC_STATUS_LINK");
            wifi_config_t *wifi_config = pdata;
            ESP_LOGI(TAG, "SSID:%s", wifi_config->sta.ssid);
            ESP_LOGI(TAG, "PASSWORD:%s", wifi_config->sta.password);

            nvs_write_blob("wifi_config",wifi_config,sizeof(wifi_config_t));

            ESP_ERROR_CHECK(esp_wifi_disconnect());
            ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, wifi_config));
            ESP_ERROR_CHECK(esp_wifi_connect());
            break;
        case SC_STATUS_LINK_OVER:
            ESP_LOGI(TAG, "SC_STATUS_LINK_OVER");
            if (pdata != NULL)
            {
                uint8_t phone_ip[4] = {0};
                memcpy(phone_ip, (uint8_t *)pdata, 4);
                ESP_LOGI(TAG, "Phone ip: %d.%d.%d.%d\n", phone_ip[0], phone_ip[1], phone_ip[2], phone_ip[3]);
            }
            xEventGroupSetBits(s_wifi_event_group, ESPTOUCH_DONE_AP_BIT1);
            break;
        default:
            break;
    }
}

void smartconfig_example_task(void *parm)
{

    EventBits_t uxBits;
    if(s_wifi_event_group==pdFALSE)
        s_wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_smartconfig_set_type(SC_TYPE_ESPTOUCH));
    ESP_ERROR_CHECK(esp_smartconfig_start(sc_callback));
    while (1)
    {
        uxBits = xEventGroupWaitBits(s_wifi_event_group, CONNECTED_AP_BIT0 | ESPTOUCH_DONE_AP_BIT1, true, false, portMAX_DELAY);
        xEventGroupClearBits(s_wifi_event_group,CONNECTED_AP_BIT0|ESPTOUCH_DONE_AP_BIT1);
        if (uxBits & CONNECTED_AP_BIT0)
        {
            ESP_LOGI(TAG, "WiFi Connected to ap");
        }
        else if (uxBits & ESPTOUCH_DONE_AP_BIT1)
        {
            ESP_LOGI(TAG, "smartconfig配网结束");
            esp_smartconfig_stop();
            vTaskDelete(NULL);
        }
    }
}
