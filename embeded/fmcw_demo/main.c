#include "sys.h"

s16 data_buf[512];
__attribute__ ((weak)) void detect_presense(void);

int32_t main(void)
{
    init_mem();
    mcu429_init();
    uart_init();
    radar_init();
    usb_hs_init();
    fpga_init();
    bt_protocol_init();
    
    while (1)
	{
		bt_uart_service();
        if (is_data_available()) {
            get_sample_data((u8 *)data_buf);
            detect_presense();
        }
	}
}

__attribute__ ((weak)) void detect_presense(void)
{
    //pass
}
