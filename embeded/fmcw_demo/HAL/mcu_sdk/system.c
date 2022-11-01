/****************************************Copyright (c)*************************
**                               Copyright (C) 2014-2020, Tuya Inc., All Rights Reserved
**
**                                 http:// www.tuya.com
**
**--------------File Infor mation-------------------------------------------------------

**file name :system.c
**description:data processing function.
**instructions for use : The user does not need to care about the content of the file.
**
**
**--------------Revision record---------------------------------------------------
** version: v1.0
** date : may 3, 2017
description: Initial version
**

**version::v2.0
** date: March 23, 2020
** description:
1. Added module unbinding interface support, command code 0x09.
2.Add rf RF test interface support, command code 0x0e.
3.Add record-based data reporting interface support, command code 0xe0.
4. Added access to real-time time API support, command code 0xe1.
5. Added support for modif ying sleep mode state bluetooth broadcast interval, command code 0xe2.
6. Added support for turning off system clock, command code 0xe4.
7. Increase low power consumption to enable support, commadn code 0xe5.
8. Add dynamic password authentication interface support, command code 0xe6.
9. Added support for disconnecting Bluetooth connection, command code 0xe7.
10. Added support for querying MCU version number, command code 0xe8.
11. Added support for MCU to actively send version Numbers, command code 0xe9.
12. Add OTA upgrade request support, command code 0xea.
13. Add OTA update file infor mation support, command 0xeb.
14. Add OTA upgrade file migration request support, command code 0xec.
15. Add OTA upgrade data support, command code 0xed.
16. Add OTA upgrade end support, command code 0xee.
17. Added support for MCU to acquire module version infor mation, commadn code 0xa0.
18. Added support for resuming factory Settings notifications, command code 0xa1.
19. Add MCU OTA demo code.
20. Optimized bt_uart_service.
**
**-----------------------------------------------------------------------------
******************************************************************************/

#define SYSTEM_GLOBAL

#include "bluetooth.h"
#include "sys.h"

extern const DOWNLOAD_CMD_S download_cmd[];

#ifdef LONGQUAN
static volatile uint16_t global_seq_num;

/**
* @brief get frame seq_num
* @param[in] {void}
* @return seq_num
*/
static uint16_t seq_num_get(void)
{
    global_seq_num++;
    if (global_seq_num > 0xfff0) {
        global_seq_num = 1;
    }
    return global_seq_num;
}
#endif

