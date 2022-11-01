#include "bluetooth.h"
#include "sys.h"

volatile u8 Flash_data_run;

//extern unsigned long version_num;
//extern prensence_info_t flash_param;

#ifdef O1_DATA_STREAM
extern void gpio_output_init(void);
#endif

#ifdef LONGQUAN
extern uint8_t adc1_flash_enable;
extern uint8_t adc1_flash_slow_enable;
extern uint8_t adc1_adc2_all_flash_enable;

extern uint8_t adc2_1s_on_enable;
#endif

/******************************************************************************
                                Transplant notes:
1.MCU call bt_uart_service() in the main loop.
2: After the program is initialized normally, it is recommended not to close the serial port interrupt. If the interrupt must be turned off, the interrupt time must be short. Turning off the interrupt will cause the serial port data packet to be lost.
3: Do not call the report function in the interrupt/timer interrupt
******************************************************************************/


/******************************************************************************
                              Step 1: initialization
1: Include the header file "Bt.h" in files that need to be used with BT-related files
2: Call the BT_protocol_init () function of the McU_api.c file during MCU initialization
3: Fill the MCU serial byte transmission function into the protocol. C file uart_transmit_output function, and delete #error
4: Call the Uart_receive_input function in the file mcu_api. c from the MCU uart port receipt function, and pass in the received bytes as parameters
5: After the MCU enters the while loop, it calls the bT_uart_service () function in the mcu_api. c file
******************************************************************************/

/******************************************************************************
                        1:Comparison Table of sequence types of dp data points
          * * this is automatically generated code. If there are any changes on the development platform, please download MCU_SDK** again.
******************************************************************************/
const DOWNLOAD_CMD_S download_cmd[] =
{
    {DPID_PIR_DELAY, DP_TYPE_VALUE},                 // Delay time

    {DPID_LOAD_RADAR_PARAMETER, DP_TYPE_ENUM},       // Coverage area
    {DPID_ENTRANCE_PARAMETER, DP_TYPE_ENUM},         // Entrance range

    {DPID_RESP_ADAPTIVE_TIMES, DP_TYPE_VALUE},        // Respiration adaptive param

    {DPID_RESP_ADAPTIVE_OFFSET, DP_TYPE_VALUE},       // Respiration adaptive param

    {DPID_RESP_ADAPTIVE_RANGE, DP_TYPE_ENUM},        // Respiration adaptive radar range success or failure

    {DPID_PERSON_IN_RANGE, DP_TYPE_ENUM},            // People's state

    {DPID_COMMON_COMMAND, DP_TYPE_ENUM},             // General's command

//    {DPID_SLOW_CHECK_RESULT, DP_TYPE_ENUM},          // move/static state

    {DPID_STUDY_CMD, DP_TYPE_ENUM},                  // Background detection function

    {DPID_STUDY_PROCESS_UPLOAD, DP_TYPE_ENUM},       // Background detection result reported

    {DPID_LIGHT_STATUS, DP_TYPE_ENUM},               // Lamp status report

    {DPID_BREATHE_FREQ, DP_TYPE_VALUE},              // Respiratory rate report

    {DPID_BREATHE_AMP, DP_TYPE_VALUE},               // Respiratory range report

    {DPID_HEART_RATE, DP_TYPE_VALUE},                // Reporting of heart rate

    {DPID_FIND_ME, DP_TYPE_BOOL},                    // Scan for devices

    {DPID_CHECK_STATUS, DP_TYPE_ENUM},               // Test status

    {DPID_LIGHT_SENSOR_RAW, DP_TYPE_VALUE},          // Photosensitive value

    {DPID_LIGHT_THRESHOLD1, DP_TYPE_VALUE},          // Light sensitive threshold one

    {DPID_LIGHT_THRESHOLD2, DP_TYPE_VALUE},          // Photosensitive threshold two

    {DPID_LIGHT_THRESHOLD3, DP_TYPE_VALUE},          // Light sensitive threshold three

    {DPID_LIGHT_THRESHOLD4, DP_TYPE_VALUE},          // Light sensitive threshold four

    {DPID_FACTORY_OP, DP_TYPE_ENUM},                 // Factory pattern

    {DPID_WORK_MODE, DP_TYPE_ENUM},                  // Working mode selection

    {DPID_UPLOAD_DUTY, DP_TYPE_VALUE},               // The communication cycle

    {DPID_UPLOAD_TESTDATA, DP_TYPE_VALUE},           // Test data reporting
};



/******************************************************************************
                           2: uart port single byte sending function
Please fill the MCU serial port sending function into this function, and pass the received data into the serial port sending function as parameters
******************************************************************************/

/*****************************************************************************
Function name: uart_transmit_output
Function description: send data processing
Input parameters: value: uart port receives byte data
Return parameter: none
Instructions for use: please fill the MCU serial port sending function into this function, and pass the received data into the serial port sending function as parameters
*****************************************************************************/
void uart_transmit_output(uint8_t value)
{
//    USART_SendData(USART3, value);
//    while (USART_GetFlagStatus(USART3, USART_FLAG_TXDE) == RESET)
//        ;
}

void uart_transmit_output2(uint8_t value)
{
//    USART_SendData(USART1, value);
//    while (USART_GetFlagStatus(USART1, USART_FLAG_TXDE) == RESET)
//        ;
}

/******************************************************************************
                           Step 2: implement specific user functions
1:APP dispatch data processing
2: data reporting processing
******************************************************************************/

/******************************************************************************
                            1: all data reporting and processing
The current function handles all data reporting (including dispatch / escalation and escalation only)
  Users are required to implement according to the actual situation:
  1: it is necessary to implement data points that can be sent / reported.
  2: only data points need to be reported.
This function must be called within MCU.
Users can also call this function to report all data.
******************************************************************************/

// automatically generate data reporting function.

