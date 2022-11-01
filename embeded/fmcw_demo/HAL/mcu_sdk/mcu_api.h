/****************************************Copyright (c)*************************
**                               Copyright (C) 2014-2020, Tuya Inc., All Rights Reserved
**
**                                 http://www.tuya.com
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
#ifndef __MCU_API_H_
#define __MCU_API_H_


#ifdef MCU_API_GLOBAL
  #define MCU_API_EXTERN
#else
  #define MCU_API_EXTERN   extern
#endif
/*****************************************************************************
Function name: hex_to_bcd
Function description: hex to bcd
Input parameters:
    Value_H: high byte / Value_L: low byte
Return value:
    bcd_value:Data after conversion is completed.
*****************************************************************************/
uint8_t hex_to_bcd(uint8_t Value_H, uint8_t Value_L);
/*****************************************************************************
Function name: my_strlen
Function description: find the length of the string
Input parameter:
    src: source address
Return parameter:
    len: data length

*****************************************************************************/
unsigned long my_strlen(uint8_t *str);

/*****************************************************************************
Function name: my_memset
Function description: set the first count bytes of the memory area referred to by src to the character ch
Input parameter: src: source address
           ch: setting character
           count:Operation data length
Return parameters :
    src:source address after data processing
*****************************************************************************/
void *my_memset(void *src, uint8_t ch, uint16_t count);

/*****************************************************************************
Function name: my_memcpy
Function description: memory copy
Input parameter: dest: destination address
           src:Source address
           count:Number of data copies
 Return parameters :
     src:Source address after data processing
*****************************************************************************/
void *my_memcpy(void *dest,  const void *src,  uint16_t count);
/*****************************************************************************
Function name: my_memcmp
Function description: memory comparison
Input parameter: buffer1: memory 1
           buffer2:memory 2
               count:Comparative length
Return parameters :0:buffer1=buffer2; -1:buffer1<buffer2; 1:buffer1>buffer2
*****************************************************************************/
int my_memcmp(const void *buffer1, const void *buffer2, int count);
/*****************************************************************************
Function name: my_atoll
Function description: string to integer
Input parameter: P string
Return parameter: integer
*****************************************************************************/
long long my_atoll(const char *p);
/*****************************************************************************
Function name: int_to_byte
Function description: split the int type into four bytes
Input parameter:
    number:Number to be processed
    value:4 bytes of data after processing
Return parameter: none
****************************************************************************/
void int_to_byte(unsigned long number, uint8_t value[4]);

/*****************************************************************************
Function name: byte_to_int
Function description: merge 4 bytes into 1 32bit variable
Input parameters: value:4 byte array
Return parameter:
    number:32bit variables after merging
****************************************************************************/
unsigned long byte_to_int(const uint8_t value[4]);

#ifndef BT_CONTROL_SELF_MODE
/*****************************************************************************
Function name: mcu_get_reset_bt_flag
Function description: MCU acquires reset bt success flag
Input parameters: none
Return parameter: RESET_BT_ERROR:failed/RESET_BT_SUCCESS:success
Instructions for use: 1:MCU call this function after calling mcu_reset_bt () actively to get the reset state
           2: if it is in module self-processing mode, MCU does not need to call this function
*****************************************************************************/
uint8_t mcu_get_reset_bt_flag(void);
/*****************************************************************************
Function name: mcu_reset_bt
Function description: MCU actively resets bt working mode
Input parameters: none
Return parameter: none
Instructions for use: 1:MCU is called actively, and the mcu_get_reset_bt_flag () function is used to obtain whether the reset bt is successful or not.
           2: if it is in module self-processing mode, MCU does not need to call this function
*****************************************************************************/
void mcu_reset_bt(void);
/*****************************************************************************
Function name: mcu_get_bt_work_state
Function description: MCU actively acquires the current bt working status
Input parameters: none
Return parameter: BT_WORK_SATE_E:
            BT_UN_BIND:Unbound state
            BT_UNCONNECT:Bound but not connected
            BT_CONNECTED:Bound and connected
Instructions for use: none
*****************************************************************************/
uint8_t mcu_get_bt_work_state(void);
#endif

