#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/i2s.h"
#include "nvs_flash.h"
#include "sdkconfig.h"

#include "AC101_DRV.h"
#include "recoder.h"
#include "uart.h"

#include "AC101_CFG.h"

#define CONFIG_AC101_I2S_DATA_IN_PIN 35
#define CONFIG_AC101_I2S_LRCK_PIN 26
#define CONFIG_AC101_I2S_BCK_PIN 27
#define CONFIG_AC101_I2S_DATA_PIN 25

void audio_recorder_AC101_init_16KHZ_16BIT_1CHANNEL()
{
	AC101_init_16KHZ_16BIT_1CHANNEL();

	i2s_config_t i2s_config = {
		.mode = I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_TX,
		.sample_rate = 16000,
		.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
		.channel_format = I2S_CHANNEL_FMT_ONLY_LEFT, //1-channels
		.communication_format = I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB,
		.dma_buf_count = 32,
		.dma_buf_len = 32 * 2,
		.intr_alloc_flags = ESP_INTR_FLAG_LEVEL1 //Interrupt level 1
	};

	i2s_pin_config_t pin_config_rx = {
		.bck_io_num = CONFIG_AC101_I2S_BCK_PIN,
		.ws_io_num = CONFIG_AC101_I2S_LRCK_PIN,
		.data_out_num = CONFIG_AC101_I2S_DATA_PIN,
		.data_in_num = CONFIG_AC101_I2S_DATA_IN_PIN};

	int reg_val = REG_READ(PIN_CTRL);
	REG_WRITE(PIN_CTRL, 0xFFFFFFF0);
	reg_val = REG_READ(PIN_CTRL);
	PIN_FUNC_SELECT(GPIO_PIN_REG_0, 1); //GPIO0 as CLK_OUT1

	/* 注册i2s设备驱动 */
	i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
	/* 设置i2s引脚 */
	i2s_set_pin(I2S_NUM_0, &pin_config_rx);
	/* 停止i2s设备 */
	i2s_stop(I2S_NUM_0);
}

void audio_recorder_AC101_init_44KHZ_16BIT_2CHANNEL() // I2S_MODE_TX
{
	AC101_init_44KHZ_16BIT_2CHANNEL(); // 寄存器未配置

	i2s_config_t i2s_config = {
		.mode = I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_TX,
		.sample_rate = 44100,
		.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
		.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT, //2-channels
		.communication_format = I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB,
		.dma_buf_count = 32,
		.dma_buf_len = 32 * 2,
		.intr_alloc_flags = ESP_INTR_FLAG_LEVEL1 //Interrupt level 1
	};

	i2s_pin_config_t pin_config_rx = {
		.bck_io_num = CONFIG_AC101_I2S_BCK_PIN,
		.ws_io_num = CONFIG_AC101_I2S_LRCK_PIN,
		.data_out_num = CONFIG_AC101_I2S_DATA_PIN,
		.data_in_num = CONFIG_AC101_I2S_DATA_IN_PIN};

	int reg_val = REG_READ(PIN_CTRL);
	REG_WRITE(PIN_CTRL, 0xFFFFFFF0);
	reg_val = REG_READ(PIN_CTRL);
	PIN_FUNC_SELECT(GPIO_PIN_REG_0, 1); //GPIO0 as CLK_OUT1

	/* 注册i2s设备驱动 */
	i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
	/* 设置i2s引脚 */
	i2s_set_pin(I2S_NUM_0, &pin_config_rx);
	/* 停止i2s设备 */
	i2s_stop(I2S_NUM_0);
}
