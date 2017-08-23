#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#define TAG "socket_client:"

#include "socket_client.h"
#include <string.h>
#include <sys/socket.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
//#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"

#define EXAMPLE_DEFAULT_SERVER_IP "192.168.15.44"
#define EXAMPLE_DEFAULT_PORT      12001
#define EXAMPLE_DEFAULT_PKTSIZE   10
#define EXAMPLE_PACK_BYTE_IS      97 //'a'

static int connect_socket = 0;
static struct sockaddr_in server_addr;
int g_total_data = 0;
bool g_rxtx_need_restart = false;

int show_socket_error_reason(const char *str, int socket);
int get_socket_error_code(int socket);

//use this esp32 as a tcp client. return ESP_OK:success ESP_FAIL:error
esp_err_t create_tcp_client()
{
    ESP_LOGI(TAG, "client socket....serverip:port=%s:%d\n",
             EXAMPLE_DEFAULT_SERVER_IP, EXAMPLE_DEFAULT_PORT);

    connect_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (connect_socket < 0) {
        show_socket_error_reason("create client", connect_socket);
        return ESP_FAIL;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(EXAMPLE_DEFAULT_PORT);
    server_addr.sin_addr.s_addr = inet_addr(EXAMPLE_DEFAULT_SERVER_IP);
    ESP_LOGI(TAG, "connecting to server...");
    if (connect(connect_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        show_socket_error_reason("client connect", connect_socket);
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "connect to server success!");

    return ESP_OK;
}

//send data
void send_data(void *pvParameters)
{
    int len = 0;
    char *databuff = (char *)malloc(EXAMPLE_DEFAULT_PKTSIZE * sizeof(char));
    memset(databuff, EXAMPLE_PACK_BYTE_IS, EXAMPLE_DEFAULT_PKTSIZE);
    vTaskDelay(100 / portTICK_RATE_MS);
    ESP_LOGI(TAG, "start sending...");

    while (1) {
        int to_write = EXAMPLE_DEFAULT_PKTSIZE;
        //send function
        while (to_write > 0) {
            len = send(connect_socket, databuff + (EXAMPLE_DEFAULT_PKTSIZE - to_write), to_write, 0);
            if (len > 0) {
              g_total_data += len;
              to_write -= len;
            } else {
                int err = get_socket_error_code(connect_socket);
                if (err != ENOMEM) {
                    show_socket_error_reason("send_data", connect_socket);
                    break;
                }
            }
        }

        ESP_LOGI(TAG, "send packet ok");
        vTaskDelay(1000 / portTICK_RATE_MS);

        if (g_total_data > 0) {
          continue;
        } else {
            break;
        }
    }

    g_rxtx_need_restart = true;
    free(databuff);
    vTaskDelete(NULL);
}

int get_socket_error_code(int socket)
{
    int result;
    u32_t optlen = sizeof(int);
    int err = getsockopt(socket, SOL_SOCKET, SO_ERROR, &result, &optlen);
    if (err == -1) {
        ESP_LOGE(TAG, "getsockopt failed:%s", strerror(err));
        return -1;
    }
    return result;
}

int show_socket_error_reason(const char *str, int socket)
{
    int err = get_socket_error_code(socket);

    if (err != 0) {
        ESP_LOGW(TAG, "%s socket error %d %s", str, err, strerror(err));
    }

    return err;
}

void close_socket()
{
    close(connect_socket);
}
