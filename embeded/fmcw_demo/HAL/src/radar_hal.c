#include "radar_hal.h"
#include "sys.h"

uint16_t vt_tab[DAC_ALL_RESOLUTION] = {0};
float target_divout2_start_freq_at_hz = 0.0f;
float target_divout2_diff_freq_at_hz = 0.0f;
float target_divout2_freqs_at_hz[DAC_WORK_RESOLUTION + 1];

extern uint16_t uhICReadValue;

float get_cap_divout2_at_hz(void)
{
    return (float)(SystemCoreClock / (uhICReadValue)) * 8.0f;
}

void frequency_calibration(void)
{
    target_divout2_start_freq_at_hz = FREQ_MIN * 1000000.0f / FREQ_OUT_DIV;
    target_divout2_diff_freq_at_hz = ((float)(FREQ_MAX - FREQ_MIN) * 1000000.0f / FREQ_OUT_DIV) / DAC_WORK_RESOLUTION;
        
    CV_LOG("target_divout2_start_freq_at_hz: %f Hz\r\n", target_divout2_start_freq_at_hz);
    CV_LOG("target_divout2_diff_freq_at_hz: %f Hz\r\n", target_divout2_diff_freq_at_hz);
    
    while (1)
    {
        USB_OTG_BSP_mDelay(100);
        CV_LOG("tim1: %f Hz\r\n", get_cap_divout2_at_hz());
    }
}

void radar_spi_init(void)
{
    spi4_cs_init();
    spi4_init();
    //write regs
    spi4_write_reg32(0x000007F8);//reg0
    spi4_write_reg32(0x001FC00A);//reg2
}

void radar_init(void)
{
    radar_spi_init();
    input_capture_init();
    dac_init();
    adc_init();
}

void adc_init(void)
{
    AdcConfig();
    ADC_SoftwareStartConv(ADC3);
    TimerConfig();
}

void input_capture_gpio_config(void)
{
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource9, GPIO_AF_TIM1);
    
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
}

void input_capture_nvic_config(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannel = TIM1_CC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_TIM10_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void input_capture_timer_config(void)
{   
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    
    TIM_TimeBaseStructure.TIM_Period = 0xFFFF;
    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
    
    TIM_ICInitTypeDef  TIM_ICInitStructure;
    
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV8;
    TIM_ICInitStructure.TIM_ICFilter = 0;
    
    TIM_ICInit(TIM1, &TIM_ICInitStructure);
    TIM_ITConfig(TIM1, TIM_IT_CC1, ENABLE);
    TIM_Cmd(TIM1, ENABLE);
}

void input_capture_init(void)
{   
    input_capture_gpio_config();
    input_capture_nvic_config();
    input_capture_timer_config();
}

void dac_gpio_config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;;

    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void dac_first_init(void)
{
    DAC_InitTypeDef  DAC_InitStructure;
    
    DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;
    DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
    DAC_Init(DAC_Channel_1, &DAC_InitStructure);
    
    DAC_Cmd(DAC_Channel_1, ENABLE);
}

void dac_first_deinit(void)
{
    DAC_Cmd(DAC_Channel_1, DISABLE);
}

void dac_secend_init(void)
{
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

void dac_init(void)
{
    dac_gpio_config();
    dac_first_init();
    dac_set_vol(1.0f);
}

void dac_first_set_value(uint32_t data)
{
    DAC_SetChannel1Data(DAC_Align_12b_R, data);
}

void dac_set_vol(float vol)
{
    uint32_t dac_raw = 0;
    
    dac_raw = vol * 4095.0f / 3.3f + 0.5f;
    
    DAC_SetChannel1Data(DAC_Align_12b_R, dac_raw);
}

void spi4_cs_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    
    SPI4_CS_HIGH;
}

void spi4_init(void)
{
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource2, GPIO_AF_SPI4);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource6, GPIO_AF_SPI4);

    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    SPI_InitTypeDef  SPI_InitStructure;

    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_Init(SPI4, &SPI_InitStructure);

    SPI_Cmd(SPI4, ENABLE);
}

void spi4_write_byte(uint8_t val)
{
    while (SPI_GetFlagStatus(SPI4, SPI_FLAG_TXE) == RESET)
        ;
    SPI_I2S_SendData(SPI4, val);
}

void spi4_write_reg32(uint32_t word)
{
    uint8_t send_buf[4];
    uint8_t i;

    send_buf[0] = (word >> 24) & 0xff;
    send_buf[1] = (word >> 16) & 0xff;
    send_buf[2] = (word >> 8) & 0xff;
    send_buf[3] = (word) & 0xff;

    SPI4_CS_LOW;
    for(i=0;i<4;i++)
    {
        spi4_write_byte(send_buf[i]);
    }
    USB_OTG_BSP_uDelay(20);
    SPI4_CS_HIGH;
}

