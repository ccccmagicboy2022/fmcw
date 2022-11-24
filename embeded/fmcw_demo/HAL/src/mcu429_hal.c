#include "mcu429_hal.h"
#include "sys.h"

void mcu429_init(void)
{
    SEGGER_RTT_Init();
    CV_LOG("%sMODULE: STM32F429 RADAR DEMO%s\r\n", RTT_CTRL_BG_BRIGHT_RED, RTT_CTRL_RESET);
    CV_LOG("compiled time: %s %s\r\n", __DATE__, __TIME__);

    SysClkInit();
    clk_test();

    led_init();
}

void led_init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

    GPIO_Init(GPIOE, &GPIO_InitStructure);

    LED1_OFF;
    LED2_OFF;
    LED3_OFF;
    CV_LOG("LED1~3: ok!\r\n");
}


