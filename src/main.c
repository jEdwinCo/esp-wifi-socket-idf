/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#define TAG "main"
#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_log.h"

#include "wifi.h"
#include "socket_client.h"

void hello_task(void *pvParameter)
{
  while(1)
  {
    ESP_LOGI(TAG, "task tcp_conn.");
    /*wating for connecting to AP*/
    xEventGroupWaitBits(tcp_event_group, WIFI_CONNECTED_BIT, false, true, portMAX_DELAY);
    ESP_LOGI(TAG, "sta has connected to ap.");
    int socket_ret = ESP_FAIL;
    if (socket_ret == ESP_FAIL) {
        ESP_LOGI(TAG, "tcp_client will start after 20s...");
        vTaskDelay(20000 / portTICK_RATE_MS);
        ESP_LOGI(TAG, "create_tcp_client.");
        socket_ret = create_tcp_client();
    }

    if (socket_ret == ESP_FAIL) {
        ESP_LOGI(TAG, "create tcp socket error,stop.");
        continue;
    }

    TaskHandle_t tx_rx_task = NULL;
    if (tx_rx_task == NULL) {
        if (pdPASS != xTaskCreate(&send_data, "send_data", 4096, NULL, 4, &tx_rx_task)) {
            ESP_LOGE(TAG, "Send task create fail!");
        }
    }

    while(1)
    {
       vTaskDelay(3000 / portTICK_RATE_MS);//every 3s
      if (g_rxtx_need_restart) {
          printf("send or receive task encoutner error, need to restart\n");
          break;
      }
    }
    close_socket();
  }
  vTaskDelete(NULL);
}

void app_main()
{
    nvs_flash_init();
    wifi_init_sta();
    create_tcp_client();
    xTaskCreate(&hello_task, "hello_task", 2048, NULL, 5, NULL);
}
