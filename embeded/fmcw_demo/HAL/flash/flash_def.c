#include "flash_def.h"
#include "sys.h"

#define     __IO           volatile
//typedef     __IO uint32_t  vu32;

// Report related parameter structure variables
tramp paratramp;

check_result  DateType1;
check_result  DateType2;
check_result  DateType3;
light_check   DateType4;

const entrance_param_t entrance_param[6] = {
#if defined(WALL_MOUNTED)
    {   /* 2m */
        .time_times = 16.0f,
        .time_add = 160.0f,
        .freq_times = 1.8f,
    },
    {   /* 2m */
        .time_times = 16.0f,
        .time_add = 160.0f,
        .freq_times = 1.8f,
    },
    {   /* 3m */
        .time_times = 12.0f,
        .time_add = 120.0f,
        .freq_times = 1.6f,
    },
    {   /* 4m */
        .time_times = 6.2f,
        .time_add = 60.0f,
        .freq_times = 1.6f,
    },
    {   /* 5m */
        .time_times = 5.0f,
        .time_add = 50.0f,
        .freq_times = 1.2f,
    },
    {   /* 6m */
        .time_times = 4.5f,
        .time_add = 45.0f,
        .freq_times = 1.2f,
    }
#elif defined(ZHIXIN)
    {
        .time_times = 16.0f,
        .time_add = 440.0f,
        .freq_times = 1.4f,
    },
    {
        .time_times = 16.0f,
        .time_add = 340.0f,
        .freq_times = 1.4f,
    },
    {
        .time_times = 16.0f,
        .time_add = 340.0f,
        .freq_times = 1.4f,
    },
    {
        .time_times = 16.0f,
        .time_add = 340.0f,
        .freq_times = 1.4f,
    },
    {
        .time_times = 16.0f,
        .time_add = 340.0f,
        .freq_times = 1.4f,
    },
    {
        .time_times = 16.0f,
        .time_add = 340.0f,
        .freq_times = 1.4f,
    }
#elif defined(MLK)
    {   /* 1m */
        .time_times = 21.0f,
        .time_add = 180.0f,
        .freq_times = 1.8f,
    },
    {   /* 2m */
        .time_times = 7.0f,
        .time_add = 60.0f,
        .freq_times = 1.8f,
    },
    {   /* 3m */
        .time_times = 4.8f,
        .time_add = 38.0f,
        .freq_times = 1.6f,
    },
    {   /* 3m */
        .time_times = 4.8f,
        .time_add = 38.0f,
        .freq_times = 1.6f,
    },
    {   /* 3m */
        .time_times = 4.8f,
        .time_add = 38.0f,
        .freq_times = 1.6f,
    },
    {   /* 3m */
        .time_times = 4.8f,
        .time_add = 38.0f,
        .freq_times = 1.6f,
    }
#elif defined(OLD_MODULE)
    {   /* 1.7m */
        .time_times = 7.8f,
        .time_add = 64.0f,
        .freq_times = 1.8f,
    },
    {   /* 2.2m */
        .time_times = 6.0f,
        .time_add = 54.0f,
        .freq_times = 1.6f,
    },
    {   /* 2.7m */
        .time_times = 4.5f,
        .time_add = 38.0f,
        .freq_times = 1.6f,
    },
    {   /* 3.2m */
        .time_times = 3.6f,
        .time_add = 31.0f,
        .freq_times = 1.4f,
    },
    {   /* 3.5m */
        .time_times = 3.2f,
        .time_add = 25.0f,
        .freq_times = 1.2f,
    },
    {   /* 4m */
        .time_times = 2.8f,
        .time_add = 18.0f,
        .freq_times = 1.2f,
    }
#else
    {   /* 1.7m */
        .time_times = 7.0f,
        .time_add = 64.0f,
        .freq_times = 1.8f,
    },
    {   /* 2.2m */
        .time_times = 5.8f,
        .time_add = 48.0f,
        .freq_times = 1.6f,
    },
    {   /* 2.7m */
        .time_times = 5.4f,
        .time_add = 44.0f,
        .freq_times = 1.6f,
    },
    {   /* 3.2m */
        .time_times = 4.0f,
        .time_add = 36.0f,
        .freq_times = 1.4f,
    },
    {   /* 3.7m */
        .time_times = 3.5f,
        .time_add = 30.0f,
        .freq_times = 1.2f,
    },
    {   /* 4m */
        .time_times = 3.2f,
        .time_add = 28.0f,
        .freq_times = 1.2f,
    }
#endif
};

