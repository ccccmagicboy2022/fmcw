/**
  ******************************************************************************
  * @file           : dataprocess.h
  * @author         : iclm team
  * @brief          : data process header file
  ******************************************************************************
  */
#ifndef __DATAPROCESS_H__
#define __DATAPROCESS_H__

#ifdef __cplusplus
 extern "C" {
#endif 

#include "platform.h"

typedef enum  
{
	DATA_STATE_HEAD = 0,
    DATA_STATE_ID,
    DATA_STATE_INDEX1,
    DATA_STATE_INDEX2,
    DATA_STATE_DATA,
    DATA_STATE_TAIL0,
    DATA_STATE_TAIL1,
    DATA_STATE_TAIL2,
    DATA_STATE_TAIL3
}dataStateEnum;

#define DATA_HEAD		          (0xAA)
#define DATA_TAIL		          (0x55)
#define ID_MASK                   (0xF0)

#define FFT0_ID                   (0x30)
#define FFT1_ID                   (0x70)

#define DSRAW0_ID                   (0x20)
#define DSRAW1_ID                   (0x60)

#define CHIRP_INDEX_POS0          (5)
#define CHIRP_INDEX_POS1          (3)
#define CHIRP_INDEX_MASK          (0x0F)
#define SKIP_NUM                  (4)

#define DFFT0_ID                  (0xB0)
#define DFFT1_ID                  (0xF0)
#define FRAME_CNT_POS             (8)

#define DFFT_PEAK_ID              (0x40)

#define DATA_PROC_STACK_SIZE      (512)
#define RADAR_DATA_QUEUE_SIZE     (2)

typedef struct RADAR_DATA_PARSE
{
    uint8_t  buf[SPI_FRAME_LEN_MAX];
    uint8_t  state;
    uint16_t chirpIndex;
    uint16_t frameCnt;
    uint16_t curIndex;
    uint16_t needCopyLen;
}RADAR_DATA_PARSE_T;

typedef struct RADAR_PARA
{
    uint8_t dataType;
    uint16_t dataLen;
    uint16_t chirpNum;
}RADAR_PARA_T;

void DataProc_Recv(void);
void DataProc_Init(void);
uint8_t DataProc_GetRadarDataType(void);
uint16_t DataProc_GetRadarDataLen(void);
uint8_t DataProc_NeedReconfig(void);
void DataProc_ResetRecvCnt(void);

#ifdef STM32_PLATFORM
void DataProc_TaskInit(void);
void DataProc_Send2RadarDataQueue(uint8_t channel, void *radarData);
void DataProc_ResetRadarDataQueue(void);
#endif

#ifdef __cplusplus
}
#endif

#endif


