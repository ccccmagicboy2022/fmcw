#ifndef __WDT_H__
#define __WDT_H__

#include "sys.h"

void wdt_config(void);

//should be every 0.256s feed
static inline void wdt_feed_dog(void)
{
    IWDG_ReloadKey();
}
#endif
