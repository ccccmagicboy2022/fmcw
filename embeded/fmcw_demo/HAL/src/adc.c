#include "adc.h"
#include "ringbuffer.h"
#include "sys.h"

#ifdef SEND_TO_MATLAB_TEST
#include "test_usart.h"
#endif

u16 buffer[ELEMENT_SIZE / 2 * ELEMENT_COUNT] __attribute__ ((aligned(4)));

ring_buf_t ring_buffer = {
    .rd = 0,
    .wr = 0,
    .max_count = ELEMENT_COUNT,
    .elem_size = ELEMENT_SIZE,
    .buf = (u8 *)buffer,
};

u16 light_buffer[LIGHT_SAMPLE_NUM] __attribute__ ((aligned(4)));

void init_pins(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void TimerConfig(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure); 
    TIM_TimeBaseStructure.TIM_Period = CHIRP_WORK_TIME * (SystemCoreClock / 2 / 1000000) / DAC_WORK_RESOLUTION;
    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
    
    TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update);
    TIM_Cmd(TIM3, DISABLE);
}

void AdcConfig(void)
{
    init_pins();
    AdcInitConfig();
    DmaConfig();
}

void AdcInitConfig(void)
{
    ADC_CommonInitTypeDef ADC_CommonInitStructure;
    
    ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
    ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
    ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
    ADC_CommonInit(&ADC_CommonInitStructure);
    
    ADC_InitTypeDef ADC_InitStructure;    
    
    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T3_TRGO;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfConversion = 1;
    ADC_Init(ADC1, &ADC_InitStructure);
    
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_28Cycles);
    
    ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);
    ADC_DMACmd(ADC1, ENABLE);
    ADC_Cmd(ADC1, ENABLE);
}

/**
 *******************************************************************************
 ** \brief  DMA configuration for ADC1 and ADC2, including initial configuration
 **         and interrupt configuration.
 **
 ******************************************************************************/
void DmaConfig(void)
{
    DmaInitConfig();    //use int
}

/**
 *******************************************************************************
 ** \brief  DMA initial configuration.
 **
 ******************************************************************************/

void DmaInitConfig(void)
{
    DMA_InitTypeDef       DMA_InitStructure;
    
    DMA_DeInit(DMA2_Stream0);
    DMA_InitStructure.DMA_Channel = DMA_Channel_0;  
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(ADC1->DR);
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)(&buffer[0]);
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
    DMA_InitStructure.DMA_BufferSize = SAMPLE_NUM_PER_CHIRP;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(DMA2_Stream0, &DMA_InitStructure);
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TCIF0 | DMA_IT_HTIF0);
    //DMA_ITConfig(DMA2_Stream0, DMA_IT_TC | DMA_IT_HT, ENABLE);
    DMA_ITConfig(DMA2_Stream0, DMA_IT_TC, ENABLE);
    
    DMA_Cmd(DMA2_Stream0, ENABLE);
}

void start_adc_timer(void)
{
    TIM_SetCounter(TIM3, 0);
    TIM_Cmd(TIM3, ENABLE);
}

void stop_adc_timer(void)
{
    TIM_Cmd(TIM3, DISABLE);
    TIM_SetCounter(TIM8, 0);
}

int get_sample_data(u8 *buf)
{
    int ret;
#ifdef SEND_TO_MATLAB_TEST
    usart_polling_send_data((uint8_t *)ring_buffer.buf + ring_buffer.rd * ring_buffer.elem_size, ELEMENT_SIZE);
#endif
    ret = ring_buffer_get(&ring_buffer, buf);

    return ret;
}

TESK_ID_TYPEDEF TaskVal = TESK1;
Task_TYPEDEF TaskState = Task_STATE1;
TaskCallBack pTaskCBS;

void TaskRegister(TaskCallBack pCBS)
{
    if (pTaskCBS == 0) {
        pTaskCBS = pCBS;
    }
}

extern ring_buf_t ring_buffer;

int is_data_available(void)
{
    return !ring_buf_empty(&ring_buffer);
}
