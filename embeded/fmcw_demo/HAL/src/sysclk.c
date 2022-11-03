#include "sys.h"
#include "sysclk.h"

void SysClkInit(void)
{
    //uart
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    //radar
    //spi4
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI4, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
    //input capture
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);
    //dac
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
    //adc
    

}


void clk_test(void)
{
    RCC_ClocksTypeDef RCC_ClockFreq;

    RCC_GetClocksFreq(&RCC_ClockFreq);

    CV_LOG("sysclk: %d\r\n", RCC_ClockFreq.SYSCLK_Frequency);
    CV_LOG("hclk: %d\r\n", RCC_ClockFreq.HCLK_Frequency);
    CV_LOG("pclk1: %d\r\n", RCC_ClockFreq.PCLK1_Frequency);
    CV_LOG("pclk2: %d\r\n", RCC_ClockFreq.PCLK2_Frequency);
}