/*****************************************************************************
Function name: set_bt_uart_byte
Function description: Writes 1 byte to BT_UART
Input parameters: dest: the actual address of the buffer area;
           byte:Write byte value
Return parameter: the total length after writing
*****************************************************************************/
uint16_t set_bt_uart_byte(uint16_t dest, uint8_t byte)
{
    uint8_t *obj = (uint8_t *)bt_uart_tx_buf + DATA_START + dest;

    *obj = byte;
    dest += 1;

    return dest;
}
/*****************************************************************************
Function name: set_bt_uart_buffer
Function description: Writes a buffer to BT_UART
Input parameter: dest: destination address
           src:source address
           len:Data length
Return parameter: none
*****************************************************************************/
uint16_t set_bt_uart_buffer(uint16_t dest, uint8_t *src, uint16_t len)
{
    uint8_t *obj = (uint8_t *)bt_uart_tx_buf + DATA_START + dest;

    my_memcpy(obj, src, len);
    dest += len;

    return dest;
}
/*****************************************************************************
Function name: bt_uart_write_data
Function description: Writes continuous data to BT UART
Input parameters: in:  buffer pointer
           len: data length
Return parameter: none
*****************************************************************************/
static void bt_uart_write_data(uint8_t *in, uint16_t len)
{
    if ((NULL == in) || (0 == len))
        return;
    while (len--) {
        uart_transmit_output(*in);
        in++;
    }
}
/*****************************************************************************
Function name:get_check_sum
Function description: calculate checksum
Input parameter: pack: data source pointer
           pack_len:data length
Return parameter: checksum
*****************************************************************************/
uint8_t get_check_sum(uint8_t *pack, uint16_t pack_len)
{
    uint16_t i;
    uint8_t check_sum = 0;

    for (i = 0; i < pack_len; i++) {
      check_sum += *pack++;
    }

    return check_sum;
}
/*****************************************************************************
Function name: bt_uart_write_frame
Function description: Send a frame of data to the bt uart port
Input parameter: fr_type:frame type
           len:data length
Return parameter: none
*****************************************************************************/
void bt_uart_write_frame(uint8_t fr_type, uint16_t len)
{
    uint8_t check_sum = 0;
#ifdef LONGQUAN
    uint16_t  seq;
#endif
    bt_uart_tx_buf[HEAD_FIRST] = 0x55;
    bt_uart_tx_buf[HEAD_SECOND] = 0xaa;
    bt_uart_tx_buf[PROTOCOL_VERSION] = VERSION;

#ifdef LONGQUAN
    seq = seq_num_get();
    bt_uart_tx_buf[SEQ_HIGH] = (seq << 8);
    bt_uart_tx_buf[SEQ_LOW] = (seq & 0xff);
#endif

    bt_uart_tx_buf[FRAME_TYPE] = fr_type;
    bt_uart_tx_buf[LENGTH_HIGH] = len >> 8;
    bt_uart_tx_buf[LENGTH_LOW] = len & 0xff;

    len += PROTOCOL_HEAD;
    check_sum = get_check_sum((uint8_t *)bt_uart_tx_buf, len - 1);
    bt_uart_tx_buf[len - 1] = check_sum;

    bt_uart_write_data((uint8_t *)bt_uart_tx_buf, len);
}
/*****************************************************************************
Function name: heat_beat_check
Function description: Heartbeat packet detection
Input parameters: none
Return parameter: none
*****************************************************************************/
static void heat_beat_check(void)
{
    uint8_t length = 0;
    static uint8_t mcu_reset_state = FALSE;

    if (FALSE == mcu_reset_state) {
        length = set_bt_uart_byte(length, FALSE);
        mcu_reset_state = TRUE;
    } else {
        length = set_bt_uart_byte(length, TRUE);
    }

    bt_uart_write_frame(HEAT_BEAT_CMD, length);
}
/*****************************************************************************
Function name: product_info_update
Function description: upload product infor mation
Input parameters: none
Return parameter: none
*****************************************************************************/
static void product_info_update(void)
{
    uint8_t length = 0;
#ifdef LONGQUAN
    length = set_bt_uart_buffer(length, "{\"p\":\"", my_strlen("{\"p\":\""));
    length = set_bt_uart_buffer(length,
                                (uint8_t *)PRODUCT_KEY,
                                my_strlen((uint8_t *)PRODUCT_KEY));
    length = set_bt_uart_buffer(length,
                                "\", \"v\":\"",
                                my_strlen("\", \"v\":\""));
    length = set_bt_uart_buffer(length,
                                (uint8_t *)MCU_VER,
                                my_strlen((uint8_t *)MCU_VER));
    length = set_bt_uart_buffer(length, "\"}", my_strlen("\"}"));
#else
    length = set_bt_uart_buffer(length,
                                (uint8_t *)PRODUCT_KEY,
                                my_strlen((uint8_t *)PRODUCT_KEY));
    length = set_bt_uart_buffer(length,
                                (uint8_t *)MCU_VER,
                                my_strlen((uint8_t *)MCU_VER));
#endif
    bt_uart_write_frame(PRODUCT_INFO_CMD, length);
}
/*****************************************************************************
Function name: get_mcu_bt_mode
Function description: query the working mode of mcu and bt
Input parameters: none
Return parameter: none
*****************************************************************************/
static void get_mcu_bt_mode(void)
{
    uint8_t length = 0;

#ifdef BT_CONTROL_SELF_MODE                        // Module self-processing
    length = set_bt_uart_byte(length, BT_STATE_KEY);
    length = set_bt_uart_byte(length, BT_RESERT_KEY);
#else
  // No need to process data
#endif

    bt_uart_write_frame(WORK_MODE_CMD, length);
}
/*****************************************************************************
Function name: get_update_dpid_index
Function description: Get the serial number of DPID in the array
Input parameters: dpid:dpid
Return parameter: index: DP serial number
*****************************************************************************/
static uint8_t get_dowmload_dpid_index(uint8_t dpid)
{
    uint8_t index;
    uint8_t total = get_download_cmd_total();

    for (index = 0; index < total; index++) {
        if (download_cmd[index].dp_id == dpid)
            break;
    }
    return index;
}
/*****************************************************************************
Function name: data_point_handle
Function description: send data processing
Input parameter:
    value: the pointer of the data source issued
Return parameter:
    ret: return data processing result
*****************************************************************************/
static uint8_t data_point_handle(const uint8_t value[])
{
    uint8_t dp_id, index;
    uint8_t dp_type;
    uint8_t ret;
    uint16_t dp_len;

    dp_id = value[0];
    dp_type = value[1];
    dp_len = value[2] * 0x100;
    dp_len += value[3];

    index = get_dowmload_dpid_index(dp_id);

    if (dp_type != download_cmd[index].dp_type) {
        // Error message
        return FALSE;
    } else {
        ret = dp_download_handle(dp_id, value + 4, dp_len);
    }

    return ret;
}
/*****************************************************************************
Function name: data_handle
Function description: data frame processing
Input parameter:
    Offset: Data start bit
Return parameter: none
*****************************************************************************/
#ifdef LONGQUAN
void data_handle(uint16_t offset)
{
    uint8_t cmd_type = 0;
    uint16_t total_len = 0, seq_num = 0;
    uint16_t dp_len;
    uint8_t ret, current_state;
    uint16_t i;

    cmd_type  = bt_uart_rx_buf[offset + FRAME_TYPE];

    total_len  = bt_uart_rx_buf[offset + LENGTH_HIGH] * 0x100;
    total_len += bt_uart_rx_buf[offset + LENGTH_LOW];

    seq_num = bt_uart_rx_buf[offset + SEQ_HIGH] << 8;
    seq_num += bt_uart_rx_buf[offset + SEQ_LOW];

    switch (cmd_type) {
    case PRODUCT_INFO_CMD:
        product_info_update();
        break;

    case ZIGBEE_STATE_CMD:
        current_state = bt_uart_rx_buf[offset + DATA_START];
        zigbee_work_state_event(current_state);
        break;
    case ZIGBEE_CFG_CMD:
        // mcu_reset_zigbee_event(bt_uart_rx_buf[offset + DATA_START]);
        break;

    case ZIGBEE_DATA_REQ_CMD:
        for (i = 0; i < total_len; ) {
            dp_len = bt_uart_rx_buf[offset + DATA_START + i + 2] * 0x100;
            dp_len += bt_uart_rx_buf[offset + DATA_START + i + 3];
            ret = data_point_handle((uint8_t *)bt_uart_rx_buf + offset + DATA_START + i);
            if (SUCCESS == ret) {
            } else {
            }
            i += (dp_len + 4);
        }
        break;
    case DATA_DATA_RES_CMD:
    case DATA_REPORT_CMD:
        break;

    case QUERY_KEY_INFO_CMD:
        break;

    case CALL_SCENE_CMD:
        break;

    case ZIGBEE_RF_TEST_CMD:
        break;

    case MCU_OTA_VERSION_CMD:
        response_mcu_ota_version_event();
        break;
#ifdef SUPPORT_MCU_OTA
    case MCU_OTA_NOTIFY_CMD:
        response_mcu_ota_notify_event(offset, cmd_type);
        break;

    case MCU_OTA_DATA_REQUEST_CMD:
        break;

    case MCU_OTA_RESULT_CMD:
        break;
#endif

    case CHECK_MCU_TYPE_CMD:
#ifdef CHECK_MCU_TYPE
        response_mcu_type();
#endif
        break;

    case TIME_GET_CMD:
#ifdef SUPPORT_MCU_RTC_CHECK
        mcu_write_rtctime((uint8_t *)(bt_uart_rx_buf + offset + DATA_START));
#endif
        break;

#ifdef BEACON_TEST
    case SEND_BEACON_NOTIFY_CMD:
        mcu_received_beacon_test_handle();
        break;
#endif
#ifdef READ_DP_DATA_NOTIFY
    case SEND_BEACON_NOTIFY_CMD:
        zigbee_notify_factory_new_hanlde();
        break;
#endif
#ifdef FACTORY_NEW_NOTIFY
    case SEND_BEACON_NOTIFY_CMD:
        zigbee_notify_factory_new_hanlde();
        break;
#endif
    default:
        break;
    }
}
#else
void data_handle(uint16_t offset)
{
#ifdef SUPPORT_MCU_FIRM_UPDATE
    unsigned long dp_len;
#else
    uint16_t dp_len;
#endif
    uint8_t length = 0;
    uint8_t ret;
    uint16_t i, total_len;
    uint8_t cmd_type = bt_uart_rx_buf[offset + FRAME_TYPE];
    // signed char bt_rssi;
#ifdef TUYA_BCI_UART_COMMON_SEND_TIME_SYNC_TYPE
    bt_time_struct_data_t bt_time;
    uint16_t time_zone_100;
    char current_timems_string[14] = "000000000000";
    long long time_stamp_ms;
#endif

    switch (cmd_type) {
    case HEAT_BEAT_CMD:            // Heartbeat package
        heat_beat_check();
        break;

    case PRODUCT_INFO_CMD:         // product infor mation
        product_info_update();
        break;

    case WORK_MODE_CMD:            // Query module working mode set by MCU
        get_mcu_bt_mode();
        break;

#ifndef BT_CONTROL_SELF_MODE
    case BT_STATE_CMD:                                  // bt work state
        bt_work_state = bt_uart_rx_buf[offset + DATA_START];
        if (bt_work_state == 0x01 || bt_work_state == 0x00) {
            // mcu_ota_init_disconnect();
        }
        bt_uart_write_frame(BT_STATE_CMD, 0);
        break;

      case BT_RESET_CMD:                 // Reset BT (BT returns success)
          reset_bt_flag = RESET_BT_SUCCESS;
          break;
#endif

    case DATA_QUERT_CMD:                     // dp data handled
        total_len = bt_uart_rx_buf[offset + LENGTH_HIGH] * 0x100;
        total_len += bt_uart_rx_buf[offset + LENGTH_LOW];

        for (i = 0; i < total_len; ) {
            dp_len = bt_uart_rx_buf[offset + DATA_START + i + 2] * 0x100;
            dp_len += bt_uart_rx_buf[offset + DATA_START + i + 3];
            ret = data_point_handle((uint8_t *)bt_uart_rx_buf + offset + DATA_START + i);
            if (SUCCESS == ret) {
                // Success tips
            } else {
                // Error message
            }
            i += (dp_len + 4);
        }
        break;

    case STATE_QUERY_CMD:                     // Status query
        all_data_update();
        break;

#ifdef TUYA_BCI_UART_COMMON_RF_TEST
    case TUYA_BCI_UART_COMMON_RF_TEST:
        if (my_memcmp((uint8_t *)bt_uart_rx_buf + offset + DATA_START+7,
            "true", 4) == 0) {
            bt_rssi = (bt_uart_rx_buf[offset + DATA_START+21]-'0')*10 + \
                      (bt_uart_rx_buf[offset + DATA_START+22]-'0');
            bt_rssi = -bt_rssi;
            bt_rf_test_result(1, bt_rssi);
        } else {
            bt_rf_test_result(0, 0);
        }
        break;
#endif

#ifdef TUYA_BCI_UART_COMMON_SEND_STORAGE_TYPE
    case TUYA_BCI_UART_COMMON_SEND_STORAGE_TYPE:
        bt_send_recordable_dp_data_result(bt_uart_rx_buf[offset + DATA_START]);
        break;
#endif

#ifdef TUYA_BCI_UART_COMMON_SEND_TIME_SYNC_TYPE
    case TUYA_BCI_UART_COMMON_SEND_TIME_SYNC_TYPE:
        ret = bt_uart_rx_buf[offset + DATA_START];
        if (ret == 0) {     // Get time succeeded
            // Time for mat 0 :Get 7 bytes of time and time type + 2 bytes of time zone infor mation
            if (bt_uart_rx_buf[offset + DATA_START+1] == 0x00) {
                bt_time.nYear = bt_uart_rx_buf[offset + DATA_START+2] + 2018;
                bt_time.nMonth = bt_uart_rx_buf[offset + DATA_START+3];
                bt_time.nDay = bt_uart_rx_buf[offset + DATA_START+4];
                bt_time.nHour = bt_uart_rx_buf[offset + DATA_START+5];
                bt_time.nMin = bt_uart_rx_buf[offset + DATA_START+6];
                bt_time.nSec = bt_uart_rx_buf[offset + DATA_START+7];
                bt_time.DayIndex = bt_uart_rx_buf[offset + DATA_START+8];
                time_zone_100 = ((uint16_t)bt_uart_rx_buf[offset + DATA_START+9] << 8) + \
                                 bt_uart_rx_buf[offset + DATA_START + 10];
            } else if (bt_uart_rx_buf[offset + DATA_START+1] == 0x01) {    // Time for mat 1: Get 13 bytes of ms-level unix time + 2 bytes of time zone infor mation
                my_memcpy(current_timems_string,
                          &bt_uart_rx_buf[offset + DATA_START+2],
                          13);
                time_stamp_ms = my_atoll(current_timems_string);
                time_zone_100 = ((uint16_t)bt_uart_rx_buf[offset + DATA_START+15] < 8) + \
                                 bt_uart_rx_buf[offset + DATA_START+16];
            } else if (bt_uart_rx_buf[offset + DATA_START+1] == 0x02) {        // Time for mat 2: Get 7 bytes of time and time type + 2 bytes of time zone infor mation
                bt_time.nYear = bt_uart_rx_buf[offset + DATA_START+2] + 2000;
                bt_time.nMonth = bt_uart_rx_buf[offset + DATA_START+3];
                bt_time.nDay = bt_uart_rx_buf[offset + DATA_START+4];
                bt_time.nHour = bt_uart_rx_buf[offset + DATA_START+5];
                bt_time.nMin = bt_uart_rx_buf[offset + DATA_START+6];
                bt_time.nSec = bt_uart_rx_buf[offset + DATA_START+7];
                bt_time.DayIndex = bt_uart_rx_buf[offset + DATA_START+8];
                time_zone_100 = ((uint16_t)bt_uart_rx_buf[offset + DATA_START+9] << 8) + \
                                 bt_uart_rx_buf[offset + DATA_START+10];
            }
            bt_time_sync_result(0,
                                bt_uart_rx_buf[offset + DATA_START+1],
                                bt_time, time_zone_100,
                                time_stamp_ms);
        } else {   // Failed to get time
            bt_time_sync_result(1,
                                bt_uart_rx_buf[offset + DATA_START+1],
                                bt_time, time_zone_100,
                                time_stamp_ms);
        }
        break;
#endif

#ifdef TUYA_BCI_UART_COMMON_MODIFY_ADV_INTERVAL
    case TUYA_BCI_UART_COMMON_MODIFY_ADV_INTERVAL:
        bt_modify_adv_interval_result(bt_uart_rx_buf[offset + DATA_START]);
        break;
#endif
#ifdef TUYA_BCI_UART_COMMON_TURNOFF_SYSTEM_TIME
    case TUYA_BCI_UART_COMMON_TURNOFF_SYSTEM_TIME:
        bt_close_timer_result(bt_uart_rx_buf[offset + DATA_START]);
        break;
#endif
#ifdef TUYA_BCI_UART_COMMON_ENANBLE_LOWER_POWER
    case TUYA_BCI_UART_COMMON_ENANBLE_LOWER_POWER:
        bt_enable_lowpoer_result(bt_uart_rx_buf[offset + DATA_START]);
        break;
#endif
#ifdef TUYA_BCI_UART_COMMON_SEND_ONE_TIME_PASSWORD_TOKEN
    case TUYA_BCI_UART_COMMON_SEND_ONE_TIME_PASSWORD_TOKEN:
        bt_send_one_time_password_token_result(bt_uart_rx_buf[offset + DATA_START]);
        break;
#endif
#ifdef TUYA_BCI_UART_COMMON_ACTIVE_DISCONNECT
    case TUYA_BCI_UART_COMMON_ACTIVE_DISCONNECT:
        bt_disconnect_result(bt_uart_rx_buf[offset + DATA_START]);
        break;
#endif
#ifdef TUYA_BCI_UART_COMMON_QUERY_MCU_VERSION
    case TUYA_BCI_UART_COMMON_QUERY_MCU_VERSION:
        length = set_bt_uart_buffer(length, (uint8_t *)MCU_APP_VER_NUM, 3);
        length = set_bt_uart_buffer(length, (uint8_t *)MCU_HARD_VER_NUM, 3);
        bt_uart_write_frame(TUYA_BCI_UART_COMMON_QUERY_MCU_VERSION, length);
        break;
#endif
#ifdef TUYA_BCI_UART_COMMON_FACTOR_RESET_NOTIFY
    case TUYA_BCI_UART_COMMON_FACTOR_RESET_NOTIFY:
        bt_factor_reset_notify();
        break;
#endif
#ifdef SUPPORT_MCU_FIRM_UPDATE
      case TUYA_BCI_UART_COMMON_MCU_OTA_REQUEST:
      case PHOSENSE_BCI_UART_COMMON_MCU_OTA_REQUEST:
        total_len = bt_uart_rx_buf[offset + LENGTH_HIGH] * 0x100;
        total_len += bt_uart_rx_buf[offset + LENGTH_LOW];
        mcu_ota_proc(cmd_type, &bt_uart_rx_buf[offset + DATA_START], total_len);
          break;
#endif

    default:
        break;
    }
}

