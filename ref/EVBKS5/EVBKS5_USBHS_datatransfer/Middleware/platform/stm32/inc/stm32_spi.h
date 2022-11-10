/**
  ******************************************************************************
  * @file    stm32_spi.h
  * @author  iclm team
  * @brief   spi header file
  ******************************************************************************
  */
#ifndef __STM32_SPI_H__
#define __STM32_SPI_H__

#ifdef __cplusplus
 extern "C" {
#endif 

#if defined(STM32F429xx)
#include "stm32f4xx_hal.h"
#endif

#if defined(STM32H750xx) || defined(STM32H743xx)
#include "stm32h7xx_hal.h"
#endif

#include "global_conf.h"

typedef enum
{
    DMA_RECV_FLAG_MEM_0        = 0,
    DMA_RECV_FLAG_MEM_1        = 1,
    DMA_RECV_FLAG_MAX
}dmaRecvFlagEnum;

#define SPI_FRAME_DLEN_MAX      RADAR_DATA_MAX_LEN
#define SPI_FRAME_HLEN          (4)
#define SPI_FRAME_TLEN          (4)
#define SPI_FRAME_LEN_MAX       (SPI_FRAME_DLEN_MAX + SPI_FRAME_HLEN + SPI_FRAME_TLEN)

#define DATA_RECV_BUF_SIZE           (SPI_FRAME_LEN_MAX * 2) /*ping-pong buffer*/

extern uint8_t g_dataRecvBuf[CHANNEL_MAX][DATA_RECV_BUF_SIZE];
extern volatile uint8_t g_dataRecvFlag[CHANNEL_MAX][DMA_RECV_FLAG_MAX];

void SPI_Init(uint16_t dmaBufLen, uint8_t dataType);
void SPI_DeInit(void);
void HAL_SPI_MspInit(SPI_HandleTypeDef* spiHandle);
void HAL_SPI_MspDeInit(SPI_HandleTypeDef* spiHandle);

#ifdef __cplusplus
}
#endif

#endif

