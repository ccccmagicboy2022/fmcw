#include "radar_hal.h"
#include "sys.h"

uint16_t vt_tab[DAC_ALL_RESOLUTION] = {0};

void radar_init(void)
{
    spi4_init();
    adc_init();
    input_capture_init();
    dac_init();
    //write regs
    spi4_write_reg32(0x000007f8);
    spi4_write_reg32(0x001fc00a);
}

void adc_init(void)
{

}

void input_capture_init(void)
{
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource9, GPIO_AF_TIM1);
    
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannel = TIM1_CC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    TIM_ICInitTypeDef  TIM_ICInitStructure;
    
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV8;
    TIM_ICInitStructure.TIM_ICFilter = 0x0;
    
    TIM_ICInit(TIM1, &TIM_ICInitStructure);
    TIM_Cmd(TIM1, ENABLE);
    TIM_ITConfig(TIM1, TIM_IT_CC1, ENABLE);
}

void dac_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;;

    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure); 
    TIM_TimeBaseStructure.TIM_Period = 0xFF;          
    TIM_TimeBaseStructure.TIM_Prescaler = 0;       
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;    
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);
    
    TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);
    TIM_Cmd(TIM6, ENABLE);
    
    DMA_InitTypeDef DMA_InitStructure;
    
    DMA_DeInit(DMA1_Stream6);
    DMA_InitStructure.DMA_Channel = DMA_Channel_7;  
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)DAC_DHR12R2_ADDRESS;
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)vt_tab;
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
    DMA_InitStructure.DMA_BufferSize = DAC_ALL_RESOLUTION;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(DMA1_Stream6, &DMA_InitStructure);
    
    DMA_Cmd(DMA1_Stream6, ENABLE);
    
    DAC_InitTypeDef  DAC_InitStructure;
    
    DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;
    DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
    DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
    DAC_Init(DAC_Channel_1, &DAC_InitStructure);
    
    DAC_Cmd(DAC_Channel_1, ENABLE);
    DAC_DMACmd(DAC_Channel_1, ENABLE);
    
}

void spi4_init(void)
{
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource2, GPIO_AF_SPI4);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource4, GPIO_AF_SPI4);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource5, GPIO_AF_SPI4);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource6, GPIO_AF_SPI4);

    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_5;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    SPI_InitTypeDef  SPI_InitStructure;

    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Hard;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_Init(SPI4, &SPI_InitStructure);

    SPI_Cmd(SPI4, ENABLE);
}

void spi4_write_byte(uint8_t val)
{
    SPI_I2S_SendData(SPI4, val);
    while (SPI_GetFlagStatus(SPI4, SPI_FLAG_TXE) == RESET)
        ;
}

void spi4_write_reg32(uint32_t word)
{
    uint8_t send_buf[4];
    uint8_t i;

    send_buf[0] = (word >> 24) & 0xff;
    send_buf[1] = (word >> 16) & 0xff;
    send_buf[2] = (word >> 8) & 0xff;
    send_buf[3] = (word) & 0xff;

    for(i=0;i<4;i++)
    {
        spi4_write_byte(send_buf[i]);
    }
}

