#ifndef __MYUSART_H
#define __MYUSART_H

#ifdef O1_DATA_STREAM
#include "stdint.h"
#endif

extern void usart_init(void);

#ifdef O1_DATA_STREAM
void sample_usart_init(void);
void usart_polling_send_data(uint8_t *data, uint32_t lenth);
void usart_polling_send_data_no_head(uint8_t *data, uint32_t lenth);
void en_O1_data_stream(uint8_t en);
#endif

#endif
