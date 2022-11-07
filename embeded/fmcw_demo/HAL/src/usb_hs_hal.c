#include "usb_hs_hal.h"
#include "sys.h"

__ALIGN_BEGIN USB_OTG_CORE_HANDLE USB_OTG_dev __ALIGN_END;

void usb_hs_init(void)
{
    USBD_Init(
    &USB_OTG_dev,
    USB_OTG_HS_CORE_ID,
    &USR_desc, 
    &USBD_CDC_cb, 
    &USR_cb);
}
