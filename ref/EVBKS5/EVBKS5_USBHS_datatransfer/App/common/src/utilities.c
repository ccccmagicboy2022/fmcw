/**
  ******************************************************************************
  * @file    utilities.c
  * @author  ting.gao@iclegend.com
  * @brief   project utilities, tools
  ******************************************************************************
  */
#include <stdio.h>
#include "utilities.h"
#include "platform.h"
#include "config.h"
#if defined(STM32H743xx)
#include "rtos.h"
#include "usb_transfer.h"
#endif
void RunFailed(uint8_t *file, uint32_t line)
{ 
    printf("Run failed: file %s line %d \r\n", file, line);

    for (uint16_t i = LED0; i < LED_MAX; i++)
    {
        LED_On((led_typedef_enum)i);
    }
    
    Config_EarseFlashData();
#if defined(STM32H743xx)	
	char errlog[256]={0};
	USB_DATA_T usbData = {0};
	sprintf(errlog, "Run failed: file %s line %d \r\n", file, line);
#endif		
    while (1)
	{
#if defined(STM32H743xx)
		usbData.len = sizeof(errlog);
        usbData.buf = (uint8_t*)errlog;
        UsbTransfer_Send2UsbDataQueue(&usbData);
		HAL_Delay(1000);
#endif
	}
}


