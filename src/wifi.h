#ifndef __WIFI_H__
#define __WIFI_H__

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#define WIFI_CONNECTED_BIT BIT0

extern EventGroupHandle_t tcp_event_group;
//using esp as station
void wifi_init_sta();

#endif /**/