const presence_param_t presence_param[4] = {
#if defined(WALL_MOUNTED)
    {   /* 2m */
        .time_times = 16.0f,
        .time_add = 160.0f,
        .freq_times = 1.8f,
        .respiration_times = 32.0f,
        .offset_max = 1.25f,
        .offset_min = 1.2f,
    },
    {   /* 2.7m */
        .time_times = 12.4f,
        .time_add = 124.0f,
        .freq_times = 1.6f,
        .respiration_times = 18.0f,
        .offset_max = 0.85f,
        .offset_min = 0.8f,
    },
    {   /* 3.4m */
        .time_times = 7.4f,
        .time_add = 74.0f,
        .freq_times = 1.6f,
        .respiration_times = 18.0f,
        .offset_max = 0.75f,
        .offset_min = 0.7f,
    },
    {   /* 4m */
        .time_times = 6.2f,
        .time_add = 60.0f,
        .freq_times = 1.6f,
        .respiration_times = 12.0f,
        .offset_max = 0.45f,
        .offset_min = 0.4f,
    }
#elif defined(ZHIXIN)
    {
        .time_times = 6.0f,
        .time_add = 400.0f,
        .freq_times = 1.4f,
        .respiration_times = 18.0f,
        .offset_max = 0.55f,
        .offset_min = 0.5f,
    },
    {
        .time_times = 6.0f,
        .time_add = 300.0f,
        .freq_times = 1.4f,
        .respiration_times = 16.0f,
        .offset_max = 0.5f,
        .offset_min = 0.45f,
    },
    {
        .time_times = 6.0f,
        .time_add = 200.0f,
        .freq_times = 1.4f,
        .respiration_times = 16.0f,
        .offset_max = 0.5f,
        .offset_min = 0.45f,
    },
    {
        .time_times = 6.0f,
        .time_add = 100.0f,
        .freq_times = 1.4f,
        .respiration_times = 16.0f,
        .offset_max = 0.5f,
        .offset_min = 0.45f,
    }
#elif defined(MLK)
    {   /* 1m */
        .time_times = 21.0f,
        .time_add = 180.0f,
        .freq_times = 1.8f,
        .respiration_times = 34.0f,
        .offset_max = 0.7f,
        .offset_min = 0.64f,
    },
    {   /* 2m */
        .time_times = 7.5f,
        .time_add = 65.0f,
        .freq_times = 1.8f,
        .respiration_times = 9.5f,
        .offset_max = 0.59f,
        .offset_min = 0.56f,
    },
    {   /* 3m */
        .time_times = 5.3f,
        .time_add = 43.0f,
        .freq_times = 1.6f,
        .respiration_times = 9.0f,
        .offset_max = 0.5f,
        .offset_min = 0.45f,
    },
    {   /* 3m */
        .time_times = 5.3f,
        .time_add = 43.0f,
        .freq_times = 1.6f,
        .respiration_times = 9.0f,
        .offset_max = 0.5f,
        .offset_min = 0.45f,
    }
#elif defined(OLD_MODULE)
    {   /* 1.7m */
        .time_times = 8.2f,
        .time_add = 68.0f,
        .freq_times = 1.8f,
        .respiration_times = 25.0f,
        .offset_max = 0.96f,
        .offset_min = 0.92f,
    },
    {   /* 2.2m */
        .time_times = 6.4f,
        .time_add = 58.0f,
        .freq_times = 1.6f,
        .respiration_times = 25.0f,
        .offset_max = 0.76f,
        .offset_min = 0.7f,
    },
    {   /* 2.7m */
        .time_times = 4.9f,
        .time_add = 42.0f,
        .freq_times = 1.6f,
        .respiration_times = 20.0f,
        .offset_max = 0.6f,
        .offset_min = 0.55f,
    },
    {   /* 3.2m */
        .time_times = 4.1f,
        .time_add = 35.0f,
        .freq_times = 1.4f,
        .respiration_times = 16.0f,
        .offset_max = 0.5f,
        .offset_min = 0.45f,
    }
#else
    {   /* 1.7m */
        .time_times = 7.4f,
        .time_add = 68.0f,
        .freq_times = 1.8f,
        .respiration_times = 20.0f,
        .offset_max = 0.85f,
        .offset_min = 0.8f,
    },
    {   /* 2.2m */
        .time_times = 6.2f,
        .time_add = 52.0f,
        .freq_times = 1.6f,
        .respiration_times = 19.0f,
        .offset_max = 0.80f,
        .offset_min = 0.75f,
    },
    {   /* 2.7m */
        .time_times = 5.8f,
        .time_add = 48.0f,
        .freq_times = 1.6f,
        .respiration_times = 16.0f,
        .offset_max = 0.50f,
        .offset_min = 0.45f,
    },
    {   /* 3.2m */
        .time_times = 4.4f,
        .time_add = 40.0f,
        .freq_times = 1.4f,
        .respiration_times = 13.0f,
        .offset_max = 0.45f,
        .offset_min = 0.4f,
    }
#endif
};

