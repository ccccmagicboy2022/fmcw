#include "wdt.h"
#include "sys.h"

void wdt_config(void)
{
    IWDG_WriteConfig(IWDG_WRITE_ENABLE);
    IWDG_SetPrescalerDiv(IWDG_PRESCALER_DIV32); //0.8ms
    IWDG_CntReload(2500);       //2s
    IWDG_ReloadKey();
    IWDG_Enable();
}
