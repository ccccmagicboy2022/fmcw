#include "timer.h"
#include "sys.h"

extern volatile uint32_t counter4;

void set_c4_start(void)
{
    counter4 = 0;
    TIM_SetCounter(TIM4, 0);
}

uint32_t get_c4_ms(void)
{
    return (uint32_t)(0.7282f * (counter4*65536 + TIM_GetCounter(TIM4)) + 0.5f);
}

void init_timer4(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Period = 0xFFFF;
    TIM_TimeBaseStructure.TIM_Prescaler = 0xFFFF;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM4, DISABLE);
}

void start_timer4(void)
{
    TIM_Cmd(TIM4, ENABLE);
}

void Timer4Config(void)
{
    init_timer4();
    start_timer4();
}