/*****************************************************************************
Function name: all_data_update
Function description: upload all dp information of the system to achieve data synchronization between APP and muc
Input parameters: none
Return parameter: none
Instructions for use: this function needs to be called internally in SDK（上报）
           MCU must implement the data reporting function within this function, including reporting only and downloadable hairstyle data.
*****************************************************************************/

void all_data_update(void)
{
//    mcu_dp_value_update(DPID_PIR_DELAY,
//                        flash_param.delay_time);     // Delay time
//    //Ddl_Delay1ms(ALL_UPLOAD_DELAY);

//    mcu_dp_enum_update(DPID_ENTRANCE_PARAMETER, flash_param.entrance_range);   // Entrance range
//    //Ddl_Delay1ms(ALL_UPLOAD_DELAY);

//    mcu_dp_enum_update(DPID_LOAD_RADAR_PARAMETER, flash_param.presence_range);   // Coverage area
//    //Ddl_Delay1ms(ALL_UPLOAD_DELAY);

//    mcu_dp_enum_update(DPID_PERSON_IN_RANGE,
//                       paratramp.check_final);        // People's state
//    //Ddl_Delay1ms(ALL_UPLOAD_DELAY);

//    mcu_dp_enum_update(DPID_LIGHT_STATUS,
//                       paratramp.light_status_flag);  // Lamp status report
//    //Ddl_Delay1ms(ALL_UPLOAD_DELAY);

//    //Ddl_Delay1ms(ALL_UPLOAD_DELAY);

//    mcu_dp_bool_update(DPID_FIND_ME, 1);              // Scan for devices
//    //Ddl_Delay1ms(ALL_UPLOAD_DELAY);

//    mcu_dp_enum_update(DPID_CHECK_STATUS,
//                       paratramp.check_status);       // Test status
//    //Ddl_Delay1ms(ALL_UPLOAD_DELAY);

//    mcu_dp_value_update(DPID_LIGHT_SENSOR_RAW,
//                        paratramp.light_adc_data);    // Photosensitive value
//    //Ddl_Delay1ms(ALL_UPLOAD_DELAY);

//    mcu_dp_value_update(DPID_LIGHT_THRESHOLD4,
//                        flash_param.light_threshold);      // Light sensitive threshold four
//    //Ddl_Delay1ms(ALL_UPLOAD_DELAY);

//    mcu_dp_enum_update(DPID_WORK_MODE,
//                       paratramp.g_work_mode);        // Working mode report
//    //Ddl_Delay1ms(ALL_UPLOAD_DELAY);
}


