#ifndef __SOCKET_CLIENT_H__
#define __SOCKET_CLIENT_H__

#include "esp_wifi.h"
//#include "esp_event_loop.h"
//#include "esp_log.h"

extern bool g_rxtx_need_restart;

esp_err_t create_tcp_client();
void send_data(void *pvParameters);
void close_socket();

#endif /**/
