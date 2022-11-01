#include "adc.h"
#include "ringbuffer.h"
#include "sys.h"

#ifdef SEND_TO_MATLAB_TEST
#include "test_usart.h"
#endif

u16 buffer[ELEMENT_SIZE / 2 * ELEMENT_COUNT];

ring_buf_t ring_buffer = {
    .rd = 0,
    .wr = 0,
    .max_count = ELEMENT_COUNT,
    .elem_size = ELEMENT_SIZE,
    .buf = (u8 *)buffer,
};

u16 light_buffer[LIGHT_SAMPLE_NUM];

void init_pin(GPIO_Module* port, unsigned int pin)
{
    GPIO_InitType GPIO_InitStructure;

    GPIO_InitStructure.Pin       = pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitPeripheral(port, &GPIO_InitStructure);
}

void AdcConfig(void)
{
    init_pin(GPIOC, GPIO_PIN_4);    //if
    init_pin(GPIOA, GPIO_PIN_6);    //light res
    AdcInitConfig();
    DmaConfig();
}

void AdcInitConfig(void)
{
    ADC_InitType ADC_InitStructure;
    ADC_InitTypeEx ADC_InitStructureEx;

    ADC_InitStructure.WorkMode       = ADC_WORKMODE_INDEPENDENT;
    ADC_InitStructure.MultiChEn      = DISABLE;
    ADC_InitStructure.ContinueConvEn = DISABLE;
    ADC_InitStructure.ExtTrigSelect  = ADC_EXT_TRIGCONV_T1_CC1;

    ADC_InitStructure.DatAlign       = ADC_DAT_ALIGN_R;
    ADC_InitStructure.ChsNumber      = 1;

    ADC_Init(ADC1, &ADC_InitStructure);
    ADC_InitStructureEx.ResBit = ADC_CTRL3_RES_12BIT;
    ADC_InitEx(ADC1, &ADC_InitStructureEx);

    ADC_ConfigRegularChannel(ADC1, ADC1_Channel_03_PA6, 1, ADC_SAMP_TIME_28CYCLES5);

    ADC_EnableExternalTrigConv(ADC1, ENABLE);
    ADC_EnableDMA(ADC1, ENABLE);
    ADC_Enable(ADC1, ENABLE);

    while(ADC_GetFlagStatusNew(ADC1,ADC_FLAG_RDY) == RESET)
        ;
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1))
        ;

    ADC_EnableSoftwareStartConv(ADC1, ENABLE);

////////////////////////////////////////////////////////////
    ADC_InitStructure.WorkMode       = ADC_WORKMODE_INDEPENDENT;
    ADC_InitStructure.MultiChEn      = DISABLE;
    ADC_InitStructure.ContinueConvEn = DISABLE;
    ADC_InitStructure.ExtTrigSelect  = ADC_EXT_TRIGCONV_T1_CC1;

    ADC_InitStructure.DatAlign       = ADC_DAT_ALIGN_R;
    ADC_InitStructure.ChsNumber      = 1;

    ADC_Init(ADC2, &ADC_InitStructure);
    ADC_InitStructureEx.ResBit = ADC_CTRL3_RES_12BIT;
    ADC_InitEx(ADC2, &ADC_InitStructureEx);

    ADC_ConfigRegularChannel(ADC2, ADC2_Channel_05_PC4, 1, ADC_SAMP_TIME_28CYCLES5);    //p195 28.5+1.5=30T=30*1/9=3.3us

    ADC_EnableExternalTrigConv(ADC2, ENABLE);
    ADC_EnableDMA(ADC2, ENABLE);    //use dma1 ch8
    ADC_Enable(ADC2, ENABLE);

    while(ADC_GetFlagStatusNew(ADC2,ADC_FLAG_RDY) == RESET)
        ;
    ADC_StartCalibration(ADC2);
    while (ADC_GetCalibrationStatus(ADC2))
        ;

    ADC_EnableSoftwareStartConv(ADC2, ENABLE);
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
    DMA_InitType DMA_InitStructure;
    NVIC_InitType NVIC_InitStructure;

    DMA_DeInit(DMA1_CH1);
    DMA_InitStructure.PeriphAddr     = (uint32_t)&ADC1->DAT;
    DMA_InitStructure.MemAddr        = (uint32_t)(&light_buffer[0]);
    DMA_InitStructure.Direction      = DMA_DIR_PERIPH_SRC;
    DMA_InitStructure.BufSize        = LIGHT_BLK_COUNT;
    DMA_InitStructure.PeriphInc      = DMA_PERIPH_INC_DISABLE;
    DMA_InitStructure.DMA_MemoryInc  = DMA_MEM_INC_DISABLE;
    DMA_InitStructure.PeriphDataSize = DMA_PERIPH_DATA_SIZE_HALFWORD;
    DMA_InitStructure.MemDataSize    = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.CircularMode   = DMA_MODE_CIRCULAR;
    DMA_InitStructure.Priority       = DMA_PRIORITY_LOW;
    DMA_InitStructure.Mem2Mem        = DMA_M2M_DISABLE;

    /* Enable the DMA Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel                   = DMA1_Channel1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    DMA_Init(DMA1_CH1, &DMA_InitStructure);
    DMA_ConfigInt(DMA1_CH1, DMA_INT_TXC, ENABLE);   //p178
    DMA_EnableChannel(DMA1_CH1, ENABLE);    //for adc1

    ///////////////////////////////////////////////////////

    DMA_DeInit(DMA1_CH8);
    DMA_InitStructure.PeriphAddr     = (uint32_t)&ADC2->DAT;
    DMA_InitStructure.MemAddr        = (uint32_t)(&buffer[0]);
    DMA_InitStructure.Direction      = DMA_DIR_PERIPH_SRC;
    DMA_InitStructure.BufSize        = ELEMENT_SIZE / sizeof(u16);
    DMA_InitStructure.PeriphInc      = DMA_PERIPH_INC_DISABLE;
    DMA_InitStructure.DMA_MemoryInc  = DMA_MEM_INC_ENABLE;
    DMA_InitStructure.PeriphDataSize = DMA_PERIPH_DATA_SIZE_HALFWORD;
    DMA_InitStructure.MemDataSize    = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.CircularMode   = DMA_MODE_CIRCULAR;
    DMA_InitStructure.Priority       = DMA_PRIORITY_VERY_HIGH;
    DMA_InitStructure.Mem2Mem        = DMA_M2M_DISABLE;

    /* Enable the DMA Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel                   = DMA1_Channel8_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    DMA_Init(DMA1_CH8, &DMA_InitStructure);
    DMA_ConfigInt(DMA1_CH8, DMA_INT_TXC, ENABLE);   //p178
    DMA_EnableChannel(DMA1_CH8, ENABLE);            //for adc2
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
