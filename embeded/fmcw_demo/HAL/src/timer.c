#include "timer.h"
#include "led.h"
#include "gpio.h"
#include "sys.h"

extern volatile led_cfg_t led_cfg;
volatile uint32_t blinking_count_for_findme = 0;

volatile uint32_t delay = 0;

#ifdef LONGQUAN

#include "mcu_api.h"
#include "sys.h"

extern tramp paratramp;

volatile uint32_t reset_network_counter = 0;
volatile uint32_t production_test_counter = 0;

volatile uint32_t freq_1hz_counter = 0;
volatile uint32_t freq_10hz_counter = 0;
volatile uint32_t freq_2hz_counter = 0;

volatile uint32_t adc2_1s_counter = 0;

uint8_t adc1_flash_enable = 0;
uint8_t adc1_flash_slow_enable = 0;
volatile uint32_t adc1_flash_times = 0;

uint8_t adc1_adc2_all_flash_enable = 0;
volatile uint32_t adc1_adc2_all_flash_times = 0;

uint8_t adc2_1s_on_enable = 0;

void Timer02A_CallBack(void)
{
    if (Set == PORT_GetBit(PortB, Pin05)) {
        reset_network_counter = 0;
    } else {
        if (reset_network_counter >= 10*1000) {
            reset_network_counter = 0;
            mcu_network_start();
        } else {
            reset_network_counter++;
        }
    }

    if (Set == PORT_GetBit(PortB, Pin00)) {
        production_test_counter = 0;
    } else {
        if (production_test_counter >= 10*1000) {
            production_test_counter = 0;
            if (0 == paratramp.production_and_test) {
                paratramp.production_and_test = 1;
                GPIO4_HIGH();
                GPIO5_HIGH();
            } else {
                paratramp.production_and_test = 0;
                GPIO4_LOW();
                GPIO5_LOW();
            }
        } else {
            production_test_counter++;
        }
    }

    if (adc1_flash_enable == 1) {
        adc1_flash_slow_enable = 0;
        if (adc1_flash_times >= 10*2*4) {      // 10hz*2s*2?
            adc1_flash_times = 0;
            adc1_flash_enable = 0;
            freq_10hz_counter = 0;
            GPIO4_LOW();
        } else {
            if (freq_10hz_counter >= 100) {    // 10hz
                freq_10hz_counter = 0;
                PORT_Toggle(PortA, Pin00);     // adc1
                adc1_flash_times++;
            } else {
                freq_10hz_counter++;
            }
        }
    } else {
        if (adc1_flash_slow_enable == 1) {
            if (freq_1hz_counter >= 1000) {
                freq_1hz_counter = 0;
                PORT_Toggle(PortA, Pin00);   // adc1
            } else {
                freq_1hz_counter++;
            }
        } else {
            if (adc1_adc2_all_flash_enable == 1) {
                if (adc1_adc2_all_flash_times >= 4*5*2) {     // 2hz*5s*2?
                    adc1_adc2_all_flash_times = 0;
                    adc1_adc2_all_flash_enable = 0;
                    freq_2hz_counter = 0;
                    GPIO4_LOW();
                    GPIO5_LOW();
                } else {
                    if (freq_2hz_counter >= 500) {     // 2hz
                        freq_2hz_counter = 0;

                        if (0 == adc1_adc2_all_flash_times) {
                            GPIO4_LOW();
                            GPIO5_HIGH();
                        } else {
                            PORT_Toggle(PortA, Pin00);    // adc1
                            PORT_Toggle(PortA, Pin04);    // adc2
                        }
                        adc1_adc2_all_flash_times++;
                    } else {
                        freq_2hz_counter++;
                    }
                }
            } else {
                if (adc2_1s_on_enable == 1) {
                    if (adc2_1s_counter >= 2000) {
                        adc2_1s_counter = 0;
                        adc2_1s_on_enable = 0;
                        GPIO5_LOW();
                    } else {
                        adc2_1s_counter++;
                        GPIO5_HIGH();
                    }
                }
            }
        }
    }
}
#endif

/**
 *******************************************************************************
 ** \brief  Timer configuration, for generating event EVT_TMR02_GCMA every second.
 **
 ** \param  None.
 **
 ** \retval None.
 **
 ******************************************************************************/
void init_timer1(void)
{
    //init timer
    TIM_TimeBaseInitType TIM_TimeBaseStructure;
    OCInitType TIM_OCInitStructure;

    /* Time base configuration */
    TIM_TimeBaseStructure.Period    = 1000 - 1;
    TIM_TimeBaseStructure.Prescaler = 72-1; //2MHz base,
    TIM_TimeBaseStructure.ClkDiv    = TIM_CLK_DIV1; //144MHz
    TIM_TimeBaseStructure.CntMode   = TIM_CNT_MODE_UP;

    TIM_InitTimeBase(TIM1, &TIM_TimeBaseStructure);

    //----------------------------------------------------------
    /* Output Compare Active Mode configuration: Channel1 */ //for adc trigger
    TIM_OCInitStructure.OcMode      = TIM_OCMODE_PWM1;
    TIM_OCInitStructure.OutputState = TIM_OUTPUT_STATE_ENABLE;
    TIM_OCInitStructure.Pulse       = 500 - 1;
    TIM_OCInitStructure.OcPolarity  = TIM_OC_POLARITY_LOW;

    TIM_InitOc1(TIM1, &TIM_OCInitStructure);
    TIM_ConfigOc1Preload(TIM1, TIM_OC_PRE_LOAD_DISABLE);

    //////////////////////////////////////////////////////
    TIM_ConfigArPreload(TIM1, ENABLE);
}

