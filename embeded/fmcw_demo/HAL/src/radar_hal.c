#include "radar_hal.h"
#include "sys.h"

void radar_init(void)
{
    spi4_init();
    //write regs
    
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