#define MIN_DELAY_TIME                   (32)
/*****************************************************************************
Function name : dp_download_pir_delay_handle
Description : PDID PIR DELAY processing function
Input   : value: Data source data
        : length: Data length
Return  : SUCCESS/ERROR
Instructions : It can be delivered and reported. After data is processed,
           the processing result must be reported to the APP
*****************************************************************************/
static uint8_t dp_download_pir_delay_handle(const uint8_t value[],
                                            uint16_t length)
{
    // Example:The current DP type is ENUM
    uint8_t ret;
    int pir_delay;
    int delay_now;

    pir_delay = mcu_get_dp_download_value(value, length);

    // printf("load:%d  \n", paratramp.delay_time_load);
//    pir_delay = (int)(pir_delay/8);          // The actual use
//    delay_now = pir_delay * 8;
    delay_now = pir_delay;

#if !defined(ZHIXIN) && !defined(GONGNIU)
    delay_now = delay_now < MIN_DELAY_TIME ? MIN_DELAY_TIME : delay_now;
#endif
    //flash_param.delay_time = delay_now;
    //set_delay_time(delay_now);

#ifdef LONGQUAN
    adc2_1s_on_enable = 1;
#endif
    // Feedback should be provided after DP data is processed
    //ret = mcu_dp_value_update(DPID_PIR_DELAY, flash_param.delay_time);
    if (ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}

static uint8_t dp_download_load_radar_parameter_handle(const uint8_t value[],
                                                       uint16_t length)
{
    // Example: The current DP type is ENUM
    uint8_t load_radar_parameter;
    uint8_t ret;

    load_radar_parameter = mcu_get_dp_download_enum(value, length);

#ifdef LONGQUAN
    adc2_1s_on_enable = 1;
#endif

    //set_radar_range(load_radar_parameter);

    // Feedback should be provided after DP data is processed
    ret = mcu_dp_enum_update(DPID_LOAD_RADAR_PARAMETER, load_radar_parameter);
    if (ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}

static uint8_t dp_download_load_entrance_parameter_handle(const uint8_t value[],
                                                       uint16_t length)
{
    uint8_t entrance_param;
    uint8_t ret;

    entrance_param = mcu_get_dp_download_enum(value, length);

#ifdef LONGQUAN
    adc2_1s_on_enable = 1;
#endif

    //set_entrance_range(entrance_param);

    ret = mcu_dp_enum_update(DPID_ENTRANCE_PARAMETER, entrance_param);
    if (ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}
/*****************************************************************************
Function name : dp_download_find_me_handle
Description   : DPID_FIND_ME processing function
Input   : value: Data source data
        : length: Data length
Return  : SUCCESS/ERROR
Instructions : It can be delivered and reported. After data is processed,
           the processing result must be reported to the APP
*****************************************************************************/
static uint8_t dp_download_find_me_handle(const uint8_t value[],
                                          uint16_t length)
{
    // Example: The current DP type is BOOL
    uint8_t ret;
    // 0:OFF/1:ON

    //paratramp.find_me_flag = 1;
#ifdef LONGQUAN
    adc1_adc2_all_flash_enable = 1;
#else
    //trigger_findme_led_blinking();
#endif
    // Feedback should be provided after DP data is processed
    ret = mcu_dp_bool_update(DPID_FIND_ME, 1);
    if (ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}

/*****************************************************************************
Function name : dp_download_light_threshold1_handle
Description   : DPID_LIGHT_THRESHOLD1 processing function
Input   : value: Data source data
        : length: Data length
Return  : SUCCESS/ERROR
Instructions : It can be delivered and reported. After data is processed,
           the processing result must be reported to the APP
*****************************************************************************/
static uint8_t dp_download_light_threshold1_handle(const uint8_t value[],
                                                         uint16_t length)
{
    // Example: The current DP type is VALUE
    uint8_t ret;
    // SEGGER_RTT_printf(0,
    //                   "%s%sload light sensor threshold1: %d%s\r\n",
    //                   RTT_CTRL_BG_BRIGHT_YELLOW,
    //                   RTT_CTRL_TEXT_BLACK,
    //                   upssa0.ppp.Light_threshold1,
    //                   RTT_CTRL_RESET);

    // Feedback should be provided after DP data is processed
    // ret = mcu_dp_value_update(DPID_LIGHT_THRESHOLD1,
    //                           upssa0.ppp.Light_threshold1);
    if (ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}
/*****************************************************************************
Function name : dp_download_light_threshold2_handle
Description   : DPID_LIGHT_THRESHOLD2 processing function
Input   : value: Data source data
        : length: Data length
Return  : SUCCESS/ERROR
Instructions : It can be delivered and reported. After data is processed,
           the processing result must be reported to the APP
*****************************************************************************/
static uint8_t dp_download_light_threshold2_handle(const uint8_t value[],
                                                   uint16_t length)
{
    // Example: The current DP type is VALUE
    uint8_t ret;
    // SEGGER_RTT_printf(0,
    //                   "%s%sload light sensor threshold2: %d%s\r\n",
    //                   RTT_CTRL_BG_BRIGHT_YELLOW,
    //                   RTT_CTRL_TEXT_BLACK,
    //                   upssa0.ppp.Light_threshold2,
    //                   RTT_CTRL_RESET);
    // Feedback should be provided after DP data is processed
    // ret = mcu_dp_value_update(DPID_LIGHT_THRESHOLD2,
    //                           upssa0.ppp.Light_threshold2);
    if (ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}
/*****************************************************************************
Function name : dp_download_light_threshold3_handle
Description   : DPID_LIGHT_THRESHOLD3 processing function
Input   : value: Data source data
        : length: Data length
Return  : SUCCESS/ERROR
Instructions : It can be delivered and reported. After data is processed,
           the processing result must be reported to the APP
*****************************************************************************/
static uint8_t dp_download_light_threshold3_handle(const uint8_t value[],
                                                         uint16_t length)
{
    // Example: The current DP type is VALUE
    uint8_t ret;

    // SEGGER_RTT_printf(0,
    //                   "%s%sload light sensor threshold3: %d%s\r\n",
    //                   RTT_CTRL_BG_BRIGHT_YELLOW,
    //                   RTT_CTRL_TEXT_BLACK,
    //                   upssa0.ppp.Light_threshold3,
    //                   RTT_CTRL_RESET);
    // Feedback should be provided after DP data is processed
    // ret = mcu_dp_value_update(DPID_LIGHT_THRESHOLD3,
    //                           upssa0.ppp.Light_threshold3);
    if (ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}
/*****************************************************************************
Function name : dp_download_light_threshold4_handle
Description   : DPID_LIGHT_THRESHOLD4 processing function
Input   : value: Data source data
        : length: Data length
Return  : SUCCESS/ERROR
Instructions : It can be delivered and reported. After data is processed,
           the processing result must be reported to the APP
*****************************************************************************/
static uint8_t dp_download_light_threshold4_handle(const uint8_t value[],
                                                         uint16_t length)
{
    // Example: The current DP type is VALUE
    uint8_t ret;

    //flash_param.light_threshold = mcu_get_dp_download_value(value, length);
    // SEGGER_RTT_printf(0,
    //                   "%s%sload light sensor threshold4: %d%s\r\n",
    //                   RTT_CTRL_BG_BRIGHT_YELLOW,
    //                   RTT_CTRL_TEXT_BLACK,
    //                   upssa0.ppp.Light_threshold4,
    //                   RTT_CTRL_RESET);
    // Feedback should be provided after DP data is processed
    //ret = mcu_dp_value_update(DPID_LIGHT_THRESHOLD4, flash_param.light_threshold);
    if (ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}


/*****************************************************************************
Function name : dp_download_common_command_handle
Description   : DPID_COMMON_COMMAND processing function
Input   : value: Data source data
        : length: Data length
Return  : SUCCESS/ERROR
Instructions : It can be delivered and reported. After data is processed,
           the processing result must be reported to the APP
*****************************************************************************/
static uint8_t dp_download_common_command_handle(const uint8_t value[],
                                                       uint16_t length)
{
    // Example: The current DP type is ENUM
    uint8_t ret;
    uint8_t common_command;


    common_command = mcu_get_dp_download_enum(value, length);
    switch (common_command) {
        case 0:   // Update detection parameters
                  // update_check_parameter();
            break;
        case 0x01:      // Save parameters to Flash
            __disable_irq();
            //Flash_Write(sizeof(flash_param), &flash_param);   // Writing to Flash (including erasing Flash)
            __enable_irq();
            break;
        case 0x02:      // Actively query the status of people
            //mcu_dp_enum_update(DPID_PERSON_IN_RANGE, paratramp.check_final);
            break;
        case 0x03:      // Proactively query slow detection results
            break;
        case 0x04:      // The report status was enabled
            //flash_param.status_upload_enable = 1;
            break;
        case 0x05:      // Prohibit reporting person status
            //flash_param.status_upload_enable = 0;
            break;
        case 0x06:      // Save current parameters to user-defined 0 presets
            // data_person_upload_enable = 0;
            break;
        case 0x07:      // Save current parameters to user-defined 1 presets
            // data_person_upload_enable = 0;
            break;
        case 0x08:      // Save current parameters to user-defined 2 presets
            // data_person_upload_enable = 0;
            break;
        case 0x09:      // Save current parameters to user-defined 3 presets
            // data_person_upload_enable = 0;
            break;
        case 0x0A:      // Save current parameters to user-defined 4 presets
            // data_person_upload_enable = 0;
            break;
        case 0x10:      // disalbe the data upload every 5s
            // data_person_upload_enable = 0;
            break;
        case 0x11:      // disalbe the data upload every 5s
            // data_person_upload_enable = 0;
            break;
        case 0x12:      // Enable the onboard LED function
            //flash_param.led_enable = 1;
            break;
        case 0x13:      // Disable the onboard LED function
            //flash_param.led_enable = 0;
            break;
        case 0x14:
            //flash_param.respiratory_upload_enable = 1;
            break;
        case 0x15:
            //flash_param.respiratory_upload_enable = 0;
            break;
        case 0x16:      // Enable to proactively report human move and static status
            //flash_param.overthreshold_upload_enable = 1;
            break;
        case 0x17:      // Disable to proactively report human move and static status
            //flash_param.overthreshold_upload_enable = 0;
            break;
        case 0x18:      // Proactively report the induction delay
            //mcu_dp_value_update(DPID_PIR_DELAY, flash_param.delay_time);
            break;
        case 0x19:      // Actively query the photosensitive threshold 3
            //mcu_dp_value_update(DPID_LIGHT_THRESHOLD3, paratramp.m_light_min);
            break;
        case 0x1A:      // Actively query the photosensitive threshold 4
            //mcu_dp_value_update(DPID_LIGHT_THRESHOLD4, flash_param.light_threshold);
            break;
        case 0x1B:      // Actively query coverage
            //mcu_dp_enum_update(DPID_LOAD_RADAR_PARAMETER, flash_param.presence_range);
            break;
        case 0x1C:
            //set_delay_time_adaptive(1);
            break;
        case 0x1D:
            //set_delay_time_adaptive(0);
            break;
        case 0x1E:
            //set_respiration_adaptive(1);
            break;
        case 0x20:      // Actively query the firmware version
            //mcu_dp_value_update(DPID_VERSION_UPLOAD, version_num);
            break;
        case 0x2B:      // Entrance range
            //mcu_dp_enum_update(DPID_ENTRANCE_PARAMETER, flash_param.entrance_range);
            break;
        case 0x30:      // Actively query the light sensitivity value
            //mcu_dp_value_update(DPID_LIGHT_SENSOR_RAW,  paratramp.light_adc_data);
            break;
        case 0x40:
            //flash_param.heartbeat_upload_enable = 1;
            break;
        case 0x41:
            //flash_param.heartbeat_upload_enable = 0;
            break;
        case 0x60:       // Test mode
            //paratramp.sampling_status = 1;
            break;
        case 0x61:       // Normal mode
            //paratramp.sampling_status  = 0;
            break;
        case 0x70:       // Enable active report of test data
            //paratramp.test_result_upload = 1;
            break;
        case 0x71:       // Disable active report of test data
            //paratramp.test_result_upload = 0;
            break;
        case 0x72:
            //paratramp.trigger_breakin_calc_once = 1;
            break;
        case 0x73:
            //paratramp.trigger_exist_calc_once = 1;
            break;
        case 0x74:
            //mcu_dp_enum_update(DPID_CHECK_STATUS, get_current_detect_state());
            break;
        case 0x80:       // production and test
            //paratramp.production_and_test = 1;
            break;
        case 0x81:
            //paratramp.production_and_test = 0;
            break;
        case 0x90:
            //paratramp.instant_response_upload_enable = 1;
            break;
        case 0x91:
            //paratramp.instant_response_upload_enable = 0;
            break;
#ifdef O1_DATA_STREAM
        case 0x92:
            en_O1_data_stream(ENABLE);
            break;
        case 0x93:
            en_O1_data_stream(DISABLE);
            break;
#endif
        default:
            break;
    }
    // Feedback should be provided after DP data is processed
    ret = mcu_dp_enum_update(DPID_COMMON_COMMAND, common_command);
    if (ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}

/*****************************************************************************
Function name : dp_download_study_cmd_handle
Description   : DPID_STUDY_CMD processing function
Input   : value: Data source data
        : length: Data length
Return  : SUCCESS/ERROR
Instructions : It can be delivered and reported. After data is processed,
           the processing result must be reported to the APP
*****************************************************************************/
static uint8_t dp_download_study_cmd_handle(const uint8_t value[],
                                                  uint16_t length)
{
    // Example: The current DP type is ENUM
    uint8_t ret;
    uint8_t study_cmd;

    study_cmd = mcu_get_dp_download_enum(value, length);
    switch (study_cmd) {
    case 0:
        // LED_GREEN();
        // study_flag = 1;
        // study_mode = 0;
        break;
    case 1:
        // study_flag = 1;
        // study_mode = 1;
        break;
    case 2:
        // study_flag = 1;
        // study_mode = 2;
        break;
    case 3:
        // study_flag = 1;
        // study_mode = 3;
        break;
    default:
        break;
    }
    // Feedback should be provided after DP data is processed
    ret = mcu_dp_enum_update(DPID_STUDY_CMD, study_cmd);
    if (ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}

/*****************************************************************************
Function name : dp_download_work_mode_handle
Description   : DPID_WORK_MODE processing function
Input   : value: Data source data
        : length: Data length
Return  : SUCCESS/ERROR
Instructions : It can be delivered and reported. After data is processed,
           the processing result must be reported to the APP
*****************************************************************************/
static uint8_t dp_download_work_mode_handle(const uint8_t value[],
                                                  uint16_t length)
{
    // Example: The current DP type is ENUM
    uint8_t ret;
    uint8_t work_mode;

    work_mode = mcu_get_dp_download_enum(value, length);
    switch (work_mode) {
    case 0:
        //paratramp.g_work_mode = NORMAL;
        break;
    case 1:
        //paratramp.g_work_mode = FAST_CHECK_ONLY;    // Check fast detection only
        break;
    case 2:
        //paratramp.g_work_mode = SLOW_CHECK_ONLY;    // Check slow detection only
        break;
    default:
        break;
    }
    //paratramp.g_work_mode = work_mode;
    // Feedback should be provided after DP data is processed
    //ret = mcu_dp_enum_update(DPID_WORK_MODE, paratramp.g_work_mode);
    if (ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}


static uint8_t dp_download_upload_duty_handle(const uint8_t value[],
                                                    uint16_t length)
{
    // Example: The current DP type is VALUE
    uint8_t ret;

    //flash_param.upload_duty = mcu_get_dp_download_value(value, length);

    // Feedback should be provided after DP data is processed
    //ret = mcu_dp_value_update(DPID_UPLOAD_DUTY, flash_param.upload_duty);
    if (ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}

/******************************************************************************
                                WARNING!!!
This code is SDK internal call, please implement the function internal data according to the actual DP data
******************************************************************************/
/*****************************************************************************
Function name : dp_download_factory_op_handle
Description   : DPID_FACTORY_OP processing function
Input   : value: Data source data
        : length: Data length
Return  : SUCCESS/ERROR
Instructions : It can be delivered and reported. After data is processed,
           the processing result must be reported to the APP
*****************************************************************************/
static uint8_t dp_download_factory_op_handle(const uint8_t value[],
                                                   uint16_t length)
{
    // Example: The current DP type is ENUM
    uint8_t ret;
    uint8_t factory_op;

    factory_op = mcu_get_dp_download_enum(value, length);
    switch (factory_op) {
    case 0:     // soft reset the mcu
        soft_reset_mcu();
        break;
    case 1:     // go bootloader and fw ota
        go_bootloader_ota();
        break;
    case 2:     // tuya re-config the network
        tuya_re_config_network();
        break;
    case 3:     // tuya reset module
        tuya_reset_module();
        break;
    case 4:     // tuya retry the ota (send fail)
        tuya_retry_ota();
        break;
    case 5:     // reset_default_parameter
        reset_default_parameter();
        break;
    case 6:     // do nothing
        break;
    case 7:     // tuya Production test
        break;
    default:
        break;
    }

    // Feedback should be provided after DP data is processed
    ret = mcu_dp_enum_update(DPID_FACTORY_OP, factory_op);
    if (ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}

uint8_t dp_download_handle(uint8_t dpid,
                                const uint8_t value[],
                                uint16_t length)
{
/**************************************************************************
The current function handles the call to issue/report data
The specific function needs to implement the data processing
The processing results need to be fed back to the APP side for completion,
otherwise the APP will think that the delivery fails
***************************************************************************/
    uint8_t ret;
    switch (dpid) {
    case DPID_PIR_DELAY:
        // Induction delay processing function
        ret = dp_download_pir_delay_handle(value, length);
        break;
    case DPID_LOAD_RADAR_PARAMETER:
        // Load the default parameter handler
        ret = dp_download_load_radar_parameter_handle(value, length);
        break;
    case DPID_ENTRANCE_PARAMETER:
        ret = dp_download_load_entrance_parameter_handle(value, length);
        break;
    case DPID_COMMON_COMMAND:
        // General command processing functions
        ret = dp_download_common_command_handle(value, length);
        break;
    case DPID_STUDY_CMD:
        // Learn command processing functions
        ret = dp_download_study_cmd_handle(value, length);
        break;
    case DPID_FIND_ME:
        // Let me handle the function
        ret = dp_download_find_me_handle(value, length);
    case DPID_LIGHT_THRESHOLD1:
        // Photosensitive threshold 1 processing function
        ret = dp_download_light_threshold1_handle(value, length);
        break;
    case DPID_LIGHT_THRESHOLD2:
        // Photosensitive threshold 2 processing function
        ret = dp_download_light_threshold2_handle(value, length);
        break;
    case DPID_LIGHT_THRESHOLD3:
        // Photosensitive threshold 3 processing function
        ret = dp_download_light_threshold3_handle(value, length);
        break;
    case DPID_LIGHT_THRESHOLD4:
        // Photosensitive threshold 4 processing function
        ret = dp_download_light_threshold4_handle(value, length);
        break;
    case DPID_FACTORY_OP:
        // Factory operation handler function
        ret = dp_download_factory_op_handle(value, length);
        break;
    case DPID_WORK_MODE:
        // Detect pattern handlers
        ret = dp_download_work_mode_handle(value, length);
        break;
    case DPID_UPLOAD_DUTY:
        // communication cycle
        ret = dp_download_upload_duty_handle(value, length);
        break;
    default:
        break;
    }
    return ret;
}


/*****************************************************************************
Function name: get_download_cmd_total
Function description: Gets the sum of all DP commands
Input parameters: None
Return parameter: issue the sum of commands
Instructions: This function cannot be modified by the user
*****************************************************************************/
uint8_t get_download_cmd_total(void)
{
  return(sizeof(download_cmd) / sizeof(download_cmd[0]));
}

/* The current version of MCU SDK has a new
   support protocol interface over the previous version */
#ifdef TUYA_BCI_UART_COMMON_UNBOUND_REQ
/*****************************************************************************
Function name: bt_unbound_req
Function description: Send the unbind request to the module,
                      and the module will unbind the Bluetooth
                      connection after receiving the instruction
Input parameters: None
Return parameter: none
Instructions: The MCU calls for active untying
*****************************************************************************/
void bt_unbound_req(void)
{
    bt_uart_write_frame(TUYA_BCI_UART_COMMON_UNBOUND_REQ, 0);
}
#endif

#ifdef TUYA_BCI_UART_COMMON_RF_TEST
/*****************************************************************************
Function name: bt_rf_test_req
Function description: transmit frequency test request to the module
Input parameters: None
Return parameter: none
Instructions for use:
*****************************************************************************/
void bt_rf_test_req(void)
{
    bt_uart_write_frame(TUYA_BCI_UART_COMMON_RF_TEST, 0);
}
/*****************************************************************************
Function name: bt_rf_test_result
Function description: Bluetooth RF test feedback
Input parameter: Result: Bluetooth RF test result;0: failure /1: success
           Rssi: Successful test indicates that the Bluetooth signal strength/test failure value is meaningless
Return parameter: none
Instructions: The MCU needs to improve the function itself
*****************************************************************************/


void bt_rf_test_result(uint8_t result, signed char rssi)
{
    #error "Please improve the function by yourself and delete the line after completion"
    if (result == 0) {
    // The test failed
    } else {
    // The test was successful
    // RSSI is the signal strength,
    // which is generally greater than -70dbM \
    // and within the normal range of Bluetooth signals
    }
}
#endif

#ifdef TUYA_BCI_UART_COMMON_SEND_STORAGE_TYPE
/*****************************************************************************
Function name: bt_send_recordable_DP_data
Function description: report the recorded data
Input parameters: Type-1: Bluetooth module built-in time report -2: original data only report, no time -3: MCU built-in time report
        Dpid: former datapoint serial number
        Dptype: Corresponds to a datapoint specific data type on the open platform
        value:
        len:
Return parameter: none
Instructions: the MCU needs to improve the function itself
    It is recommended to use the cache queue. All data to be sent to the module should be put into the MCU cache queue, and the next data should be reported after one has been reported successfully. The recorded data should ensure that each data has been reported successfully
*****************************************************************************/
void bt_send_recordable_dp_data(uint8_t snedType,
                                uint8_t dpid,
                                uint8_t dpType,
                                uint8_t value[],
                                uint16_t len)
{
    #error "Please improve the function by yourself and delete the line after completion"
    if (snedType == 0x01) {         // Format 1, Bluetooth module self-report time
    } else if (snedType == 0x02) {  // Format 2, report only the original data, no time (Note: Telink docking platform does not support this format)
    } else if (snedType == 0x03) {  // Format 3, MCU own time report
    }
}
/*****************************************************************************
Function name: bt_send_recordable_dp_data_result
Function description: report the recorded data
Input parameter :result: 0 storage success, 1 storage failure
Return parameter: none
Instructions: the MCU needs to improve the function itself
*****************************************************************************/
void bt_send_recordable_dp_data_result(uint8_t result)
{
    #error "Please improve the function by yourself and delete the line after completion"
}
#ifdef TUYA_BCI_UART_COMMON_SEND_TIME_SYNC_TYPE
/*****************************************************************************
Function name: bt_send_time_sync_req
Function description: Send time synchronization request to module
Input parameter: sync_time_type
0x00- Gets 7 bytes of time time type +2
Byte time zone information
0x01- Gets 13 bytes of MS level Unix time
+ 2-byte time zone information
0x02-get 7 byte time type + 2 Byte time zone information

Return parameter: none
*****************************************************************************/
void bt_send_time_sync_req(uint8_t sync_time_type)
{
    uint16_t length = 0;

    length = set_bt_uart_byte(length, sync_time_type);

    bt_uart_write_frame(TUYA_BCI_UART_COMMON_SEND_TIME_SYNC_TYPE, length);
}
/*****************************************************************************
Function name: bt_time_sync_result
Function description: send the result of time synchronization to the module
Input parameters: result synchronization result 0 successful, other failed
        sync_time_type :time format
        Bt_time: Custom time (valid if time format 0 or 1)
        Time_zone_100: time zone
        Time_stamp_ms: timestamp (valid if it is in time format 1)
Return parameter: none
Instructions: MCU needs to improve the function on its own.
*****************************************************************************/
void bt_time_sync_result(uint8_t result,
                        uint8_t sync_time_type,
                        bt_time_struct_data_t bt_time,
                        uint16_t time_zone_100,
                        long long time_stamp_ms)
{
    #error "Please improve the function by yourself and delete the line after completion"
    if (result == 0x00) {
        // synchronization time is successful
        if (sync_time_type == 0x00 || sync_time_type == 0x02) {
            /* populate the data of custom time
               format in bt_time into the mcu clock system */
            // time_zone_100
        } else if (sync_time_type == 0x01) {
            // populate the timestamp in time_stamp_ms into the mcu clock system
            // time_zone_100
        }
    } else {
        // synchronization time failed
    }
}
#endif
#endif

#ifdef TUYA_BCI_UART_COMMON_MODIFY_ADV_INTERVAL
/*****************************************************************************
Function name: bt_modify_adv_interval_req
Function description: send a request to the module to modify the broadcast interval of the module at low power consumption
Input parameter: value * 100ms equals the broadcast interval, value (0-20 to be modified)
Return parameter: none
Instructions for use:
*****************************************************************************/
void bt_modify_adv_interval_req(uint8_t value)
{
    uint16_t length = 0;

    length = set_bt_uart_byte(length, value);
    bt_uart_write_frame(TUYA_BCI_UART_COMMON_MODIFY_ADV_INTERVAL, length);
}
/*****************************************************************************
Function name: bt_modify_adv_interval_result
Function description:Processing the result of modifying the broadcast interval
Input parameters: result synchronization result 0 successful, other failed

Return parameter: none
Instructions: MCU needs to improve the function on its own.
*****************************************************************************/
void bt_modify_adv_interval_result(uint8_t result)
{
    // #error "Please improve the function by yourself
    //         and delete the line after completion"
    if (result == 0x00) {
        // success
    } else {
        // failed
    }
}
#endif

#ifdef TUYA_BCI_UART_COMMON_TURNOFF_SYSTEM_TIME
/*****************************************************************************
Function name: bt_close_timer_req
Function description: send a request to the module to turn off the system clock (currently available on telink platform only)
Input parameters: value 0 off, 1 on
Return parameter: none
Instructions for use:
*****************************************************************************/
void bt_close_timer_req(uint8_t value)
{
    uint16_t length = 0;
    length = set_bt_uart_byte(length, value);
    bt_uart_write_frame(TUYA_BCI_UART_COMMON_TURNOFF_SYSTEM_TIME, length);
}
/*****************************************************************************
Function name: bt_close_timer_result
Function description: processing result
Input parameters: 0 successful, other failed

Return parameter: none
Instructions: MCU needs to improve the function on its own.
*****************************************************************************/
void bt_close_timer_result(uint8_t result)
{
    // #error "Please improve the function by yourself
    //         and delete the line after completion"
    if (result == 0x00) {
        // success
    } else {
        // failed
    }
}
#endif

#ifdef TUYA_BCI_UART_COMMON_ENANBLE_LOWER_POWER
/*****************************************************************************
Function name: bt_enable_lowpoer_req
Function description: send a request to enable low power consumption to the module (currently only applicable to telink platform)
Input parameters: value 0 off, 1 on
Return parameter: none
*****************************************************************************/
void bt_enable_lowpoer_req(uint8_t value)
{
    uint16_t length = 0;

    length = set_bt_uart_byte(length, value);
    bt_uart_write_frame(TUYA_BCI_UART_COMMON_ENANBLE_LOWER_POWER, length);
}
/*****************************************************************************
Function name: bt_enable_lowpoer_result
Function description: processing result
Input parameters: 0 successful, other failed

Return parameter: none
Instructions: MCU needs to improve the function on its own.
*****************************************************************************/

void bt_enable_lowpoer_result(uint8_t result)
{
    // #error "Please improve the function by yourself
    //         and delete the line after completion"
    if (result == 0x00) {
        // success
    } else {
        // failed
    }
}
#endif

#ifdef TUYA_BCI_UART_COMMON_SEND_ONE_TIME_PASSWORD_TOKEN
/*****************************************************************************
Function name: bt_send_one_time_password_token
Function description: dynamic password check
Input parameters dynamic password entered on the: value cmcu side, len 8
Return parameter: none
Instructions for use: it is used to lock the universal serial port docking dynamic password function.
*****************************************************************************/
uint8_t bt_send_one_time_password_token(uint8_t value[],
                                              uint8_t len)
{
    uint16_t length = 0;
    if (len != 8) return 0;
    length = set_bt_uart_buffer(length, value, 8);
    bt_uart_write_frame(TUYA_BCI_UART_COMMON_SEND_ONE_TIME_PASSWORD_TOKEN,
                        length);
    return 0;
}
/*****************************************************************************
Function name:bt_send_one_time_password_token_result
Function description: get the result of one-time dynamic password matching from the module
Input parameters:0x00 password check passed, 0x01 password check failed
Return parameter: none
Instructions: MCU needs to improve the function on its own.
*****************************************************************************/
void bt_send_one_time_password_token_result(uint8_t result)
{
    // #error "Please improve the function by yourself
    //         and delete the line after completion"
    if (result == 0x00) {
        // password verification passed
    } else {
        // Password check failed.
    }
}
#endif

#ifdef TUYA_BCI_UART_COMMON_ACTIVE_DISCONNECT
/*****************************************************************************
Function name: bt_disconnect_req
Function description: send a request to disconnect the Bluetooth connection to the module
Input parameters: value 0 off, 1 on
Return parameter: none
Instructions for use:
*****************************************************************************/
void bt_disconnect_req(void)
{
    bt_uart_write_frame(TUYA_BCI_UART_COMMON_ACTIVE_DISCONNECT, 0);
}
/*****************************************************************************
Function name: bt_disconnect_result
Function description: receive the result that the module is disconnected from Bluetooth
Input parameters: result result 0 successful, other failed

Return parameter: none
Instructions: MCU needs to improve the function on its own.
*****************************************************************************/
void bt_disconnect_result(uint8_t result)
{
    // #error "Please improve the function by yourself
    //         and delete the line after completion"
    if (result == 0x00) {
        // success
    } else {
        // failed
    }
}
#endif

#ifdef TUYA_BCI_UART_COMMON_MCU_SEND_VERSION
/*****************************************************************************
Function name: bt_send_mcu_ver
Function description: send the MCU version number to the module actively, mainly in order that the module can obtain the MCU version information in a more timely manner.

Return parameter: none
Instructions: MCU can be called once after initialization of serial port.
*****************************************************************************/
void bt_send_mcu_ver(void)
{
    uint16_t length = 0;
    length = set_bt_uart_buffer(length, (uint8_t *)MCU_APP_VER_NUM, 3);
    length = set_bt_uart_buffer(length, (uint8_t *)MCU_HARD_VER_NUM, 3);
    bt_uart_write_frame(TUYA_BCI_UART_COMMON_MCU_SEND_VERSION, length);
}
#endif
#ifdef TUYA_BCI_UART_COMMON_FACTOR_RESET_NOTIFY
/*****************************************************************************
Function name: bt_factor_reset_notify
Function description: notification sent to mcu after the module resumes factory settings

Return parameter: none
Instructions for use: MCU can complete the operation of restoring factory settings with MCU here.
*****************************************************************************/
void bt_factor_reset_notify(void)
{
    // #error "Please improve the function by yourself
    //         and delete the line after completion"
}
#endif

void soft_reset_mcu(void)
{
    NVIC_SystemReset();
}

void go_bootloader_ota(void)
{
    // goto bootloader
}

void tuya_re_config_network(void)
{
    mcu_reset_bt();
}

void tuya_reset_module(void)
{
    bt_unbound_req();
}

void tuya_retry_ota(void)
{
}

void reset_default_parameter(void)
{
    __disable_irq();
    //Flash_SectorErase();   /* Erase sector  62. */
    __enable_irq();

    //Flash_Init_62();

#ifdef LONGQUAN
    mcu_dp_value_update(DPID_PIR_DELAY, 64);
    mcu_dp_enum_update(DPID_ENTRANCE_PARAMETER, 4);
    mcu_dp_enum_update(DPID_LOAD_RADAR_PARAMETER, 4);
#endif
}

#ifdef LONGQUAN
/**
* @brief received zigbee net_work state handle
* @param[in] {zigbee_work_state}  zigbee current network state
* @return  void
*/
void zigbee_work_state_event(uint8_t zigbee_work_state)
{
    uint16_t length = 0;
    bt_uart_write_frame(ZIGBEE_STATE_CMD, length);

#ifdef LONGQUAN
    switch (zigbee_work_state) {
    case ZIGBEE_NOT_JION:
        GPIO4_HIGH();    // adc1 led on
        break;

    case ZIGBEE_JOIN_GATEWAY:
        all_data_update();
        adc1_flash_enable = 1;
        break;

    case ZIGBEE_JOIN_ERROR:
        GPIO4_HIGH();    // adc1 led on
        break;

    case ZIGBEE_JOINING:
        adc1_flash_slow_enable = 1;
        break;

    default:
        break;
    }
#else
    switch (zigbee_work_state) {
    case ZIGBEE_NOT_JION:
        GPIO4_LOW();
        GPIO5_HIGH();
        break;

    case ZIGBEE_JOIN_GATEWAY:
        all_data_update();
        GPIO4_HIGH();
        GPIO5_LOW();
        break;

    case ZIGBEE_JOIN_ERROR:
        GPIO4_HIGH();
        GPIO5_HIGH();
        break;

    case ZIGBEE_JOINING:
        GPIO4_LOW();
        GPIO5_LOW();
        break;

    default:
        break;
    }
#endif
}


/**
* @brief received reset zigbee response
* @param[in] {state} response state
* @return  void
*/
void mcu_reset_zigbee_event(uint8_t state)
{
    switch (state) {
    case RESET_ZIGBEE_OK:
        break;

    case RESET_ZIGBEE_ERROR:
        break;

    default:
        break;
    }
}


/**
* @brief check mcu version response
* @param[in] {void}
* @return  void
*/
void response_mcu_ota_version_event(void)
{
    uint16_t length = 0;
    length = set_bt_uart_byte(length,     // current fw version
                              get_current_mcu_fw_ver());
    bt_uart_write_frame(MCU_OTA_VERSION_CMD, length);
}


#ifdef SUPPORT_MCU_OTA
/**
* @brief mcu ota update notify response
* @param[in] {offset} data offset
* @return  void
*/
void response_mcu_ota_notify_event(uint8_t offset, uint8_t cmd)
{
    my_memset(&msa, 0, sizeof(msa));

    int i = 0;
    while (i < 8) {
        msa.ppp.mcu_ota_fw_pid[i] = bt_uart_rx_buf[offset + DATA_START + i];
        i++;
    }

    msa.ppp.mcu_ota_fw_ver = bt_uart_rx_buf[offset + DATA_START + 8];
    msa.ppp.mcu_ota_fw_size = bt_uart_rx_buf[offset + DATA_START + 9] << 24 | \
                              bt_uart_rx_buf[offset + DATA_START + 10] << 16 | \
                              bt_uart_rx_buf[offset + DATA_START + 11] << 8 | \
                              bt_uart_rx_buf[offset + DATA_START + 12];
    msa.ppp.mcu_ota_checksum = bt_uart_rx_buf[offset + DATA_START + 13] << 24 |\
                               bt_uart_rx_buf[offset + DATA_START + 14] << 16 |\
                               bt_uart_rx_buf[offset + DATA_START + 15] << 8 | \
                               bt_uart_rx_buf[offset + DATA_START + 16];
    msa.ppp.mcu_current_offset = 0x00;

    go_bootloader_ota_zigbee(cmd);
}

void soft_reset_mcu_zigbee(void)
{
    NVIC_SystemReset();
}

void go_bootloader_ota_zigbee(uint8_t cmd)
{
    write_ota_flag_zigbee(cmd);
    soft_reset_mcu_zigbee();
}

void write_ota_flag_zigbee(uint8_t cmd)
{
    uint32_t u32Addr;

    msa.ppp.magic_byte = cmd;

    EFM_Unlock();

    EFM_FlashCmd(Enable);

    while (Set != EFM_GetFlagStatus(EFM_FLAG_RDY)) {}
    EFM_SectorErase(FLASH_SECTOR61_ADDR);

    u32Addr = FLASH_SECTOR61_ADDR;

    for (int i = 0u; i < 7u; i++) {
        EFM_SingleProgram(u32Addr, msa.int_value[i]);
        u32Addr += 4u;
    }

    EFM_Lock();
}

/**
* @brief received mcu ota data request response
* @param[in] {fw_offset}  offset of file
* @param[in] {data}  received data
* @return  void
*/
void reveived_mcu_ota_data_handle(unsigned int fw_offset, char *data)
{
    // #error "received frame data, should save in flash,
    //         mcu should realize this fuction, and delete this line "
}

/**
* @brief mcu send ota data request
* @param[in] {void}
* @return  void
*/
void mcu_ota_fw_request_event(uint8_t offset)
{
}

/**
* @brief mcu ota data result notify
* @param[in] {void}
* @return  void
*/
void mcu_ota_result_event(uint8_t offset)
{
    uint8_t status = bt_uart_rx_buf[offset + DATA_START];

    if (status == 0x00) {}
    else if (status == 0x01) {}
}

/**
* @brief mcu ota data handle
* @param[in] {fw_offset} frame offset
* @param[in] {data} received data
* @return  void
*/
void ota_fw_data_handle(unsigned int fw_offset, char *data)
{
    // #error "Please process the firmware package data
    //         in this function and delete the line"
}
#endif

#endif
