/* 
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "system_lib.h"

#include "my_wifi.h"
#include "my_nvs.h"
#include "my_smartconfig.h"
#include "my_tcp_server.h"

#include "residual.h"
#include "household_food.h"
#include "hazardous.h"
#include "recyclable.h"
#include "wake_up_prompt_tone.h"
#include "speech_commands_action.h"

#define LED_GPIO 22

static const char *TAG = "main";

void led_init(int gpio)
{
    gpio_config_t io_conf;
    io_conf.intr_type = (gpio_int_type_t)GPIO_PIN_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pull_up_en = (gpio_pullup_t)1;

    uint64_t test = ((uint64_t)1 << gpio);
    io_conf.pin_bit_mask = test;
    gpio_config(&io_conf);
#ifdef CONFIG_AI_ESP32_AUDIO_KIT_V2_2_BOARD
    gpio_set_level(gpio, true);
#else
    gpio_set_level(gpio, false);
#endif
}

void led_on(int gpio)
{
#ifdef CONFIG_AI_ESP32_AUDIO_KIT_V2_2_BOARD
    gpio_set_level(gpio, false);
#else
    gpio_set_level(gpio, true);
#endif
}

void led_off(int gpio)
{
#ifdef CONFIG_AI_ESP32_AUDIO_KIT_V2_2_BOARD
    gpio_set_level(gpio, true);
#else
    gpio_set_level(gpio, false);
#endif
}
typedef struct
{
    char *name;
    const uint16_t *data;
    int length;
} dac_audio_item_t;

dac_audio_item_t playlist[] = {
    {"residual", residual, sizeof(residual)},
    {"household_food", household_food, sizeof(household_food)},
    {"hazardous", hazardous, sizeof(hazardous)},
    {"recyclable", recyclable, sizeof(recyclable)},
    {"wake_up_prompt_tone", wake_up_prompt_tone, sizeof(wake_up_prompt_tone)},
};

esp_err_t iot_dac_audio_play(const uint16_t *data, int length, TickType_t ticks_to_wait)
{
    size_t bytes_write = 0;
#ifdef CONFIG_ESP_LYRAT_V4_3_BOARD
    uint16_t *data_out = malloc(length * 2);
    for (int i = 0; i < length / 2; i++)
    {
        data_out[2 * i] = data[i];
        data_out[2 * i + 1] = data[i];
    }
    i2s_write(0, (const char *)data_out, length * 2, &bytes_write, ticks_to_wait);
    free(data_out);
#elif defined CONFIG_ESP_LYRAT_MINI_V1_1_BOARD
    i2s_write(0, (const char *)data, length, &bytes_write, ticks_to_wait);
#endif
    i2s_zero_dma_buffer(I2S_NUM_0);
    return ESP_OK;
}

// WakeNet
static const esp_wn_iface_t *wakenet = &WAKENET_MODEL;
static const model_coeff_getter_t *model_coeff_getter = &WAKENET_COEFF;

// MultiNet
static const esp_mn_iface_t *multinet = &MULTINET_MODEL;
model_iface_data_t *model_data_mn = NULL;

struct RingBuf *aec_rb = NULL;
struct RingBuf *rec_rb = NULL;

void wakenetTask(void *arg)
{
    model_iface_data_t *model_data = arg;
    int frequency = wakenet->get_samp_rate(model_data);
    int audio_chunksize = wakenet->get_samp_chunksize(model_data);
    int chunk_num = multinet->get_samp_chunknum(model_data_mn);
    printf("chunk_num = %d\n", chunk_num);
    int16_t *buffer = malloc(audio_chunksize * sizeof(int16_t));
    assert(buffer);
    int chunks = 0;
    int mn_chunks = 0;
    bool detect_flag = 0;

    while (1)
    {
        rb_read(rec_rb, (uint8_t *)buffer, audio_chunksize * sizeof(int16_t), portMAX_DELAY);
        if (detect_flag == 0)
        {
            int r = wakenet->detect(model_data, buffer);
            if (r)
            {
                float ms = (chunks * audio_chunksize * 1000.0) / frequency;
                printf("%.2f: %s DETECTED.\n", (float)ms / 1000.0, wakenet->get_word_name(model_data, r));
                detect_flag = 1;
                // iot_dac_audio_play(playlist[4].data, playlist[4].length, portMAX_DELAY);
                // vTaskDelay(1000 / portTICK_PERIOD_MS);
                printf("-----------------LISTENING-----------------\n\n");
                led_on(LED_GPIO);
            }
        }
        else
        {
            int command_id = multinet->detect(model_data_mn, buffer);
            mn_chunks++;
            if (mn_chunks == chunk_num || command_id > -1)
            {
                mn_chunks = 0;
                detect_flag = 0;
                if (command_id > -1)
                {
                    if (speech_commands_action(command_id) == false)
                    {
                        printf("illegal command : %d.\n", command_id);
                    }
                }
                else
                {
                    printf("can not recognize any speech commands\n");
                }

                printf("\n-----------awaits to be waken up-----------\n");
                led_off(LED_GPIO);
            }
        }
        chunks++;
    }
    vTaskDelete(NULL);
}

void self_testing_in_Task(void *arg);
void self_testing_out_Task(void *arg);

void app_main()
{
    init_nvs();

    led_init(LED_GPIO);

    ESP_LOGI(TAG, "project version :%s", system_get_sdk_version());

    codec_init();
    aec_rb = rb_init(BUFFER_PROCESS, 8 * 1024, 1, NULL);
    rec_rb = rb_init(BUFFER_PROCESS, 8 * 1024, 1, NULL);

    model_iface_data_t *model_data = wakenet->create(model_coeff_getter, DET_MODE_90);
    model_data_mn = multinet->create(&MULTINET_COEFF, 6000);

    xTaskCreatePinnedToCore(&recsrcTask, "rec", 2 * 1024, NULL, 8, NULL, 1);
    xTaskCreatePinnedToCore(&agcTask, "agc", 2 * 1024, NULL, 8, NULL, 1);
    xTaskCreatePinnedToCore(&wakenetTask, "wakenet", 2 * 1024, (void *)model_data, 5, NULL, 1);

    printf("-----------awaits to be waken up-----------\n");

    set_udp_receive_cb(tcp_callback);

    initWifiApMode();
    vTaskDelay(1500 / portTICK_PERIOD_MS);
    TaskHandle_t pvCreatedTask_tcp_server50000, pvCreatedTask_tcp_server50001, pvCreatedTask_tcp_server50002, pvCreatedTask_tcp_server50003;
    xTaskCreate(tcp_server_task, "tcp_server50000", 1024 * 4, 50000, 1, &pvCreatedTask_tcp_server50000);
    xTaskCreate(tcp_server_task, "tcp_server50001", 1024 * 4, 50001, 1, &pvCreatedTask_tcp_server50001);
    xTaskCreate(tcp_server_task, "tcp_server50002", 1024 * 4, 50002, 1, &pvCreatedTask_tcp_server50002);
    xTaskCreate(tcp_server_task, "tcp_server50003", 1024 * 4, 50003, 1, &pvCreatedTask_tcp_server50003);
}