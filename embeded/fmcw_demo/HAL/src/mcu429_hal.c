#include "mcu429_hal.h"
#include "sys.h"

void mcu429_init(void)
{
    //
    SEGGER_RTT_Init();
    CV_LOG("%sMODULE: STM32F429 RADAR DEMO%s\r\n", RTT_CTRL_BG_BRIGHT_RED, RTT_CTRL_RESET);
    CV_LOG("compiled time: %s %s\r\n", __DATE__, __TIME__);
    
    SysClkInit();
    clk_test();
}
