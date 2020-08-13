/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "system_lib.h"

#include "my_tcp_server.h"
#include "cJSON.h"
#include "speech_commands_action.h"

static const char *TAG = "speech_commands_action";
//卧室port=50000
//走廊port=50001
//客厅port=50002
//书房port=50003

void tcp_callback(int sock, uint16_t port, char *data, size_t len)
{
    //printf("data:%s\n", data);
    ESP_LOGI(TAG, "port=%d,sock=%d", port, sock);
    switch (port)
    {
    case 50000: //客厅port=50000

        break;
    case 50001: //卧室port=50001

        break;
    case 50002: //厨房port=50002

        break;
    case 50003: //走廊port=50003

        break;

    default:
        break;
    }
}

void open_light(u16_t port)
{
    tcp_server_send(port, "{\"status\":true}", strlen("{\"status\":true}"));
}

void close_light(u16_t port)
{
    tcp_server_send(port, "{\"status\":false}", strlen("{\"status\":false}"));
}

bool speech_commands_action(int command_id)
{
    printf("Commands ID: %d.\n", command_id);
    switch (command_id)
    {
    // case 0:
    //     printf("打开一号灯\n");
    //     tcp_server_send(50000,"打开一号灯",strlen("打开一号灯"));
    //     break;
    // case 1:
    //     printf("打开二号灯\n");
    //     tcp_server_send(50000,"打开二号灯",strlen("打开二号灯"));
    //     break;
    // case 2:
    //     printf("打开三号灯\n");
    //     tcp_server_send(50000,"打开三号灯",strlen("打开三号灯"));
    //     break;
    // case 3:
    //     printf("打开四号灯\n");
    //     tcp_server_send(50000,"打开四号灯",strlen("打开四号灯"));
    //     break;
    // case 4:
    //     printf("打开五号灯\n");
    //     tcp_server_send(50000,"打开五号灯",strlen("打开五号灯"));
    //     break;
    case 5:
        printf("打开客厅的灯\n");
        open_light(50000);
        break;
    case 6:
        printf("关闭客厅的灯\n");
        close_light(50000);
        break;
    case 7:
        printf("打开卧室的灯\n");
        open_light(50001);
        break;
    case 8:
        printf("关闭卧室的灯\n");
        close_light(50001);
        break;
    case 9:
        printf("打开厨房的灯\n");
        open_light(50002);
        break;
    case 10:
        printf("关闭厨房的灯\n");
        close_light(50002);
        break;
    case 11:
        printf("打开走廊的灯\n");
        open_light(50003);
        break;
    case 12:
        printf("关闭走廊的灯\n");
        close_light(50003);
        break;
        // case 13:
        //     printf("打开厕所的灯\n");
        //     tcp_server_send(50000, "打开厕所的灯", strlen("打开厕所的灯"));
        //     break;
        // case 14:
        //     printf("关闭厕所的灯\n");
        //     tcp_server_send(50000, "关闭厕所的灯", strlen("关闭厕所的灯"));
        //     break;
        // case 15:
        //     printf("打开卫生间的灯\n");
        //     tcp_server_send(50000, "打开卫生间的灯", strlen("打开卫生间的灯"));
        //     break;
        // case 16:
        //     printf("关闭卫生间的灯\n");
        //     tcp_server_send(50000, "关闭卫生间的灯", strlen("关闭卫生间的灯"));
        //     break;
    case 17:
    case 19:
        printf("全部打开\n");
        open_light(50000);
        vTaskDelay(50 / portTICK_PERIOD_MS);
        open_light(50001);
        vTaskDelay(50 / portTICK_PERIOD_MS);
        open_light(50002);
        vTaskDelay(50 / portTICK_PERIOD_MS);
        open_light(50003);

        break;
    case 18:
    case 20:
        printf("全部关闭\n");
        close_light(50000);
        vTaskDelay(50 / portTICK_PERIOD_MS);
        close_light(50001);
        vTaskDelay(50 / portTICK_PERIOD_MS);
        close_light(50002);
        vTaskDelay(50 / portTICK_PERIOD_MS);
        close_light(50003);
        break;

    default:
        return false;
        break;
    }
    return true;
}