#include "radar_hal.h"
#include "sys.h"

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

}

void dac_init(void)
{

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

