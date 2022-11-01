#include "usart.h"
#include "sys.h"

#ifdef O1_DATA_STREAM
extern uint8_t sampling_en;

void en_O1_data_stream(uint8_t en)
{
    if (DISABLE != en)
    {
        sampling_en = 1;
        sample_usart_init();
    }
    else
    {
        sampling_en = 0;
        gpio_output_init();
    }
}
#endif

void usart_init(void)
{
    GPIO_InitType GPIO_InitStructure;

    GPIO_InitStructure.Pin        = GPIO_PIN_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitPeripheral(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.Pin       = GPIO_PIN_11;
    GPIO_InitStructure.GPIO_Speed = GPIO_INPUT;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitPeripheral(GPIOB, &GPIO_InitStructure);

    NVIC_InitType NVIC_InitStructure;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

    NVIC_InitStructure.NVIC_IRQChannel            = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd         = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    USART_InitType USART_InitStructure;

    USART_InitStructure.BaudRate            = 115200;
    USART_InitStructure.WordLength          = USART_WL_8B;
    USART_InitStructure.StopBits            = USART_STPB_1;
    USART_InitStructure.Parity              = USART_PE_NO;
    USART_InitStructure.HardwareFlowControl = USART_HFCTRL_NONE;
    USART_InitStructure.Mode                = USART_MODE_RX | USART_MODE_TX;

    USART_Init(USART3, &USART_InitStructure);
    USART_ConfigInt(USART3, USART_INT_RXDNE, ENABLE);
    USART_Enable(USART3, ENABLE);
}

int fputc(int ch, FILE* f)
{
    USART_SendData(USART3, (uint8_t)ch);
    while (USART_GetFlagStatus(USART3, USART_FLAG_TXDE) == RESET)
        ;

    return (ch);
}



#ifdef O1_DATA_STREAM
void sample_usart_init(void)
{
    GPIO_InitType GPIO_InitStructure;

    GPIO_InitStructure.Pin        = GPIO_PIN_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);

    USART_InitType USART_InitStructure;

    //USART_InitStructure.BaudRate            = 115200;
    USART_InitStructure.BaudRate            = 500000;
    //USART_InitStructure.BaudRate            = 512000;
    USART_InitStructure.WordLength          = USART_WL_8B;
    USART_InitStructure.StopBits            = USART_STPB_1;
    USART_InitStructure.Parity              = USART_PE_NO;
    USART_InitStructure.HardwareFlowControl = USART_HFCTRL_NONE;
    USART_InitStructure.Mode                = USART_MODE_TX;

    USART_Init(USART1, &USART_InitStructure);
    USART_Enable(USART1, ENABLE);
}

void usart_polling_send_data(uint8_t *data, uint32_t lenth)
{
    uart_transmit_output2(0xABu);
    uart_transmit_output2(0xCDu);
    for (int i = 0; i < lenth; i++) {
        uart_transmit_output2(data[i]);
    }
}

void usart_polling_send_data_no_head(uint8_t *data, uint32_t lenth)
{
    for (int i = 0; i < lenth; i++) {
        uart_transmit_output2(data[i]);
    }
}

#endif