/*****************************************************************************
Function name: mcu_dp_raw_update
Function description: raw dp data upload
Return parameter: dpid:id
           value:dp date
           len:length
Return parameter: none
*****************************************************************************/
uint8_t mcu_dp_raw_update(uint8_t dpid, const uint8_t value[], uint16_t len);

/*****************************************************************************
Function name: mcu_dp_bool_update
Function description: Bool dp data upload
Input parameter: dpid:id number
           value:
Return parameter: none
*****************************************************************************/
uint8_t mcu_dp_bool_update(uint8_t dpid, uint8_t value);

/*****************************************************************************
Function name: mcu_dp_value_update
Function description: value dp data upload
Input parameter: dpid:id number
           value:
Return parameter: none
*****************************************************************************/
uint8_t mcu_dp_value_update(uint8_t dpid, unsigned long value);

/*****************************************************************************
Function name: mcu_dp_string_update
Function description: string dp data upload
Input parameter: dpid:id number
           value:current DP value pointer
           len:data length
Return parameter: none
*****************************************************************************/
uint8_t mcu_dp_string_update(uint8_t dpid, const uint8_t value[], uint16_t len);

/*****************************************************************************
Function name: mcu_dp_enum_update
Function description: enum dp data upload
Input parameter: dpid:id number
           value:
Return parameter: none
*****************************************************************************/
uint8_t mcu_dp_enum_update(uint8_t dpid, uint8_t value);

/*****************************************************************************
Function name: mcu_dp_fault_update
Function description: bitmap dp data upload
Input parameter: dpid:id number
           value:current dp value
Return parameter: none
*****************************************************************************/
uint8_t mcu_dp_fault_update(uint8_t dpid, unsigned long value);

/*****************************************************************************
Function name: mcu_get_dp_download_bool
Function description: mcu acquires the DP value issued by Bool
Input parameter: value:dp data buffer address
           length:dp data length
Return parameter: current DP value of bool
*****************************************************************************/
uint8_t mcu_get_dp_download_bool(const uint8_t value[], uint16_t len);

/*****************************************************************************
Function name: mcu_get_dp_download_value
Function description: mcu acquires the DP value issued by value
Input parameter: value:dp data buffer address
           length:dp data length
Return parameter: current DP value
*****************************************************************************/
unsigned long mcu_get_dp_download_value(const uint8_t value[], uint16_t len);

/*****************************************************************************
Function name: mcu_get_dp_download_enum
Function description: mcu acquires the DP value issued by enum
Input parameter: value:dp data buffer address
           length:dp data length
Return parameter: current DP value of eunm
*****************************************************************************/
uint8_t mcu_get_dp_download_enum(const uint8_t value[], uint16_t len);

#ifdef LONGQUAN
/**
* @brief mcu reset zigbee module
* @param[in]  {void}
* @return void
*/
void mcu_reset_zigbee(void);

/**
* @brief mcu start zigbee module
* @param[in]  {void}
* @return void
*/
void mcu_network_start(void);

/**
* @brief mcu start zigbee module test
* @param[in]  {channel} test channel usually 11
* @return void
*/
void mcu_start_zigbee_test(uint8_t channel);

void zigbee_uart_service(void);
#endif

/*****************************************************************************
Function name: uart_receive_input
Function description: receiving data processing
Input parameters: value: uart port receives byte data
Return parameter: none
Instructions for use: call this function in the MCU uart port receiving function and pass in the received data as parameters
*****************************************************************************/
void uart_receive_input(uint8_t value);

/*****************************************************************************
Function name: bt_uart_service
Function description: bt serial port processing service
Input parameters: none
Return parameter: none
Instructions: call this function in the while loop of the MCU main function
*****************************************************************************/
void bt_uart_service(void);

/*****************************************************************************
Function name: bt_protocol_init
Function description: protocol serial port initialization function
Input parameters: none
Return parameter: none
Instructions for use: this function must be called in the MCU initialization code
*****************************************************************************/
void bt_protocol_init(void);
/*****************************************************************************
Function name: uart_transmit_output
Function description: send data processing
Input parameters:
    value: uart port receives byte data
Return parameter: none
Instructions for use: please fill the MCU serial port sending function into this function,and pass the received data into the serial port sending function as parameters
*****************************************************************************/
void uart_transmit_output(uint8_t value);






#endif
