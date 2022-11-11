#include "usb_hs_hal.h"
#include "sys.h"

__ALIGN_BEGIN USB_OTG_CORE_HANDLE USB_OTG_dev __ALIGN_END;

void usb_hs_init(void)
{
    usb_hs_phy_gpio_init();
    
    USBD_Init(
    &USB_OTG_dev,
    USB_OTG_HS_CORE_ID,
    &USR_desc, 
    &USBD_CDC_cb, 
    &USR_cb);
}

void usb_hs_phy_gpio_init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
}
