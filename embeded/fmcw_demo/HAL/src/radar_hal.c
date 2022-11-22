#include "radar_hal.h"
#include "sys.h"

uint16_t vt_tab[DAC_ALL_RESOLUTION];
float cap_freq_mean_a[FREQ_CAP_MEAN_NUM];
extern uint16_t uhICReadValue;
PidCtrlTypedef pidCtrl;

///////////
uint16_t cycle_freq_diff[DAC_WORK_RESOLUTION + 1] = {0};
uint32_t cycle_all, cycle_all_ready = 0;
uint16_t vtune;
#define CHECK_TIME 100
///////////

float get_cap_divout2_at_khz(void)
{
    return (float)(SystemCoreClock / (uhICReadValue)) * 8.0f / 1000.0f;
}

float get_cap_rf_freq_at_khz(void)
{
    return get_cap_divout2_at_khz()*(float)(FREQ_OUT_DIV);
}

void pidInit()
{
    pidCtrl.S.Kp = KP;
    pidCtrl.S.Ki = KI;
    pidCtrl.S.Kd = KD;
    arm_pid_init_f32(&pidCtrl.S, 1);

    pidCtrl.out = 0.0f;
}

uint8_t cal_once(float target_khz, uint16_t * dac_raw)
{
    float cap_freq_mean = 0.0f;
    int16_t ii = 0;
    uint8_t res = 0;

    while (1)
    {
        cap_freq_mean_a[((ii++)%FREQ_CAP_MEAN_NUM)] = get_cap_divout2_at_khz();
        arm_mean_f32(cap_freq_mean_a, FREQ_CAP_MEAN_NUM, &cap_freq_mean);
        if (ii%FREQ_CAP_MEAN_NUM == 0)
        {
            //printf("{cap}%.3lf\n{dac}%d\n", cap_freq_mean, dac_get_value());
            res = pidExecu(target_khz, cap_freq_mean);
            if (res)
            {
                break;
            }
        }
    }

    *dac_raw = dac_get_value();

    return 1;
}

