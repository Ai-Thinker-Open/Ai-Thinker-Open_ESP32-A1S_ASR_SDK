#ifndef __MY_WIFI__
#define __MY_WIFI__

#define AP_SSID "k20_pro"
#define AP_PASSWORD "13973619466"

void wifiRst();
//esp_err_t event_handler(void *ctx, system_event_t *event);
void initWifiToAp();
void initWifiApMode();
#endif