#endif
/*****************************************************************************
Function name:get_queue_total_data
Function description: read data in the queue
Input parameters: none
Return parameter: none
*****************************************************************************/
uint8_t get_queue_total_data(void)
{
    if (queue_in != queue_out)
        return 1;
    else
        return 0;
}
/*****************************************************************************
Function name:Queue_Read_Byte
Function description: Read 1 byte data of queue
Input parameters: none
Return parameter: none
*****************************************************************************/
uint8_t Queue_Read_Byte(void)
{
    uint8_t value;

    if (queue_out != queue_in) {
        // Data is not empty
        if (queue_out >= (uint8_t *)(bt_queue_buf + sizeof(bt_queue_buf))) {
            // Data has reached the end
            queue_out = (uint8_t *)(bt_queue_buf);
        }
        value = *queue_out++;
    }
    return value;
}

#ifdef LONGQUAN
/**
* @brief mcu version string to char
* @param[in] {void}
* @return  result of version
*/
uint8_t get_current_mcu_fw_ver(void)
{
    uint8_t *fw_ver = (uint8_t*) MCU_VER;    // Current version
    uint8_t current_mcu_fw_ver = 0;
    current_mcu_fw_ver = assic_to_hex(fw_ver[0]) << 6;    // high ver
    current_mcu_fw_ver |= assic_to_hex(fw_ver[2]) << 4;    // mid ver
    current_mcu_fw_ver |= assic_to_hex(fw_ver[4]);    // low ver
    return current_mcu_fw_ver;
}

/**
* @brief translate assic to hex
* @param[in] {assic_num} assic number
* @return hex data
*/
int assic_to_hex(uint8_t assic_num)
{
    if (assic_num < 0x30 && assic_num > 0x39)    // 0~9
        return -1;
    else
        return assic_num % 0x30;
}
#endif
