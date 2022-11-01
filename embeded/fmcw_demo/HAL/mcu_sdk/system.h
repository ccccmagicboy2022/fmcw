/****************************************Copyright (c)*************************
**                               Copyright (C) 2014-2020, Tuya Inc., All Rights Reserved
**
**                                 http:// www.tuya.com
**
**--------------File Information-------------------------------------------------------

**file name :system.h
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
3.Add record-based data reporting interface support,command code 0xe0.
4. Added access to real-time time API support,command code 0xe1.
5. Added support for modifying sleep mode state bluetooth broadcast interval,command code 0xe2.
6. Added support for turning off system clock,command code 0xe4.
7. Increase low power consumption to enable support,commadn code 0xe5.
8. Add dynamic password authentication interface support,command code 0xe6.
9. Added support for disconnecting Bluetooth connection,command code 0xe7.
10. Added support for querying MCU version number,command code 0xe8.
11. Added support for MCU to actively send version Numbers,command code 0xe9.
12. Add OTA upgrade request support,command code 0xea.
13. Add OTA update file information support,command 0xeb.
14. Add OTA upgrade file migration request support,command code 0xec.
15. Add OTA upgrade data support,command code 0xed.
16. Add OTA upgrade end support,command code 0xee.
17. Added support for MCU to acquire module version information,commadn code 0xa0.
18. Added support for resuming factory Settings notifications,command code 0xa1.
19. Add MCU OTA demo code.
20. Optimized bt_uart_service.
**
**-----------------------------------------------------------------------------
******************************************************************************/
#ifndef __SYSTEM_H_
#define __SYSTEM_H_

#ifdef SYSTEM_GLOBAL
  #define SYSTEM_EXTERN
#else
  #define SYSTEM_EXTERN   extern
#endif

#ifdef LONGQUAN
#ifdef SUPPORT_MCU_OTA

union BBB
{
    unsigned int int_value[7];
    struct {
        unsigned int magic_byte;
        uint8_t mcu_ota_fw_pid[8];
        unsigned int mcu_ota_fw_ver;
        unsigned int mcu_ota_fw_size;
        unsigned int mcu_ota_checksum;
        unsigned int mcu_current_offset;
    } ppp;
};

SYSTEM_EXTERN union BBB msa;
#endif
#endif
// =============================================================================
// Byte order of the frame
// =============================================================================
#ifdef LONGQUAN
#define         HEAD_FIRST                      0
#define         HEAD_SECOND                     1
#define         PROTOCOL_VERSION                2
#define         SEQ_HIGH                        3
#define         SEQ_LOW                         4
#define         FRAME_TYPE                      5
#define         LENGTH_HIGH                     6
#define         LENGTH_LOW                      7
#define         DATA_START                      8
#else
#define         HEAD_FIRST                      0
#define         HEAD_SECOND                     1
#define         PROTOCOL_VERSION                2
#define         FRAME_TYPE                      3
#define         LENGTH_HIGH                     4
#define         LENGTH_LOW                      5
#define         DATA_START                      6
#endif
// =============================================================================
// Data frame type
// =============================================================================
#define         HEAT_BEAT_CMD                   0       // Heartbeat package
#define         PRODUCT_INFO_CMD                1       // product information
#define         WORK_MODE_CMD                   2       // Query module working mode set by MCU
#define         BT_STATE_CMD                    3       // bluetooth work state
#define         BT_RESET_CMD                    4       // reset thr bluetooth
#define         DATA_QUERT_CMD                  6       // dp data form bt moudle
#ifdef LONGQUAN
#define         STATE_UPLOAD_CMD                6
#else
#define         STATE_UPLOAD_CMD                7       // report dp data to bt moudle
#endif
#define         STATE_QUERY_CMD                 8       // query dp state

/* Compared with the previous version,
   the current MCU SDK version supports new protocol interfaces */
