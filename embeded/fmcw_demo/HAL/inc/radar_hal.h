#ifndef _B96F1637_7840_479F_B02D_701B368CF843_
#define _B96F1637_7840_479F_B02D_701B368CF843_

#include "sys.h"

#define ADC3_DR_ADDRESS        ((uint32_t)0x4001224C)
#define SPI4_CS_HIGH GPIO_SetBits(GPIOE, GPIO_Pin_4)
#define SPI4_CS_LOW GPIO_ResetBits(GPIOE, GPIO_Pin_4)

void radar_init(void);
void spi4_init(void);
void spi4_write_byte(uint8_t val);
void spi4_write_reg32(uint32_t word);
void adc_init(void);
void input_capture_init(void);
void dac_init(void);
void spi4_cs_init(void);
void dac_set_value(uint16_t data);
uint16_t dac_get_value(void);
void radar_spi_init(void);
void dac_set_vol(float vol);
void frequency_calibration(void);
uint8_t cal_once(float target_khz, uint16_t * dac_raw);
uint8_t check_once(float target_khz, uint16_t dac_raw_in, uint8_t index);
uint8_t pidExecu(float target_khz, float cap_khz);
void input_capture_disable(void);
void dac_first_deinit(void);
void dac_secend_init(void);
void dac_timer_config(void);
void dac_dma_config(void);
void frame_timer_init(void);
void start_dac_timer(void);
void stop_dac_timer(void);
float get_cap_rf_freq_at_khz(void);
float get_cap_divout2_at_khz(void);


#endif//_B96F1637_7840_479F_B02D_701B368CF843_
