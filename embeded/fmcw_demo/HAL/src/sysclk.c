#include "sys.h"
#include "sysclk.h"

void SysClkInit(void)
{
    //io
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
    
    //uart
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    //radar
    //spi4
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI4, ENABLE);
    //input capture
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    //dac
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);
    //adc
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    //spi3
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
}


void clk_test(void)
{
    RCC_ClocksTypeDef RCC_ClockFreq;

    RCC_GetClocksFreq(&RCC_ClockFreq);

    CV_LOG("sysclk: %d Hz\r\n", RCC_ClockFreq.SYSCLK_Frequency);
    CV_LOG("hclk: %d Hz\r\n", RCC_ClockFreq.HCLK_Frequency);
    CV_LOG("pclk1: %d Hz\r\n", RCC_ClockFreq.PCLK1_Frequency);
    CV_LOG("pclk2: %d Hz\r\n", RCC_ClockFreq.PCLK2_Frequency);
}
