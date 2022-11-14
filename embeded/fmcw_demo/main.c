#include "sys.h"

s16 data_buf[512];
__attribute__ ((weak)) void detect_presense(void);
uint8_t Txbuffer[1024];

uint16_t cycle_freq_diff[DAC_WORK_RESOLUTION + 1] = {0};

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
    
    for (int i = 0; i <= DAC_WORK_RESOLUTION; i++)
    {
        cycle_freq_diff[i] = (uint16_t)((float)CPU_FREQ * FREQ_OUT_DIV * 8 \
            / (FREQ_MIN + ((float)(FREQ_MAX - FREQ_MIN) / DAC_WORK_RESOLUTION) * i) + 0.5f);
    }
    
    while (1)
    {
//        if (freq_calibration_state > DAC_WORK_RESOLUTION)
//        {
//            input_captuere_disable();
//            dac_first_deinit();
//            printf("frequency celibration finish");
//            for (int i = DAC_WORK_RESOLUTION + 1; i < DAC_WORK_RESOLUTION + DAC_IDEL_RESOLUTION / 2; i++) {
//                vt_tab[i] = vt_tab[DAC_WORK_RESOLUTION];
//            }
//            for (int i = DAC_WORK_RESOLUTION + DAC_IDEL_RESOLUTION / 2; i < DAC_ALL_RESOLUTION; i++) {
//                vt_tab[i] = vt_tab[0];
//            }
//            adc_config();
//            dac_secend_init();
//            break;
//        }
//        else
//        {
//            
//        }
    }
    
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
