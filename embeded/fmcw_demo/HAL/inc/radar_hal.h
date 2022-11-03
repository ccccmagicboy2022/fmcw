#ifndef _B96F1637_7840_479F_B02D_701B368CF843_
#define _B96F1637_7840_479F_B02D_701B368CF843_

#include "sys.h"

void radar_init(void);
void spi4_init(void);
void spi4_write_byte(uint8_t val);
void spi4_write_reg32(uint32_t word);
void adc_init(void);
void input_capture_init(void);
void dac_init(void);

#endif//_B96F1637_7840_479F_B02D_701B368CF843_