prensence_info_t flash_param,default_param = {
    .magic_num = MAGIC_NUM,
    .crc = 0,
    .delay_time = DELAY_TIME,
    .upload_duty = 8000,
    .light_threshold  = LIGHE_HOLD,
#if defined(WALL_MOUNTED)
    .entrance_range = RADAR_ENTRANCE_THIRD,
#elif defined(ZHIXIN)
    .entrance_range = RADAR_ENTRANCE_SECOND,
#elif defined(YWR)
    .entrance_range = RADAR_ENTRANCE_FIRST,
#elif defined(MLK)
    .entrance_range = RADAR_ENTRANCE_FIRST,
#elif defined(OLD_MODULE)
    .entrance_range = RADAR_ENTRANCE_FOURTH,
#elif defined(YHY)
    .entrance_range = RADAR_ENTRANCE_FIFTH,
#else
    .entrance_range = RADAR_ENTRANCE_FOURTH,
#endif

#if defined(WALL_MOUNTED)
    .presence_range = RADAR_RANGE_THIRD,
#elif defined(ZHIXIN)
    .presence_range = RADAR_RANGE_SECOND,
#elif defined(YWR)
    .presence_range = RADAR_RANGE_FIRST,
#elif defined(MLK)
    .presence_range = RADAR_RANGE_FIRST,
#elif defined(OLD_MODULE)
    .presence_range = RADAR_RANGE_FOURTH,
#elif defined(GONGNIU)
    .presence_range = RADAR_RANGE_SECOND,
#elif defined(YHY)
    .presence_range = RADAR_RANGE_FOURTH,
#else
    .presence_range = RADAR_RANGE_FOURTH,
#endif

    .status_upload_enable = 1,
    .overthreshold_upload_enable = 0,
    .led_enable = 0,
    .respiratory_upload_enable = 0,
    .heartbeat_upload_enable = 0,
};

/*
*********************************************************************************************************
* Function name: Flash_Write
* Function description: Continuous write operation to sector 62
* Parameters:u8Len:Data length,
*            u32TestBuf: pointer to write data \
*            (46064 sectors, 8K each, currently 23 sectors used, nearly 40 sectors available)
* Return value: none
*********************************************************************************************************
*/
void Flash_Write(uint8_t u8Len, prensence_info_t *current_detect_param)
{
    uint32_t Counter_Num = 0;
    //current_detect_param->crc = calculate_CRC32((uint32_t*)&current_detect_param->delay_time, sizeof(prensence_info_t) - FLASH_LENGTH_CRC_MAGIC);

    FLASH_Unlock();

//    if (FLASH_COMPL != FLASH_EraseOnePage(FLASH_SECTOR62_ADDR))
//    {
//        //error
//    }

//    for (Counter_Num = 0; Counter_Num < u8Len; Counter_Num += 4)
//    {
//        if (FLASH_COMPL != FLASH_ProgramWord(FLASH_SECTOR62_ADDR + Counter_Num, *((u32 *)((u8 *)current_detect_param + Counter_Num))))
//        {
//            //error
//        }
//    }

    FLASH_Lock();
}
/*
*********************************************************************************************************
* Function name: Flash_SectorErase
* Function description: Erase sector  62.
* Parameters: none
* Return value: none
*********************************************************************************************************
*/
void Flash_SectorErase()
{
    FLASH_Unlock();

//    if (FLASH_COMPL != FLASH_EraseOnePage(FLASH_SECTOR62_ADDR))
//    {
//        //error
//    }

    FLASH_Lock();
}

