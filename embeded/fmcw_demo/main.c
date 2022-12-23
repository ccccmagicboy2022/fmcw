#include "sys.h"

s16 data_buf[SAMPLE_NUM_PER_CHIRP * NUM_CHIRPS_PER_FRAME];

uint8_t matlab_debug_use_flag;
uint8_t flash_finished = 0;
extern prensence_info_t flash_param;

int32_t main(void)
{
    output_result_t_fmcw fmcw_result;

    usb_hs_init();

#ifdef SEND_TO_MATLAB_TEST
    matlab_debug_use_flag = 1;
    while(matlab_debug_use_flag == 1)
    {
        //pass
    }
#endif
    init_mem();
    mcu429_init();
    Flash_Init_3();
    uart_init();
    radar_init();
    fpga_init();
    bt_protocol_init();

    while (1)
    {
        bt_uart_service();
        if (is_data_available()) {
            get_sample_data((u8 *)data_buf);

            #ifdef SEND_TO_MATLAB_TEST
                #ifndef FAKE_USB_DATA
                    usb_polling_send_data((u8 *)data_buf, sizeof(data_buf));
                #else
                    usb_polling_send_fake_data((u8 *)data_buf, sizeof(data_buf));
                #endif
                detect(data_buf, &fmcw_result);
            #else
                if (flash_finished == 0)
                    detect(data_buf, &fmcw_result);
                else
                    flash_finished = 0;

                if (fmcw_result) {
                    GPIO_O_HIGH;
                    LED3_ON;
                } else {
                    GPIO_O_LOW;
                    LED3_OFF;
                }
            #endif
            if (inquire_tracking_check_status() != 0 && inquire_tracking_check_finished()) {
                set_tracking_check(0);
                check_success(DPID_TEACKING_CHECK);
                flash_finished = 1;
            }

            if (inquire_respiration_check_status() != 0 && inquire_respiration_check_finished()) {
                set_respiration_check(0);
                check_success(DPID_RESPIRATION_CHECK);
                flash_finished = 1;
            }
        }
    }
}