// If you don’t need a certain command interface, \
// just comment out the definition of the command macro, \
// and the related code will not be compiled to reduce the code space
#define TUYA_BCI_UART_COMMON_UNBOUND_REQ                0x09        // unbind
// #define TUYA_BCI_UART_COMMON_RF_TEST                   0x0E        // rf radio frequency test
// #define TUYA_BCI_UART_COMMON_SEND_STORAGE_TYPE         0xE0        // Recorded dp data report (offline cache)
// #define TUYA_BCI_UART_COMMON_SEND_TIME_SYNC_TYPE       0xE1        // Get real time
#define TUYA_BCI_UART_COMMON_MODIFY_ADV_INTERVAL        0xE2        // Modify the sleep mode broadcast interval
#define TUYA_BCI_UART_COMMON_TURNOFF_SYSTEM_TIME        0xE4        // Turn off the system clock function
#define TUYA_BCI_UART_COMMON_ENANBLE_LOWER_POWER        0xE5        // Low power enable
#define TUYA_BCI_UART_COMMON_SEND_ONE_TIME_PASSWORD_TOKEN   0xE6        // Dynamic password verification
#define TUYA_BCI_UART_COMMON_ACTIVE_DISCONNECT          0xE7        // Disconnect device Bluetooth connection

#define TUYA_BCI_UART_COMMON_QUERY_MCU_VERSION          0xE8        // Query MCU version number
#define TUYA_BCI_UART_COMMON_MCU_SEND_VERSION           0xE9        // MCU actively sends the version number

#define TUYA_BCI_UART_COMMON_MCU_OTA_REQUEST            0xEA        // OTA upgrade request
#define PHOSENSE_BCI_UART_COMMON_MCU_OTA_REQUEST        0xCA        // OTA升级请求
// #define TUYA_BCI_UART_COMMON_MCU_OTA_FILE_INFO          0xEB        // OTA upgrade request
// #define TUYA_BCI_UART_COMMON_MCU_OTA_FILE_OFFSET        0xEC        // OTA upgrade file offset request
// #define TUYA_BCI_UART_COMMON_MCU_OTA_DATA               0xED        // OTA upgrade data
// #define TUYA_BCI_UART_COMMON_MCU_OTA_END                0xEE        // OTA upgrade is over
// #define TUYA_BCI_UART_COMMON_QUERY_MOUDLE_VERSION       0xA0        // MCU obtains module version information

#define TUYA_BCI_UART_COMMON_FACTOR_RESET_NOTIFY        0xA1        // Factory reset notification


#ifdef TUYA_BCI_UART_COMMON_SEND_TIME_SYNC_TYPE
typedef struct
{
    uint16_t nYear;
    uint8_t nMonth;
    uint8_t nDay;
    uint8_t nHour;
    uint8_t nMin;
    uint8_t nSec;
    uint8_t DayIndex; /* 0 = Sunday */
} bt_time_struct_data_t;
#endif


#ifdef LONGQUAN
// /< define cmd of frame
#define ZIGBEE_FACTORY_NEW_CMD              0
#define PRODUCT_INFO_CMD                    1       // pid information
#define ZIGBEE_STATE_CMD                    2       // zigbee module send network state to mcu
#define ZIGBEE_CFG_CMD                      3       // mcu config zigbee state
#define ZIGBEE_DATA_REQ_CMD                 4       // zigbee module shend DP data to mcu
#define DATA_DATA_RES_CMD                   5       // mcu response zigbee DP send
#define DATA_REPORT_CMD                     6       // mcu report DP data to zigbee
#define ZIGBEE_RF_TEST_CMD                  8
#define DYNAMIC_TIME_CMD                    24
#define QUERY_KEY_INFO_CMD                  9
#define CALL_SCENE_CMD                      10

// /< define communication state
#define ZIGBEE_NOT_JION                     0x00  // zigbee module not jion network
#define ZIGBEE_JOIN_GATEWAY                 0x01  // zigbee module had jioned network
#define ZIGBEE_JOIN_ERROR                   0x02  // zigbee module network state error
#define ZIGBEE_JOINING                      0x03  // zigbee module jioning

#define RESET_ZIGBEE_OK                     0x00  // rest zigbee success
#define RESET_ZIGBEE_ERROR                  0x01  // reset zigbee error

#define MCU_OTA_VERSION_CMD                 0x0B   // zigbee request mcu version
#define MCU_OTA_NOTIFY_CMD                  0x0C   // mcu ota notify
#define MCU_OTA_DATA_REQUEST_CMD            0x0D   // MCU OTA data request
#define MCU_OTA_RESULT_CMD                  0x0E   // MCU OTA result report
#define CHECK_ZIGBEE_NETWORK_CMD            0x20
#define READ_MCU_DP_DATA_CMD                0x28
#define SEND_BEACON_NOTIFY_CMD              0x29

