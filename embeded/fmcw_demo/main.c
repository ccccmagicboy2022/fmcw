#include "sys.h"

s16 data_buf[512];
__attribute__ ((weak)) void detect_presense(void);
uint8_t Txbuffer[1024];

int32_t main(void)
{
    init_mem();
    mcu429_init();
    usb_hs_init();
    uart_init();
    radar_init();
    fpga_init();
    bt_protocol_init();

    frequency_calibration();
    
    Txbuffer[0] = 0xEB;
    
    while (1)
	{
		bt_uart_service();
        if (is_data_available()) {
            get_sample_data((u8 *)data_buf);
            detect_presense();
        }
        USB_OTG_BSP_mDelay(1);
        
        USB_SendData(Txbuffer, 16);
	}
}

__attribute__ ((weak)) void detect_presense(void)
{
    //pass
}
