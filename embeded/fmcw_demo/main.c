#include "sys.h"

s16 data_buf[SAMPLE_NUM_PER_CHIRP * NUM_CHIRPS_PER_FRAME] __attribute__ ((aligned (4)));
__attribute__ ((weak)) int detect_presense(s16 *data);

int32_t main(void)
{
    init_mem();
    mcu429_init();
    //usb_hs_init();
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
    output_result_t_fmcw fmcw_result;

#ifndef FAKE_USB_DATA
    usb_polling_send_data((u8 *)data, sizeof(data_buf));
#else
     usb_polling_send_fake_data((u8 *)data, sizeof(data_buf));
#endif

    detect(data, &fmcw_result);

    return 0;
}
