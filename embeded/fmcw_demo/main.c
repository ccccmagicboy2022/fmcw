#include "sys.h"

s16 data_buf[SAMPLE_NUM_PER_CHIRP * NUM_CHIRPS_PER_FRAME] __attribute__ ((aligned (4)));
__attribute__ ((weak)) int detect_presense(s16 *data);

uint8_t matlab_debug_use_flag;

int32_t main(void)
{
    usb_hs_init();
    matlab_debug_use_flag = 0;
    while(matlab_debug_use_flag == 1)
    {
        //pass
    }

    init_mem();
    mcu429_init();
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

#ifdef SEND_TO_MATLAB_TEST
    #ifndef FAKE_USB_DATA
        usb_polling_send_data((u8 *)data, sizeof(data_buf));
    #else
        usb_polling_send_fake_data((u8 *)data, sizeof(data_buf));
    #endif
#else
    detect(data, &fmcw_result);

    if (fmcw_result)
    {
        GPIO_O_HIGH;
        LED3_ON;
    }
    else
    {
        GPIO_O_LOW;
        LED3_OFF;
    }
#endif

    return 0;
}
