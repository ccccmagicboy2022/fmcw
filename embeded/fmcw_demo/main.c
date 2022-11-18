#include "sys.h"

s16 data_buf[SAMPLE_NUM_PER_CHIRP * NUM_CHIRPS_PER_FRAME];
__attribute__ ((weak)) int detect_presense(s16 *data);

int32_t main(void)
{
    init_mem();
    mcu429_init();
    usb_hs_init();
    uart_init();
    radar_init();
    fpga_init();
    bt_protocol_init();
    
    while (1)
	{
		bt_uart_service();
        if (is_data_available()) {
            get_sample_data((u8 *)data_buf);
            detect_presense(data_buf);
        }
	}
}

__attribute__ ((weak)) int detect_presense(s16 *data)
{
    printf("algo run once!\r\n");
    USB_SendData((u8 *)data, 16);
    return 0;
}
