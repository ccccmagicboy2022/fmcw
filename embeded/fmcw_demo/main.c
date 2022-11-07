#include "sys.h"

int32_t main(void)
{
    mcu429_init();
    uart_init();
    radar_init();
    usb_hs_init();
    fpga_init();
    bt_protocol_init();
    
    while (1)
	{
		bt_uart_service();
	}
}
