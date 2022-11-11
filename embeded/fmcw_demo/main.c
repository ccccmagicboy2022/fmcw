#include "sys.h"

s16 data_buf[512];
__attribute__ ((weak)) void detect_presense(void);
extern __ALIGN_BEGIN USB_OTG_CORE_HANDLE USB_OTG_dev __ALIGN_END;
uint8_t Txbuffer[64];

int32_t main(void)
{
    init_mem();
    mcu429_init();
    usb_hs_init();
    uart_init();
    radar_init();
    fpga_init();
    bt_protocol_init();
    
    Txbuffer[0] = 0xEB;
    
    while (1)
	{
		bt_uart_service();
        if (is_data_available()) {
            get_sample_data((u8 *)data_buf);
            detect_presense();
        }
        USB_OTG_BSP_mDelay(1);
        
        while (VCP_CheckDataSent() == 1);
        VCP_SendData(&USB_OTG_dev, Txbuffer, 1);
	}
}

__attribute__ ((weak)) void detect_presense(void)
{
    //pass
}
