#include "hc32f46x_exint_nmi_swi.h"
#include "hc32_ddl.h"
#include "sys.h"
#include "common.h"

void SWI_Init(const stc_swi_config_t *pstcSwiConfig)
{
    uint8_t u8SWI_IRQ = 0u, i;
    stc_irq_regi_conf_t stcIrqRegiConf;

    /* configure structure initialization */
    MEM_ZERO_STRUCT(stcIrqRegiConf);

    for (i = 0u; i< 32u; i++) {
        if ((uint32_t)(1ul << i) == (pstcSwiConfig->enSwiCh)) {
            u8SWI_IRQ = i;
            break;
        }
    }

    /* Select software Int Ch.8 */
    stcIrqRegiConf.enIntSrc = (en_int_src_t)(u8SWI_IRQ);

    /* Register software Int to Vect.No.008 */
    stcIrqRegiConf.enIRQn = (IRQn_Type)(u8SWI_IRQ);

    /* Callback function */
    stcIrqRegiConf.pfnCallback = pstcSwiConfig->pfnSwiCallback;

    enIrqRegistration(&stcIrqRegiConf);

    /* Clear Pending */
    NVIC_ClearPendingIRQ(stcIrqRegiConf.enIRQn);

    /* Set priority */
    NVIC_SetPriority(stcIrqRegiConf.enIRQn, DDL_IRQ_PRIORITY_15);

    /* Enable NVIC */
    NVIC_EnableIRQ(stcIrqRegiConf.enIRQn);
}

void trigger_preprocess(void)
{
    SWI_Enable(SwiCh29);
}

void disable_preprocess(void)
{
    enIntEnable(SwiCh29);
}

void SWI29_Callback(void)
{
    SWI_Disable(SwiCh29);
    while (1)
        PRINT_LOG(LOG_LEVEL_DEBUG, "test");
}

void init_swi(void)
{
    stc_swi_config_t stcSwiConfig;

    stcSwiConfig.enSwiCh = SwiCh29;

    /* Software interrupt */
    stcSwiConfig.enSwiType = SwInt;

    /* Software interrupt callback function */
    stcSwiConfig.pfnSwiCallback = &SWI29_Callback;

    SWI_Init(&stcSwiConfig);
}