uint8_t pidExecu(float target_khz, float cap_khz)
{
    float pidErr;
    static uint16_t ok_timer = 0;

    pidErr = target_khz - cap_khz;

    if(fabs(pidErr) > ERR_LIMIT)
    {
        pidCtrl.out = arm_pid_f32(&pidCtrl.S, pidErr);
        dac_set_vol(pidCtrl.out);
        USB_OTG_BSP_uDelay(2);
        ok_timer = 0;
    }
    else
    {
        ok_timer++;
    }

    //printf("{ok_timer}%d\n", ok_timer);
    if (ok_timer > OK_TIMER_NUM)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void frequency_calibration(void)
{
    float target_divout2_start_freq_at_khz = 0.0f;
    float target_divout2_diff_freq_at_khz = 0.0f;
    uint16_t dac_raw = 0;

    target_divout2_start_freq_at_khz = FREQ_MIN * 1000.0f / FREQ_OUT_DIV;
    target_divout2_diff_freq_at_khz = ((float)(FREQ_MAX - FREQ_MIN) * 1000.0f / FREQ_OUT_DIV) / DAC_WORK_RESOLUTION;

    printf("target_divout2_start_freq_at_hz: %.3f KHz\r\n", target_divout2_start_freq_at_khz);
    printf("target_divout2_diff_freq_at_hz: %.6f KHz\r\n", target_divout2_diff_freq_at_khz);

    pidInit();

    //while(1)
    {
        for (int i = 0; i <= DAC_WORK_RESOLUTION; i++)
        {
            cal_once(target_divout2_start_freq_at_khz + i * target_divout2_diff_freq_at_khz, &dac_raw);
            vt_tab[i] = dac_raw;
            printf("{i}%d\n{dac_set}%d\n", i, dac_raw);
        }
    }

    for (int i = DAC_WORK_RESOLUTION + 1; i < DAC_WORK_RESOLUTION + DAC_IDEL_RESOLUTION / 2; i++)
    {
        vt_tab[i] = vt_tab[DAC_WORK_RESOLUTION];
    }

    for (int i = DAC_WORK_RESOLUTION + DAC_IDEL_RESOLUTION / 2; i < DAC_ALL_RESOLUTION; i++)
    {
        vt_tab[i] = vt_tab[0];
    }

    //check vt_tab
    for (int i = 0;i < DAC_ALL_RESOLUTION;i++)
    {
        printf("{vt_tab}%d\n", vt_tab[i]);
    }

    //while(1)
//    {
//        for (int i = 0; i <= DAC_WORK_RESOLUTION; i++)
//        {
//            check_once(target_divout2_start_freq_at_khz + i * target_divout2_diff_freq_at_khz, vt_tab[i], i);
//            USB_OTG_BSP_mDelay(1);
//        }
//    }

    input_capture_disable();
    dac_first_deinit();
    printf("frequency celibration finish\n");
    CV_LOG("frequency celibration finish\n");
}

uint8_t check_once(float target_khz, uint16_t dac_raw_in, uint8_t index)
{
    int16_t ii = 0;
    float cap_freq_mean = 0.0f;

    float rf_start_freq_at_khz = 0.0f;
    float rf_diff_freq_at_khz = 0.0f;

    rf_start_freq_at_khz = FREQ_MIN * 1000.0f;
    rf_diff_freq_at_khz = ((float)(FREQ_MAX - FREQ_MIN) * 1000.0f) / DAC_WORK_RESOLUTION;

    dac_set_value(dac_raw_in);

    while (1)
    {
        cap_freq_mean_a[((ii++)%FREQ_CHECK_MEAN_NUM)] = get_cap_rf_freq_at_khz();
        arm_mean_f32(cap_freq_mean_a, FREQ_CHECK_MEAN_NUM, &cap_freq_mean);
        if (ii%FREQ_CHECK_MEAN_NUM == 0)
        {
            printf("{check_freq_diff}%.1lf, %.1lf\n", cap_freq_mean, rf_start_freq_at_khz + index * rf_diff_freq_at_khz);
            break;
        }
        USB_OTG_BSP_uDelay(2);
    }

    return 1;
}

void input_capture_disable(void)
{
    TIM_ITConfig(TIM1, TIM_IT_CC1, DISABLE);
    TIM_Cmd(TIM1, DISABLE);
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
    LED2_ON;
    CV_LOG("frequency celibration start\n");
#ifdef FAST_CALC_FREQ
    frequency_calibration2();
#else
    frequency_calibration();
#endif
    LED3_ON;
    dac_secend_init();
    adc_init();
    frame_timer_init();
}

void frequency_calibration2(void)
{
    uint32_t freq_calibration_state = 0;
    float frequency = 0;
    uint32_t printf_error_indexa, checkout_time, cycle_all_storage = 0;
    
    for (int i = 0; i <= DAC_WORK_RESOLUTION; i++) {
        cycle_freq_diff[i] = (uint16_t)((float)SystemCoreClock / 1000000.0f * FREQ_OUT_DIV * 8 \
            / (FREQ_MIN + ((float)(FREQ_MAX - FREQ_MIN) / DAC_WORK_RESOLUTION) * i));
        printf("{cycle_freq_diff}%d\n", cycle_freq_diff[i]);
    }
    
    while (1) {
        if (freq_calibration_state > DAC_WORK_RESOLUTION) {
            break;
        } else {
            if (cycle_all_ready) {
                cycle_all_storage = cycle_all;
                frequency = SystemCoreClock / 1000000.0f / (float)(cycle_all_storage) * 8;
//                printf("period: %d, frequency: %f, v: %d\n", \
//                        cycle_all_storage, frequency, vtune);
                if (cycle_all_storage == cycle_freq_diff[freq_calibration_state]) {
                    checkout_time++;
                    if (checkout_time > CHECK_TIME) {
                        checkout_time = 0;
                        vt_tab[freq_calibration_state] = vtune - 112;
                        printf("progress: no.%d\nperiod: %d, frequency: %f, v: %d\n\n", \
                            freq_calibration_state, cycle_all_storage, frequency, vtune);
                        freq_calibration_state += 1;
                    }
                } else {
                    if ((cycle_all_storage > cycle_freq_diff[freq_calibration_state] - 2) && \
                        (cycle_all_storage < cycle_freq_diff[freq_calibration_state] + 2)) {
                        checkout_time--;
                    } else {
                        checkout_time = 0;
                    }
                    if ((cycle_all_storage < cycle_freq_diff[freq_calibration_state]) \
                            && (vtune < 3000)) {
                        vtune++;
                    } else if ((cycle_all_storage > cycle_freq_diff[freq_calibration_state]) \
                            && (vtune > 0)) {
                        vtune--;
                    } else if (vtune == 3000 || vtune == 0) {
                        printf_error_indexa++;
                        if (printf_error_indexa > 100) {
                            printf_error_indexa = 0;
                            printf("RF device error");
                            while (1) {}
                        }
                    }
                    dac_set_value(vtune);
                    USB_OTG_BSP_uDelay(2);
                }
                cycle_all_ready = 0;
            }
        }
    }
/////////////////////////////////////////////////////////////////////////
    float target_divout2_start_freq_at_khz = 0.0f;
    float target_divout2_diff_freq_at_khz = 0.0f;
    uint16_t dac_raw = 0;

    target_divout2_start_freq_at_khz = FREQ_MIN * 1000.0f / FREQ_OUT_DIV;
    target_divout2_diff_freq_at_khz = ((float)(FREQ_MAX - FREQ_MIN) * 1000.0f / FREQ_OUT_DIV) / DAC_WORK_RESOLUTION;

    printf("target_divout2_start_freq_at_hz: %.3f KHz\r\n", target_divout2_start_freq_at_khz);
    printf("target_divout2_diff_freq_at_hz: %.6f KHz\r\n", target_divout2_diff_freq_at_khz);

    for (int i = DAC_WORK_RESOLUTION + 1; i < DAC_WORK_RESOLUTION + DAC_IDEL_RESOLUTION / 2; i++)
    {
        vt_tab[i] = vt_tab[DAC_WORK_RESOLUTION];
    }

    for (int i = DAC_WORK_RESOLUTION + DAC_IDEL_RESOLUTION / 2; i < DAC_ALL_RESOLUTION; i++)
    {
        vt_tab[i] = vt_tab[0];
    }

    //check vt_tab
    for (int i = 0;i < DAC_ALL_RESOLUTION;i++)
    {
        printf("{vt_tab}%d\n", vt_tab[i]);
    }

//    while(1)
//    {
//        for (int i = 0; i <= DAC_WORK_RESOLUTION; i++)
//        {
//            check_once(target_divout2_start_freq_at_khz + i * target_divout2_diff_freq_at_khz, vt_tab[i], i);
//            USB_OTG_BSP_mDelay(1);
//        }
//    }

    input_capture_disable();
    dac_first_deinit();
    printf("frequency celibration2 finish\n");
    CV_LOG("frequency celibration2 finish\n");
}

void frame_timer_init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Period = FRAME_TIME_D - 1;
    TIM_TimeBaseStructure.TIM_Prescaler = SystemCoreClock / 2 / 1000000 - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);

    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
    TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM5, ENABLE);
}