//void noperson_status_upload(uint8_t aaaa, upload_status_t *upload_status)
//{
//    int first_status_flag = aaaa;
//    static int first_status_flag_last = 1;

//    if (first_status_flag !=  first_status_flag_last) {
//#ifdef DETECT_RESPIRATORY_HEART
//        if (flash_param.respiratory_upload_enable)
//            mcu_dp_value_update(DPID_BREATHE_FREQ,
//                                (int32_t)(upload_status->resp_heart_rate.respiratory_rate * 1000));
//        if (flash_param.heartbeat_upload_enable)
//            mcu_dp_value_update(DPID_HEART_RATE,
//                                (int32_t)(upload_status->resp_heart_rate.heart_rate * 1000));
//#endif
//        if (first_status_flag == 0) {
//            paratramp.check_final = NO_PERSON_STATUS;
//            if (flash_param.status_upload_enable == 1)
//                mcu_dp_enum_update(DPID_PERSON_IN_RANGE, paratramp.check_final);
//        } else {
//            paratramp.check_final = PERSON_BIG1;
//            if (flash_param.status_upload_enable == 1)
//                mcu_dp_enum_update(DPID_PERSON_IN_RANGE, paratramp.check_final);
//        }
//        first_status_flag_last = first_status_flag;
//    }
//}

void sampling_status_upload(void)
{
//    if (paratramp.sampling_status == 1 && paratramp.test_result_upload == 1) {
//        paratramp.test_result_upload = 0;
//        DateType1.floatdate = moudle_para_t.condition_notgt1;
//        DateType2.floatdate = moudle_para_t.condition_tgt;
//        DateType3.floatdate = moudle_para_t.condition_notgt2;
//        DateType4.lightnum  = paratramp.light_adc_data;
//        mcu_dp_value_update(DPID_UPLOAD_TESTDATA, DateType1.data);
//        mcu_dp_value_update(DPID_UPLOAD_TESTDATA, DateType2.data);
//        mcu_dp_value_update(DPID_UPLOAD_TESTDATA, DateType3.data);
//        mcu_dp_value_update(DPID_BREATHE_FREQ, DateType4.data);
//    }
}

void calc_data_upload(void)
{
//    volatile s32 upload_test_data = 0;

//    if (paratramp.trigger_exist_calc_once == 1)
//    {
//        upload_test_data = 0;
//        upload_test_data = (s16)(get_upload_respiration_times()*100);
//        upload_test_data += 0x0F000000 & (0x01u  << 24);
//        mcu_dp_value_update(DPID_UPLOAD_TESTDATA, upload_test_data);

//        upload_test_data = 0;
//        upload_test_data = (s16)(get_upload_offsetmax()*100);
//        upload_test_data += 0x0F000000 & (0x02u  << 24);
//        mcu_dp_value_update(DPID_UPLOAD_TESTDATA, upload_test_data);

//        paratramp.trigger_exist_calc_once = 0;
//    }

//    if (paratramp.trigger_breakin_calc_once == 1)
//    {
//        upload_test_data = 0;
//        upload_test_data = (s16)(get_upload_time_times()*100);
//        upload_test_data += 0x0F000000 & (0x01u  << 24);
//        mcu_dp_value_update(DPID_UPLOAD_TESTDATA, upload_test_data);

//        upload_test_data = 0;
//        upload_test_data = (s16)(get_upload_time_add()*100);
//        upload_test_data += 0x0F000000 & (0x02u  << 24);
//        mcu_dp_value_update(DPID_UPLOAD_TESTDATA, upload_test_data);

//        paratramp.trigger_breakin_calc_once = 0;
//    }
}

