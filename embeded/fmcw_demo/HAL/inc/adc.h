#ifndef __MYADC_H
#define __MYADC_H

#include "ringbuffer.h"
#include "sys.h"

typedef enum {
    TESK1 = 1,
    TESK2,
} TESK_ID_TYPEDEF;

typedef enum {
    Task_STATE1  = 1,
    Task_STATE2,
    Task_STATE3,
    Task_STATE4,
} Task_TYPEDEF;

#define LIGHT_SAMPLE_NUM            (2u)
#define LIGHT_BLK_COUNT             (6000u)

typedef void (*TaskCallBack)(TESK_ID_TYPEDEF Task_ID, Task_TYPEDEF Task_State);

extern void AdcConfig(void);
void TaskRegister(TaskCallBack pCBS);

int get_sample_data(u8 *buf);

void DmaInitConfig(void);
void AdcConfig(void);
void AdcInitConfig(void);
void DmaConfig(void);
int is_data_available(void);
void init_pins(void);
void TimerConfig(void);
void start_adc_timer(void);
void stop_adc_timer(void);

#endif