void init_timer2(void)
{
    //init timer
    TIM_TimeBaseInitType TIM_TimeBaseStructure;
    OCInitType TIM_OCInitStructure;
    NVIC_InitType NVIC_InitStructure;

    /* Enable the TIM2 global Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel                   = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;

    NVIC_Init(&NVIC_InitStructure);

    /* Time base configuration */
    TIM_TimeBaseStructure.Period    = 1000 - 1;
    TIM_TimeBaseStructure.Prescaler = 7200-1; //
    TIM_TimeBaseStructure.ClkDiv    = TIM_CLK_DIV1; //72MHz
    TIM_TimeBaseStructure.CntMode   = TIM_CNT_MODE_UP;

    TIM_InitTimeBase(TIM2, &TIM_TimeBaseStructure);

    /* Output Compare Active Mode configuration: Channel1 */
    TIM_OCInitStructure.OcMode      = TIM_OCMODE_PWM1;
    TIM_OCInitStructure.OutputState = TIM_OUTPUT_STATE_ENABLE;
    TIM_OCInitStructure.Pulse       = 500 - 1;
    TIM_OCInitStructure.OcPolarity  = TIM_OC_POLARITY_LOW;

    TIM_InitOc1(TIM2, &TIM_OCInitStructure);
    TIM_ConfigOc1Preload(TIM2, TIM_OC_PRE_LOAD_DISABLE);

    //////////////////////////////////////////////////////
    TIM_ConfigArPreload(TIM2, ENABLE);
    TIM_ConfigInt(TIM2, TIM_INT_CC1, ENABLE);
}

void init_timer3(void)
{
    //init timer
    TIM_TimeBaseInitType TIM_TimeBaseStructure;
    OCInitType TIM_OCInitStructure;
    NVIC_InitType NVIC_InitStructure;

    /* Enable the TIM2 global Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel                   = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;

    NVIC_Init(&NVIC_InitStructure);

    /* Time base configuration */
    TIM_TimeBaseStructure.Period    = 1000 - 1;
    TIM_TimeBaseStructure.Prescaler = 7200-1; //
    TIM_TimeBaseStructure.ClkDiv    = TIM_CLK_DIV1; //72MHz
    TIM_TimeBaseStructure.CntMode   = TIM_CNT_MODE_UP;

    TIM_InitTimeBase(TIM3, &TIM_TimeBaseStructure);

    /* Output Compare Active Mode configuration: Channel1 */
    TIM_OCInitStructure.OcMode      = TIM_OCMODE_PWM1;
    TIM_OCInitStructure.OutputState = TIM_OUTPUT_STATE_ENABLE;
    TIM_OCInitStructure.Pulse       = 500 - 1;
    TIM_OCInitStructure.OcPolarity  = TIM_OC_POLARITY_LOW;

    TIM_InitOc1(TIM3, &TIM_OCInitStructure);
    TIM_ConfigOc1Preload(TIM3, TIM_OC_PRE_LOAD_DISABLE);

    //////////////////////////////////////////////////////
    TIM_ConfigArPreload(TIM3, ENABLE);
    TIM_ConfigInt(TIM3, TIM_INT_CC1, ENABLE);
}

void start_timer1(void)
{
    TIM_ClearFlag(TIM1, TIM_FLAG_CC1);
    TIM_Enable(TIM1, ENABLE);
    TIM_EnableCtrlPwmOutputs(TIM1, ENABLE);
}

void start_timer2(void)
{
    TIM_ClearFlag(TIM2, TIM_FLAG_CC1);
    TIM_Enable(TIM2, ENABLE);
}

void start_timer3(void)
{
    TIM_ClearFlag(TIM3, TIM_FLAG_CC1);
    TIM_Enable(TIM3, ENABLE);
}

void TimerConfig(void)
{
    init_timer1();   //for trigger adc1 and adc2
    start_timer1();

#ifdef LONGQUAN
    stc_irq_regi_conf_t stcIrqRegiConf;

    /* Enable channel A interrupt */
    TIMER0_IntCmd(TMR_UNIT, Tim0_ChannelA, Enable);
    /* Register TMR_INI_GCMA Int to Vect.No.001 */
    stcIrqRegiConf.enIRQn = Int002_IRQn;
    /* Select I2C Error or Event interrupt function */
    stcIrqRegiConf.enIntSrc = INT_TMR02_GCMA;
    /* Callback function */
    stcIrqRegiConf.pfnCallback =&Timer02A_CallBack;
    /* Registration IRQ */
    enIrqRegistration(&stcIrqRegiConf);
    /* Clear Pending */
    NVIC_ClearPendingIRQ(stcIrqRegiConf.enIRQn);
    /* Set priority */
    NVIC_SetPriority(stcIrqRegiConf.enIRQn, DDL_IRQ_PRIORITY_15);
    /* Enable NVIC */
    NVIC_EnableIRQ(stcIrqRegiConf.enIRQn);
#else
    init_timer2();
#endif
}

/**
 *******************************************************************************
 ** \brief  Timer configuration for led blinking every 100ms
 **
 ** \param  None.
 **
 ** \retval None.
 **
 ******************************************************************************/
void Timer01_Config(void)
{
    init_timer3();
}

void systick_init(void)
{
    /* setup systick timer for 1000Hz interrupts */
    if (SysTick_Config(SystemCoreClock / 1000U)){
        /* capture error */
        while (1){
        }
    }
    /* configure the systick handler priority */
    NVIC_SetPriority(SysTick_IRQn, 0x00U);
}

uint32_t SysTick_GetTick(void)
{
    return delay;
}