void product_test_status_upload(void)
{
    mcu_dp_enum_update(DPID_PERSON_IN_RANGE, 1);
}

//void output_status_upload(upload_status_t *upload_status)
//{
//    uint8_t output_result_t;

//    output_result_t = upload_status->status;
//    turnoff_led_blinking();

//#ifdef DETECT_RESPIRATORY_HEART
//    if (output_result_t) {
//        if (flash_param.respiratory_upload_enable)
//            mcu_dp_value_update(DPID_BREATHE_FREQ,
//                                (int32_t)(upload_status->resp_heart_rate.respiratory_rate * 1000));
//        if (flash_param.heartbeat_upload_enable)
//            mcu_dp_value_update(DPID_HEART_RATE,
//                                (int32_t)(upload_status->resp_heart_rate.heart_rate * 1000));
//    }
//#endif

//    switch (output_result_t) {
//    case 0:    // unmanned
//        noperson_status_upload(output_result_t, upload_status);
//#ifndef ZHIXUAN
//        if (flash_param.led_enable == 1)
//            LED_GREEN();
//        else
//            LED_OFF();
//#endif
//        if (paratramp.light_enable_flag == 1) {
//            OUT_OUT1_OFF();
//        }
//        break;

//    case 1:     // Fast 1
//        noperson_status_upload(output_result_t, upload_status);
//#ifdef ZHIXUAN
//        if (paratramp.board_led_upload_enable == 1) {
//            LED_BLUE();
//            trigger_led_blinking(GREEN_LED);
//        }
//#else
//        if (flash_param.led_enable == 1)
//            LED_RED();
//        else
//            LED_OFF();
//#endif
//        if (paratramp.light_enable_flag == 1)     // OUT
//            OUTON_OUT1OFF();
//        break;

//    case 2:     // Big 1
//        paratramp.check_final = PERSON_BIG1;
//        if (flash_param.status_upload_enable == 1)
//            mcu_dp_enum_update(DPID_PERSON_IN_RANGE, paratramp.check_final);
//        if (flash_param.overthreshold_upload_enable == 1)
//            mcu_dp_enum_update(DPID_SLOW_CHECK_THRESH, 1);
//#ifndef ZHIXUAN
//        if (flash_param.led_enable == 1) {
//            LED_RED();
//            trigger_led_blinking(RED_LED);
//        } else {
//            LED_OFF();
//        }
//#endif
//        if (paratramp.light_enable_flag == 1) {
//            OUTON_OUT1OFF();
//            flash_param.light_threshold = LIGHE_HOLD;
//        }
//        break;

//    case 3:     // Tiny 1
//        paratramp.check_final = PERSON_TINY1;
//        if (flash_param.status_upload_enable == 1)
//            mcu_dp_enum_update(DPID_PERSON_IN_RANGE, paratramp.check_final);
//        if (flash_param.overthreshold_upload_enable == 1)
//            mcu_dp_enum_update(DPID_SLOW_CHECK_THRESH, 1);
//#ifndef ZHIXUAN
//        if (flash_param.led_enable == 1) {
//            LED_BLUE();
//            trigger_led_blinking(GREEN_LED);
//        } else {
//            LED_OFF();
//        }
//#endif
//        if (paratramp.light_enable_flag == 1) {
//            OUT_OUT1_ON();
//            flash_param.light_threshold = LIGHE_HOLD;
//        }
//        break;

//    case 4:      // Big 0
//        paratramp.check_final = PERSON_BIG0_TINY0;
//        if (flash_param.status_upload_enable == 1)
//            mcu_dp_enum_update(DPID_PERSON_IN_RANGE, paratramp.check_final);
//        if (flash_param.overthreshold_upload_enable == 1)
//            mcu_dp_enum_update(DPID_SLOW_CHECK_THRESH, 0);
//#ifndef ZHIXUAN
//        if (flash_param.led_enable == 1)
//            LED_RED();
//        else
//            LED_OFF();
//#endif
//        if (paratramp.light_enable_flag == 1) {
//            OUTON_OUT1OFF();
//            flash_param.light_threshold = LIGHE_HOLD;
//        }
//        break;

