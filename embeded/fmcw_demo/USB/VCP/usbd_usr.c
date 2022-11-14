/**
  ******************************************************************************
  * @file    usbd_usr.c
  * @author  MCD Application Team
  * @version V1.2.1
  * @date    17-March-2018
  * @brief   This file includes the user application layer
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2015 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                      <http://www.st.com/SLA0044>
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------ */
#include "usbd_usr.h"
#include "sys.h"

uint8_t usb_ok_flag;

void wait_usb_init_ok(void)
{
    while(get_usb_ok() == 0)
    {
        //pass
    }
}

uint8_t get_usb_ok(void)
{
    return usb_ok_flag;
}
/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
* @{
*/

/** @defgroup USBD_USR 
* @brief    This file includes the user application layer
* @{
*/

/** @defgroup USBD_USR_Private_TypesDefinitions
* @{
*/
/**
* @}
*/


/** @defgroup USBD_USR_Private_Defines
* @{
*/
/**
* @}
*/


/** @defgroup USBD_USR_Private_Macros
* @{
*/
/**
* @}
*/


/** @defgroup USBD_USR_Private_Variables
* @{
*/

USBD_Usr_cb_TypeDef USR_cb = {
  USBD_USR_Init,
  USBD_USR_DeviceReset,
  USBD_USR_DeviceConfigured,
  USBD_USR_DeviceSuspended,
  USBD_USR_DeviceResumed,


  USBD_USR_DeviceConnected,
  USBD_USR_DeviceDisconnected,
};

/**
* @}
*/

/** @defgroup USBD_USR_Private_Constants
* @{
*/

/**
* @}
*/



/** @defgroup USBD_USR_Private_FunctionPrototypes
* @{
*/
/**
* @}
*/


/** @defgroup USBD_USR_Private_Functions
* @{
*/

/**
* @brief  USBD_USR_Init 
*         Displays the message on LCD for host lib initialization
* @param  None
* @retval None
*/
void USBD_USR_Init(void)
{
#ifdef USE_USB_OTG_HS
    #ifdef USE_EMBEDDED_PHY
    CV_LOG("USB OTG HS_IN_FS VCP Device\r\n");
    #else
    CV_LOG("USB OTG HS VCP Device init\r\n");
    #endif
#else
    CV_LOG("USB OTG FS VCP Device\r\n");
#endif
}

/**
* @brief  USBD_USR_DeviceReset 
*         Displays the message on LCD on device Reset Event
* @param  speed : device speed
* @retval None
*/
void USBD_USR_DeviceReset(uint8_t speed)
{
  switch (speed)
  {
  case USB_OTG_SPEED_HIGH:
    CV_LOG("USB Device Library V1.2.1 [HS]\r\n");
    break;
  case USB_OTG_SPEED_FULL:
    CV_LOG("USB Device Library V1.2.1 [FS]\r\n");
    break;
  default:
    CV_LOG("USB Device Library V1.2.1 [N/A]\r\n");
    break;
  }
}


/**
* @brief  USBD_USR_DeviceConfigured
*         Displays the message on LCD on device configuration Event
* @param  None
* @retval Status
*/
void USBD_USR_DeviceConfigured(void)
{
    CV_LOG("%s%sVCP Interface configured\r\n%s", RTT_CTRL_BG_BRIGHT_GREEN, RTT_CTRL_TEXT_BLACK, RTT_CTRL_RESET);
    usb_ok_flag = 1;
}

/**
* @brief  USBD_USR_DeviceSuspended 
*         Displays the message on LCD on device suspend Event
* @param  None
* @retval None
*/
void USBD_USR_DeviceSuspended(void)
{
  /* Users can do their application actions here for the USB-Reset */
    CV_LOG("USB Device in Suspend Mode.\r\n");
}


/**
* @brief  USBD_USR_DeviceResumed 
*         Displays the message on LCD on device resume Event
* @param  None
* @retval None
*/
void USBD_USR_DeviceResumed(void)
{
  /* Users can do their application actions here for the USB-Reset */
    CV_LOG("USB Device in Idle Mode.\r\n");
}


/**
* @brief  USBD_USR_DeviceConnected
*         Displays the message on LCD on device connection Event
* @param  None
* @retval Status
*/
void USBD_USR_DeviceConnected(void)
{
    CV_LOG("%s%sUSB Device Connected\r\n%s", RTT_CTRL_BG_BRIGHT_GREEN, RTT_CTRL_TEXT_BLACK, RTT_CTRL_RESET);
}


/**
* @brief  USBD_USR_DeviceDisonnected
*         Displays the message on LCD on device disconnection Event
* @param  None
* @retval Status
*/
void USBD_USR_DeviceDisconnected(void)
{
    CV_LOG("USB Device Disconnected.\r\n");
}

/**
* @}
*/

/**
* @}
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