#define TIME_GET_CMD                        0x24
#define CHECK_MCU_TYPE_CMD                  0x25
#define SET_ZIGBEE_NEK_PARAMETER_CMD        0x26
#define SEND_BROADCAST_DATA_CMD             0x27
#endif

// =============================================================================
#ifdef LONGQUAN
#define         VERSION                     0x02    // Protocol version number
#define         PROTOCOL_HEAD               0x09    // Fixed protocol header length
#else
#define         VERSION                     0x00    // Protocol version number
#define         PROTOCOL_HEAD               0x07    // Fixed protocol header length
#endif
#define         FIRM_UPDATA_SIZE            200     // Upgrade package size
#define         FRAME_FIRST                 0x55
#define         FRAME_SECOND                0xaa
// =============================================================================
#ifdef LONGQUAN
SYSTEM_EXTERN uint8_t volatile bt_queue_buf[PROTOCOL_HEAD + 512];  // Serial queue buffer
SYSTEM_EXTERN uint8_t bt_uart_rx_buf[PROTOCOL_HEAD + 256];         // Serial receive buffer
#else
SYSTEM_EXTERN uint8_t volatile bt_queue_buf[PROTOCOL_HEAD + BT_UART_QUEUE_LMT];  // Serial queue buffer
SYSTEM_EXTERN uint8_t bt_uart_rx_buf[PROTOCOL_HEAD + BT_UART_RECV_BUF_LMT];      // Serial receive buffer
#endif
SYSTEM_EXTERN uint8_t bt_uart_tx_buf[PROTOCOL_HEAD + BT_UART_SEND_BUF_LMT];      // Serial transmission buffer

SYSTEM_EXTERN volatile uint8_t *queue_in;
SYSTEM_EXTERN volatile uint8_t *queue_out;

SYSTEM_EXTERN uint8_t stop_update_flag;

#ifndef BT_CONTROL_SELF_MODE
SYSTEM_EXTERN uint8_t reset_bt_flag;
SYSTEM_EXTERN uint8_t set_btmode_flag;
SYSTEM_EXTERN uint8_t bt_work_state;
#endif


/*****************************************************************************
Function name: set_bt_uart_byte
Function description: Writes 1 byte to BT_UART
Input parameters: dest: the actual address of the buffer area;
           byte:Write byte value
Return parameter: the total length after writing
*****************************************************************************/
uint16_t set_bt_uart_byte(uint16_t dest, uint8_t byte);

/*****************************************************************************
Function name: set_bt_uart_buffer
Function description: Writes a buffer to BT_UART
Input parameter: dest: destination address
           src:source address
           len:Data length
Return parameter: none
*****************************************************************************/
uint16_t set_bt_uart_buffer(uint16_t dest, uint8_t *src, uint16_t len);

/*****************************************************************************
Function name: bt_uart_write_frame
Function description: Send a frame of data to the bt uart port
Input parameter: fr_type:frame type
           len:data length
Return parameter: none
*****************************************************************************/
void bt_uart_write_frame(uint8_t fr_type, uint16_t len);
/*****************************************************************************
Function name:get_check_sum
Function description: calculate checksum
Input parameter: pack: data source pointer
           pack_len:data length
Return parameter: checksum
*****************************************************************************/
uint8_t get_check_sum(uint8_t *pack, uint16_t pack_len);

/*****************************************************************************
Function name: data_handle
Function description: data frame processing
Input parameter:
  Offset: Data start bit
Return parameter: none
*****************************************************************************/
void data_handle(uint16_t offset);

/*****************************************************************************
Function name:get_queue_total_data
Function description: read data in the queue
Input parameters: none
Return parameter: none
*****************************************************************************/
uint8_t get_queue_total_data(void);

/*****************************************************************************
Function name:Queue_Read_Byte
Function description: Read 1 byte data of queue
Input parameters: none
Return parameter: none
*****************************************************************************/
uint8_t Queue_Read_Byte(void);

#ifdef LONGQUAN
/**
* @brief mcu version string to char
* @param[in] {void}
* @return  result of version
*/
uint8_t get_current_mcu_fw_ver(void);

/**
* @brief translate assic to hex
* @param[in] {assic_num} assic number
* @return hex data
*/
int assic_to_hex(uint8_t assic_num);

#endif

#endif