//    case 5:     // Tiny 0
//        paratramp.check_final = PERSON_BIG0_TINY0;
//        if (flash_param.status_upload_enable == 1)
//            mcu_dp_enum_update(DPID_PERSON_IN_RANGE, paratramp.check_final);
//        if (flash_param.overthreshold_upload_enable == 1)
//            mcu_dp_enum_update(DPID_SLOW_CHECK_THRESH, 0);
//#ifndef ZHIXUAN
//        if (flash_param.led_enable == 1)
//            LED_BLUE();
//        else
//            LED_OFF();
//#endif
//        if (paratramp.light_enable_flag == 1) {
//            OUT_OUT1_ON();
//            flash_param.light_threshold = LIGHE_HOLD;
//        }
//        break;
//    case 6:     // instant one
//        if (paratramp.instant_response_upload_enable) {
//            paratramp.check_final = PERSON_BIG1;
//            if (flash_param.status_upload_enable == 1)
//                mcu_dp_enum_update(DPID_PERSON_IN_RANGE, paratramp.check_final);
//            if (flash_param.overthreshold_upload_enable == 1)
//                mcu_dp_enum_update(DPID_SLOW_CHECK_THRESH, 0);
//#ifndef ZHIXUAN
//            if (flash_param.led_enable == 1) {
//                LED_RED();
//            } else {
//                LED_OFF();
//            }
//#endif
//            if (paratramp.light_enable_flag == 1) {
//                OUTON_OUT1OFF();
//                flash_param.light_threshold = LIGHE_HOLD;
//            }
//        }
//        break;
//    default:
//        break;
//    }
//}

//void respiration_adaptive_param_upload(void)
//{
//    mcu_dp_value_update(DPID_RESP_ADAPTIVE_TIMES,
//                        (int32_t)(resp_adaptive.respiration_times * 1000));
//    mcu_dp_value_update(DPID_RESP_ADAPTIVE_OFFSET,
//                        (int32_t)(resp_adaptive.offset * 1000));
//}

//void respiration_adaptive_radar_range(void)
//{
//    int detect_range = 0,i;

//    if (resp_adaptive.respiration_times > presence_param[0].respiration_times || resp_adaptive.offset > presence_param[0].offset_max)
//        detect_range = 0;
//    else {
//        for (i = 0; i < RADAR_RANGE_FOURTH ; i++) {
//            if (presence_param[i].respiration_times > resp_adaptive.respiration_times && presence_param[i].offset_max > resp_adaptive.offset)
//                detect_range = i + 1;
//        }
//    }

//    if (detect_range != 0) {
//        flash_param.presence_range = detect_range;
//        set_radar_range(flash_param.presence_range);
//        mcu_dp_enum_update(DPID_RESP_ADAPTIVE_RANGE, 1);
//    } else
//        mcu_dp_enum_update(DPID_RESP_ADAPTIVE_RANGE, 0);

//}
/*
Amplitude limiting average filtering method is equivalent to "amplitude limiting filtering method" +
"recursive average filtering method". Each sampled new data is first processed by amplitude limiting,
and then sent to the queue for recursive average filtering.
Firstly, the sampling data is screened by the amplitude limiting filtering method,
and the data that meets the requirements is stored in the array circulatively. Finally,
the arithmetic average value of the sampling value in the array is taken.
After filtering algorithm, the pulse interference in the system can be effectively filtered out,
making the sampling data more smooth.
The limiting average filtering method combines the advantages of the limiting average filtering method
and the recursive average filtering method, which makes the sampling results more accurate,
but the calculation speed will be slow, and also requires a certain amount of space to store data.
*/


unsigned int num_sub(unsigned int a, unsigned int b)
{
    return ( a >=  b ? ( a - b ) : ( b - a ) );
}
// There is a delay in sampling sinusoidal phase
unsigned int filter6(int filter6_data)
{
    static unsigned int value_buf[num_sub_N];
    static unsigned int i = 0;
    unsigned int count;
    unsigned int  new_value = 0;
    static unsigned int last_value = 0;
    int  sum = 0;

    new_value = filter6_data;

    if ( num_sub( new_value, last_value ) < num_sub_A )
        value_buf[i++] = last_value;
    else {
        value_buf[i++] = new_value;
        last_value = new_value;
    }

    if ( i == num_sub_N ) i = 0;

    for ( count = 0; count < num_sub_N; count++ ) {
        sum +=  value_buf[count];
    }
    return ( unsigned int )( sum / num_sub_N );
}


