#ifndef _B96F1637_7840_479F_B02D_701B368CF843_
#define _B96F1637_7840_479F_B02D_701B368CF843_

#include "sys.h"

#define DAC_DHR12R2_ADDRESS    0x40007414
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
void dac_first_set_value(uint32_t data);
void radar_spi_init(void);

#endif//_B96F1637_7840_479F_B02D_701B368CF843_
