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

    bt_uart_tx_buf[HEAD_FIRST] = 0x55;
    bt_uart_tx_buf[HEAD_SECOND] = 0xaa;
    bt_uart_tx_buf[PROTOCOL_VERSION] = VERSION;

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

    length = set_bt_uart_buffer(length,
                                (uint8_t *)PRODUCT_KEY,
                                my_strlen((uint8_t *)PRODUCT_KEY));
    length = set_bt_uart_buffer(length,
                                (uint8_t *)MCU_VER,
                                my_strlen((uint8_t *)MCU_VER));

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