void LightData_TaskHandle(TESK_ID_TYPEDEF Task_ID, Task_TYPEDEF Task_State)
{
#if defined(ZHIXIN) || defined(YWR) || defined(YHY)
    paratramp.light_enable_flag = 1;
#else
    if (Task_State == Task_STATE1) {
        paratramp.light_adc_data = filter6(paratramp.light_adc_temp);
        if (paratramp.light_adc_data < flash_param.light_threshold) {
            paratramp.light_enable_flag = 1;   // The evening has to
        } else {
            paratramp.light_enable_flag = 0;   // Is still during the day
            //OUT_OUT1_OFF();
        }
    } else if (Task_State == Task_STATE2) {
        paratramp.light_adc_data = filter6(paratramp.light_adc_temp);
        if (paratramp.light_adc_data < flash_param.light_threshold) {
            paratramp.light_enable_flag = 1;   // The evening has to
        } else {
            paratramp.light_enable_flag = 0;   // Is still during the day
            //OUT_OUT1_OFF();
        }
    }
#endif
}

/*****************************************************************************
Function name : dp_download_load_radar_parameter_handle
Description   : DPID LOAD RADAR PARAMETER processing function
Input   : value: Data source data
        : length: Data length
Return  : SUCCESS/ERROR
Instructions : It can be delivered and reported. After data is processed,
           the processing result must be reported to the APP
*****************************************************************************/
//void set_entrance_range(int range)
//{
//    detect_param_t paraset;

//    if (range < RADAR_ENTRANCE_FIRST || range > RADAR_ENTRANCE_SIXTH)
//        return;

//    flash_param.entrance_range = range;

//    range -= 1;
//    paraset.time_detect_param.time_times_0 = entrance_param[range].time_times;
//    paraset.time_detect_param.time_add_0 = entrance_param[range].time_add;
//    paraset.freq_detect_param.freq_times = entrance_param[range].freq_times;

//    set_entrance_param(&paraset);
//}

//void set_radar_range(int range)
//{
//    detect_param_t paraset;

//    if (range < RADAR_RANGE_FIRST || range > RADAR_RANGE_FOURTH)
//        return;

//    flash_param.presence_range = range;

//    range -= 1;
//    paraset.time_detect_param.time_times = presence_param[range].time_times;
//    paraset.time_detect_param.time_add = presence_param[range].time_add;
//    paraset.freq_detect_param.freq_times2 = presence_param[range].freq_times;
//    paraset.freq_detect_param.respiration_times = presence_param[range].respiration_times;

//    paraset.offset_max = presence_param[range].offset_max;
//    paraset.offset_min = presence_param[range].offset_min;

//    set_presence_param(&paraset);
//}

void Flash_Init_62()
{
    uint32_t crc_value = 0;
    uint32_t read_magic_num = 0;
    uint32_t read_crc = 0;

    read_magic_num = *((uint32_t *)FLASH_SECTOR62_ADDR);

    if (read_magic_num != MAGIC_NUM)
        flash_param = default_param;
    else {
        read_crc = *((uint32_t *)FLASH_SECTOR62_ADDR + FLASH_ADDR_OFFSET_CRC);

        //crc_value = calculate_CRC32((uint32_t*)(FLASH_SECTOR62_ADDR + FLASH_LENGTH_CRC_MAGIC), sizeof(flash_param) - FLASH_LENGTH_CRC_MAGIC);

        if (crc_value == read_crc) {
            for (int i = 0; i < sizeof(flash_param) / 4; i++)
                *((uint32_t *)&flash_param + i) = *((uint32_t *)FLASH_SECTOR62_ADDR + i);
        } else {
            flash_param = default_param;
          }
    }

    //set_delay_time(flash_param.delay_time);
    //set_radar_range(flash_param.presence_range);
    //set_entrance_range(flash_param.entrance_range);

    paratramp.instant_response_upload_enable = 1;
}

