#ifndef _9433C34C_8E28_4380_8834_41AC2D04A886_
#define _9433C34C_8E28_4380_8834_41AC2D04A886_

#include "sys.h"

#define LED1_OFF GPIO_SetBits(GPIOE, GPIO_Pin_12)
#define LED1_ON GPIO_ResetBits(GPIOE, GPIO_Pin_12)
#define LED1_TOGGLE GPIO_ToggleBits(GPIOE, GPIO_Pin_12)
#define LED2_OFF GPIO_SetBits(GPIOE, GPIO_Pin_13)
#define LED2_ON GPIO_ResetBits(GPIOE, GPIO_Pin_13)
#define LED3_OFF GPIO_SetBits(GPIOE, GPIO_Pin_14)
#define LED3_ON GPIO_ResetBits(GPIOE, GPIO_Pin_14)

void mcu429_init(void);
void led_init(void);


#endif//_9433C34C_8E28_4380_8834_41AC2D04A886_
