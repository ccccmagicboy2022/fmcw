#include "sys.h"

s16 data_buf[SAMPLE_NUM_PER_CHIRP * NUM_CHIRPS_PER_FRAME] __attribute__ ((aligned (4)));
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

void usb_polling_send_data(uint8_t *data, uint32_t lenth)
{
    u8 header[2];
 
    header[0] = 0xAB;
    header[1] = 0xCD;
    USB_SendData(header, sizeof(header));
    USB_SendData((u8 *)data, lenth);
}

void usb_polling_send_fake_data(uint8_t *data, uint32_t lenth)
{
    u8 header[2];
    
    for (int i=0;i<lenth;i++)
    {
        data[i] = i%0x100;
    }
 
    header[0] = 0xAB;
    header[1] = 0xCD;
    USB_SendData(header, sizeof(header));
    USB_SendData((u8 *)data, lenth);
}

__attribute__ ((weak)) int detect_presense(s16 *data)
{
    printf("algo run once!\r\n");
    
    //usb_polling_send_data((u8 *)data, sizeof(data_buf));
    usb_polling_send_fake_data((u8 *)data, sizeof(data_buf));
    
    return 0;
}
