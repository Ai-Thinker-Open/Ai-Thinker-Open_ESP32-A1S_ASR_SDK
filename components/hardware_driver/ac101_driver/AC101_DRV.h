/*
 * AC101.h
 *
 *  Created on: 2018年1月9日
 *      Author: ai-thinker
 */

#ifndef MAIN_AC101_H_
#define MAIN_AC101_H_

#include "stdint.h"
#include "../../../esp-idf/components/driver/include/driver/i2s.h"
#define AC101_ADDR	0x1a   //0011010
#define PA_EN_PIN   21
#define PA_EN_PIN_SEL      (1ULL<<PA_EN_PIN)

/*
 * @AC101 I2C Init
 * @return
 *     - ESP_OK Success
 *     - ESP_FAIL Fail
 * */
esp_err_t AC101_i2c_master_init();

void I2C_init();
esp_err_t AC101_init_16KHZ_16BIT_1CHANNEL();
esp_err_t AC101_init_44KHZ_16BIT_2CHANNEL();
void mic_init(void);
uint16_t AC101_read_Reg(uint8_t reg) ;
esp_err_t AC101_Write_Reg(uint8_t reg, uint16_t val);

/*
 * @Please Input Volume Between 0 and 100
 * */
esp_err_t ac101_set_spk_volume(uint8_t volume);
/*
 * @Init Gpio PA 1:enable 0:disable
 * */
esp_err_t Init_Gpio_PA(int EN);

/*
 * @BYPASS 1:enable 0:disable
 * */
esp_err_t EN_PA(int EN);
/*
 * @EN 1:mute 0:on
 * */
esp_err_t Codec_Mute(int EN);

esp_err_t SET_AudioFormats(i2s_port_t i2s_num, uint32_t rate, i2s_bits_per_sample_t bits, i2s_channel_t ch);


#endif /* MAIN_AC101_H_ */
