#include "uart_hal.h"
#include "usart.h"
#include "sys.h"

void uart_init(void)
{
    usart1_init();
    test_uart1();
}

