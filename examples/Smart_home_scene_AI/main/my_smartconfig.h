#ifndef __MY_SMARTCONFIG__
#define __MY_SMARTCONFIG__

#define CONNECTED_AP_BIT0       (1<<0)
#define ESPTOUCH_DONE_AP_BIT1   (1<<1)

EventGroupHandle_t s_wifi_event_group;

bool getIsSmartConfig();
void setIsSmartConfig(bool stu);
void sendEventGroupHandle(EventBits_t uxBitsToSet);
void smartconfig_example_task(void * parm);

#endif