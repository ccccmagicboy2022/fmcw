#ifndef __TEST_USART_H__
#define __TEST_USART_H__
#include "stdint.h"

void usart_polling_send_data(uint8_t *buffer, uint32_t lenth);
void usart_polling_send_data_no_head(uint8_t *data, uint32_t lenth);
void test_usart_init(void);

#endif
