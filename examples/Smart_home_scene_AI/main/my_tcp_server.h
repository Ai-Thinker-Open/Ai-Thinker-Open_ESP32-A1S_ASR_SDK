#ifndef __MY_TCP_SERVER__
#define __MY_TCP_SERVER__

typedef void (*pTcp_Fun)(int sock,uint16_t port,char * data,size_t len);
void set_udp_receive_cb(pTcp_Fun fun);
void tcp_server_task(void *pvParameters);
bool tcp_server_send(u16_t port, char *dat, size_t len);
#endif