void adc_init(void)
{
    AdcConfig();
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
    DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
    DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0;
    DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
    DAC_Init(DAC_Channel_1, &DAC_InitStructure);
    DAC_Cmd(DAC_Channel_1, ENABLE);
}

void dac_first_deinit(void)
{
    DAC_Cmd(DAC_Channel_1, DISABLE);
}

void dac_timer_config(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Period = CHIRP_WORK_TIME * (SystemCoreClock / 1000000) / DAC_WORK_RESOLUTION;
    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);

    TIM_SelectOutputTrigger(TIM8, TIM_TRGOSource_Update);

    TIM_Cmd(TIM8, DISABLE);
}

void dac_dma_config(void)
{
    DMA_InitTypeDef DMA_InitStructure;

    DMA_DeInit(DMA1_Stream5);
    DMA_InitStructure.DMA_Channel = DMA_Channel_7;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(DAC->DHR12R1);
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&vt_tab[0];
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
    DMA_Init(DMA1_Stream5, &DMA_InitStructure);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    DMA_ClearITPendingBit(DMA1_Stream5, DMA_IT_TCIF5 | DMA_IT_HTIF5);
    DMA_ITConfig(DMA1_Stream5, DMA_IT_TC, ENABLE);

    DMA_Cmd(DMA1_Stream5, ENABLE);
}

void dac_secend_init(void)
{
    dac_timer_config();

    DAC_InitTypeDef  DAC_InitStructure;

    DAC_InitStructure.DAC_Trigger = DAC_Trigger_T8_TRGO;
    DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
    DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0;
    DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
    DAC_Init(DAC_Channel_1, &DAC_InitStructure);
    DAC_Cmd(DAC_Channel_1, ENABLE);

    dac_dma_config();

    DAC_DMACmd(DAC_Channel_1, ENABLE);
}

void dac_init(void)
{
    dac_gpio_config();
    dac_first_init();
    dac_set_vol(1.5f);
}

void dac_set_value(uint16_t data)
{
    DAC_SetChannel1Data(DAC_Align_12b_R, data);
}

uint16_t dac_get_value(void)
{
    return DAC_GetDataOutputValue(DAC_Channel_1);
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

void start_dac_timer(void)
{
    TIM_SetCounter(TIM8, 0);
    TIM_Cmd(TIM8, ENABLE);
}

void stop_dac_timer(void)
{
    TIM_Cmd(TIM8, DISABLE);
    TIM_SetCounter(TIM8, 0);
}

