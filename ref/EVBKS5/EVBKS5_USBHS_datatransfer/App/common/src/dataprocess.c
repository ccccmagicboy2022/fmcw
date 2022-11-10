/**
  ******************************************************************************
  * @file           : dataprocess.c
  * @author         : iclm team
  * @brief          : data process module
  ******************************************************************************
  */
#include <stdio.h>
#include <string.h>
#include "global_conf.h"
#include "utilities.h"
#include "dataprocess.h"
#include "banyan.h"
#include "cmdprocess.h"
#include "system.h"
#ifdef STM32_PLATFORM
#include "rtos.h"
#include "cmsis_os.h"
#include "usb_transfer.h"
#endif
#include "platform.h"

static RADAR_DATA_PARSE_T RadarDataParse[CHANNEL_MAX];
static RADAR_PARA_T RadarPara;

#if defined(DEVMB) || defined(DEVMB_PLUS)
static uint8_t chipIndex = 0;
#include "fftzerofill.h"
#include "arm_math.h"
#include "arm_const_structs.h"
#ifdef USE_FFT_Q31
static q31_t fftInOutBuf[2048] = {0};
static const arm_cfft_instance_q31* fftCalSize[9] = 
{
	&arm_cfft_sR_q31_len16,
	&arm_cfft_sR_q31_len32,
	&arm_cfft_sR_q31_len64,
	&arm_cfft_sR_q31_len128,
	&arm_cfft_sR_q31_len256,
	&arm_cfft_sR_q31_len512,
	&arm_cfft_sR_q31_len1024,
	&arm_cfft_sR_q31_len2048,
	&arm_cfft_sR_q31_len4096
};
#else
static q15_t fftInOutBuf[2048] = {0};
static const arm_cfft_instance_q15* fftCalSize[9] = 
{
	&arm_cfft_sR_q15_len16,
	&arm_cfft_sR_q15_len32,
	&arm_cfft_sR_q15_len64,
	&arm_cfft_sR_q15_len128,
	&arm_cfft_sR_q15_len256,
	&arm_cfft_sR_q15_len512,
	&arm_cfft_sR_q15_len1024,
	&arm_cfft_sR_q15_len2048,
	&arm_cfft_sR_q15_len4096
};
#endif
static q31_t fftWindowData[1024] = {0};
#endif

#ifndef STM32_PLATFORM
static uint8_t SendData[CHANNEL_MAX][SPI_FRAME_LEN_MAX];
#endif

static void CheckChirpIndex(uint8_t channel, uint8_t chirpIndex)
{
    static uint8_t curIndex[CHANNEL_MAX] = {0};
    static uint8_t oldCurIndex[CHANNEL_MAX] = {0};
    static uint8_t skipNum = SKIP_NUM;

    if (channel >= CHANNEL_MAX)
    {
        return;
    }
        
	if (skipNum) 
    {
		skipNum--;
		curIndex[channel] = oldCurIndex[channel] = chirpIndex % RadarPara.chirpNum;
	} 
    else 
    {
		curIndex[channel] = chirpIndex % RadarPara.chirpNum;
		if (curIndex[channel] != ((oldCurIndex[channel] + 1) % RadarPara.chirpNum))
        {
			Indicator_RadarDataIndexError();
		}
		oldCurIndex[channel] = curIndex[channel];
	}
}

static void CheckFrameCnt(uint8_t channel, uint16_t frameCnt)
{
    static uint16_t oldFrameCnt[CHANNEL_MAX] = {0};
    static uint8_t skipNum = SKIP_NUM;

    if (channel >= CHANNEL_MAX)
    {
        return;
    }
    
	if (skipNum) 
    {
		skipNum--;
		oldFrameCnt[channel] = frameCnt;
	} 
    else if (frameCnt != oldFrameCnt[channel] + 1)
    {
        Indicator_RadarDataIndexError();
    }
    oldFrameCnt[channel] = frameCnt;
}

static void DataStateIdParse(uint8_t data, uint8_t channel)
{
    uint8_t flag = 0;
    
    switch (RadarPara.dataType)
    {
        case DATA_TYPE_FFT:
            if ((data & ID_MASK) == FFT0_ID || (data & ID_MASK) == FFT1_ID)
            {
				if(channel > 1)
				{
					data += 0x20;
				}
                RadarDataParse[channel].chirpIndex = (data & CHIRP_INDEX_MASK) << CHIRP_INDEX_POS0;
                flag = 1;
            }
            break;
        case DATA_TYPE_DSRAW:
            if ((data & ID_MASK) == DSRAW0_ID || (data & ID_MASK) == DSRAW1_ID)
            {
#if defined(DEVMB) || defined(DEVMB_PLUS)
				if(FFTZero_GetEnable())
				{
					data += 0x10;
				}
#endif
				if(channel > 1)
				{
					data += 0x20;
				}
                RadarDataParse[channel].chirpIndex = (data & CHIRP_INDEX_MASK) << CHIRP_INDEX_POS0;
                flag = 1;
            }
            break;
        case DATA_TYPE_DFFT:
			if ((data & ID_MASK) == DFFT0_ID || (data & ID_MASK) == DFFT1_ID)
            {
				if(channel > 1)
				{
					data -= 0x80;
				}
                flag = 1;
			}
            break;
        
        case DATA_TYPE_DFFT_PEAK:
            if ((data & ID_MASK) == DFFT_PEAK_ID)
            {
#if defined(DEVMB) || defined(DEVMB_PLUS)
				if(g_TxCount > 1)
				{		
 					if(chipIndex % 2 == 0)
					{
						data += 0x4;
					}	
		
				}
				if(channel > 1)
				{
					data += 0x1;
				}
#endif				
                flag = 1;
			}
            break;
        
        default:
            break;
    }

    if (flag)
    {
        RadarDataParse[channel].buf[RadarDataParse[channel].curIndex++] = data;
        RadarDataParse[channel].state = DATA_STATE_INDEX1;
    }
    else
    {
        RadarDataParse[channel].state = DATA_STATE_HEAD;
    }
}

static void DataStateIndex1Parse(uint8_t data, uint8_t channel)
{
    switch (RadarPara.dataType)
    {
		case DATA_TYPE_DSRAW:
        case DATA_TYPE_FFT:
            RadarDataParse[channel].chirpIndex += data >> CHIRP_INDEX_POS1;
            break;
        
        case DATA_TYPE_DFFT:
            RadarDataParse[channel].frameCnt = data << FRAME_CNT_POS;
            break;
        
        case DATA_TYPE_DFFT_PEAK:
            break;
        
        default:
            break;
    }

    RadarDataParse[channel].buf[RadarDataParse[channel].curIndex++] = data;
    RadarDataParse[channel].state = DATA_STATE_INDEX2;
}

static void DataStateIndex2Parse(uint8_t data, uint8_t channel)
{
    switch (RadarPara.dataType)
    {
        case DATA_TYPE_FFT:
            break;
        
        case DATA_TYPE_DFFT:
            RadarDataParse[channel].frameCnt += data;
            break;
        
        case DATA_TYPE_DFFT_PEAK:
            break;
        
        default:
            break;
    }
    
    RadarDataParse[channel].buf[RadarDataParse[channel].curIndex++] = data;
    RadarDataParse[channel].state = DATA_STATE_DATA;
}

static uint8_t DataStateTail2Parse(uint8_t data, uint8_t channel)
{
    switch (RadarPara.dataType)
    {
        case DATA_TYPE_FFT:
			if(channel > 1)
			{
				data += 0x20;
			}
            break;
				
        case DATA_TYPE_DSRAW:
			if(channel > 1)
			{
				data += 0x20;
			}
            
            break;
        
        default:
            break;
    }

	RadarDataParse[channel].buf[RadarDataParse[channel].curIndex++] = data;
	RadarDataParse[channel].state = DATA_STATE_TAIL3;
    return 1;
}

static uint8_t DataStateTail3Parse(uint8_t data, uint8_t channel)
{
    RadarDataParse[channel].state = DATA_STATE_HEAD;
    if (data != DATA_TAIL) 
    {
        return 0;
    }
    
    switch (RadarPara.dataType)
    {
		case DATA_TYPE_DSRAW:
        case DATA_TYPE_FFT:
            if ((RadarDataParse[channel].chirpIndex == RadarPara.chirpNum - 1) && (channel == CHANNEL_MAX - 1))
            {
                CmdProc_AdcReset();
				CmdProc_NopConfig();
            }
            break;
        
        case DATA_TYPE_DFFT:             
            if (channel == CHANNEL_MAX - 1)
            {
                CmdProc_AdcReset();
				CmdProc_NopConfig();
            }
            break;
        
        case DATA_TYPE_DFFT_PEAK:
            break;
        
        default:
            break;
    }
    
    RadarDataParse[channel].buf[RadarDataParse[channel].curIndex++] = data;
    
    return 1;
}

static void DataCopy(uint8_t* buf, uint16_t len, uint8_t channel, uint16_t *i)
{
    uint16_t copyLen = 0;

    if (NULL == buf || NULL == i)
    {
        return;
    }
    
	copyLen = (RadarDataParse[channel].needCopyLen > (len - *i))?
            (len - *i) : RadarDataParse[channel].needCopyLen;
    memcpy(&RadarDataParse[channel].buf[RadarDataParse[channel].curIndex], &buf[*i], copyLen);

    RadarDataParse[channel].curIndex += copyLen;
    *i += (copyLen - 1);
    RadarDataParse[channel].needCopyLen -= copyLen;

    if (!RadarDataParse[channel].needCopyLen)
    {
    	RadarDataParse[channel].state = DATA_STATE_TAIL0;
        RadarDataParse[channel].needCopyLen = RadarPara.dataLen - SPI_FRAME_HLEN - SPI_FRAME_TLEN;
    }
}

static uint8_t DataParse(uint8_t* buf, uint16_t len, uint8_t channel, uint16_t* left)
{
    uint16_t i = 0;
    uint8_t parseFinish = 0;
	*left = len;
	
    if (NULL == buf || 0 == len || channel >= CHANNEL_MAX)
    {
        return 0;
    }

	for (i = 0; (i < len) && (parseFinish == 0); i++) 
    {		
		switch(RadarDataParse[channel].state)
		{                    
			case DATA_STATE_HEAD:
				if (buf[i] == DATA_HEAD) 
                {
                    RadarDataParse[channel].curIndex = 0;
                    RadarDataParse[channel].buf[RadarDataParse[channel].curIndex++] = buf[i];
					RadarDataParse[channel].state = DATA_STATE_ID;
				}
				break;
                
			case DATA_STATE_ID:
                DataStateIdParse(buf[i], channel);
				break;
                
			case DATA_STATE_INDEX1:
                DataStateIndex1Parse(buf[i], channel);
				break;
                
			case DATA_STATE_INDEX2:
                DataStateIndex2Parse(buf[i], channel);
				break;
            
			case DATA_STATE_DATA:				
                DataCopy(buf, len, channel, &i);
				break;

            case DATA_STATE_TAIL0:
                RadarDataParse[channel].buf[RadarDataParse[channel].curIndex++] = buf[i];
				RadarDataParse[channel].state = DATA_STATE_TAIL1;
				break;

            case DATA_STATE_TAIL1:
                RadarDataParse[channel].buf[RadarDataParse[channel].curIndex++] = buf[i];
				RadarDataParse[channel].state = DATA_STATE_TAIL2;
				break;

            case DATA_STATE_TAIL2:
				DataStateTail2Parse(buf[i], channel);
				break;
                
   		    case DATA_STATE_TAIL3:
                parseFinish = DataStateTail3Parse(buf[i], channel);
				break;
			
			default:
				RadarDataParse[channel].state = DATA_STATE_HEAD;
				break;

		}
	}

	*left -= i;
	
    return parseFinish;
}

#if defined(SUPPORT_DATA_PASSTHROUGH) || defined(SUPPORT_DYNAMIC_SYS_MODE)
static uint16_t nRecvCnt[CHANNEL_MAX] = {0};
void DataProc_ResetRecvCnt(void)
{
    memset(nRecvCnt, 0, sizeof(nRecvCnt));
}

static void StartDataTransfer(uint8_t* frameBuf, uint16_t bufLen, uint8_t channel, uint16_t index)
{	
		static uint16_t nRecvCnt = 0;
		uint8_t directSend = 0;
	  uint16_t uploadSampleRate = System_GetUploadSampleRate();
		
	  if (nRecvCnt++ % uploadSampleRate == 0) 
		{
			  UsbTransfer_Send(frameBuf, bufLen, directSend); 
		}

}
#endif

void StartAlgorithm(uint8_t* dataBuf, uint16_t dataLen, uint8_t channel, uint16_t index)
{
		/*do algo here*/
}

static void DataDispatch(uint8_t* frameBuf, uint16_t bufLen, uint8_t channel, uint16_t index)
{
    uint8_t* dataBuf = NULL;
    uint16_t dataLen = 0;

    if (NULL == frameBuf || 0 == bufLen)
    {
        return;
    }
	
    dataBuf = frameBuf + SPI_FRAME_HLEN;
    dataLen = bufLen - SPI_FRAME_HLEN - SPI_FRAME_TLEN;

#if defined (SUPPORT_DATA_PASSTHROUGH)
    StartDataTransfer(frameBuf, bufLen, channel, index);
#else
		StartAlgorithm(dataBuf, dataLen, channel, index);
#endif

}

static void DataProcess(uint8_t channel, uint8_t dmaFlag, uint8_t *recvBuf, uint16_t bufLen)
{
    uint8_t parseFinish = 0;
    uint16_t bufLeftLen = bufLen;
    uint16_t index = 0;
    uint16_t threshold = INDICATOR_RECV_THRESHOLD;
    
    if (channel >= CHANNEL_MAX || dmaFlag >= DMA_RECV_FLAG_MAX || NULL == recvBuf)
    {
        printf("Error para!\r\n");
        return;
    }
	
    if (CmdProc_InCmdMode())
    {
        return;
    }

	while (bufLeftLen > 0)
	{
		parseFinish = DataParse(
			recvBuf + bufLen - bufLeftLen,
			bufLeftLen,
			channel,
			&bufLeftLen);
		g_dataRecvFlag[channel][dmaFlag] = 0;
		if (!parseFinish)
		{
			continue;
		}
		
		switch (RadarPara.dataType)
		{
			case DATA_TYPE_DSRAW:
			case DATA_TYPE_FFT:
				CheckChirpIndex(channel, RadarDataParse[channel].chirpIndex);
				index = RadarDataParse[channel].chirpIndex;
				break;
			
			case DATA_TYPE_DFFT:
				CheckFrameCnt(channel, RadarDataParse[channel].frameCnt);	
				index = RadarDataParse[channel].frameCnt;
				threshold >>= INDICATOR_RECV_THD_DFFT_SHIFT;
				break;
			
			case DATA_TYPE_DFFT_PEAK:
				threshold >>= INDICATOR_RECV_THD_DPEAK_SHIFT;
				break;
			
			default:
				break;
		}

		
		Indicator_RadarDataReceived(threshold);
		DataDispatch(RadarDataParse[channel].buf, RadarPara.dataLen, channel, index);	
	}	
}

void DataProc_Recv(void)
{
    uint8_t channel = 0;
    uint8_t dmaFlag = 0;
    uint16_t dataPos = 0;

    for (channel = 0; channel < g_ChannelCount; channel++)
    {
        for (dmaFlag = 0; dmaFlag < DMA_RECV_FLAG_MAX; dmaFlag++)
        {
            if (!g_dataRecvFlag[channel][dmaFlag])
            {
                continue;
            }
            
            if (DMA_RECV_FLAG_MEM_0 == dmaFlag)
            {
                dataPos = 0;
            }
            else
            {
                dataPos = RadarPara.dataLen;
            }
            
            DataProcess(channel, dmaFlag, &g_dataRecvBuf[channel][dataPos], RadarPara.dataLen);
        }
    }
}

static int8_t GetRadarPara(RADAR_PARA_T *radarPara)
{
#if defined(DEVMB) || defined (EVBKS5) || defined(DEVMB_PLUS)
    uint8_t dfftDataNum = 0;
    uint8_t dfftChirpNum = 0;
#endif

    if (NULL == radarPara)
    {
        return FAIL;
    }
    
    radarPara->dataType = Radar_GetDataType();
#if defined(CONFIG_DEBUG)
    printf("dataType: %d\r\n", radarPara->dataType);
#endif
    switch (radarPara->dataType)
    {
		case DATA_TYPE_DSRAW:
            radarPara->dataLen = Radar_GetRawPoint() * 2 * 2 + SPI_FRAME_HLEN + SPI_FRAME_TLEN;  /*16 bit, IQ*/
            break;
        
        case DATA_TYPE_FFT:
            radarPara->dataLen = Radar_GetFftPoint() * 2 * 2 + SPI_FRAME_HLEN + SPI_FRAME_TLEN;  /*16 bit, IQ*/
            break;
        
#if defined(DEVMB) || defined (EVBKS5) || defined(DEVMB_PLUS)
        case DATA_TYPE_DFFT:
            dfftDataNum = Radar_GetDfftDataNum();
            dfftChirpNum = Radar_GetDfftChirpNum();
#if defined(CONFIG_DEBUG)
            printf("dfftDataNum: %d, dfftChirpNum: %d\r\n", dfftDataNum, dfftChirpNum);
#endif
            radarPara->dataLen = dfftDataNum * 2 * 2 * dfftChirpNum + SPI_FRAME_HLEN + SPI_FRAME_TLEN;  /*16 bit, IQ*/
            break;
#endif
        
        case DATA_TYPE_DFFT_PEAK:
            radarPara->dataLen = Radar_GetDfftPeakSize() + SPI_FRAME_HLEN + SPI_FRAME_TLEN;
            break;
        
        default:
            printf("Error: unsupport dataType\r\n");
            return FAIL;
    }

#if defined(CONFIG_DEBUG)
    printf("dataLen: %d\r\n", radarPara->dataLen);
#endif
    if (radarPara->dataLen > SPI_FRAME_LEN_MAX)
    {
        printf("Error: dataLen is too long\r\n");
        return FAIL;
    }

    radarPara->chirpNum = Radar_GetOneFrameChirpNum();
#if defined(CONFIG_DEBUG)
    printf("chirpNum: %d\r\n", radarPara->chirpNum);
#endif
    
    return OK;
}

void DataProc_Init(void)
{
    uint8_t channel = 0;
    
    memset(&RadarDataParse, 0 ,sizeof(RadarDataParse));
    memset(&RadarPara, 0 ,sizeof(RadarPara));

    if (FAIL == GetRadarPara(&RadarPara))
    {
        //RunFailed((uint8_t *)__FILE__, __LINE__);
    }

    for (channel = 0; channel < CHANNEL_MAX; channel++)
    {
        RadarDataParse[channel].needCopyLen = RadarPara.dataLen - SPI_FRAME_HLEN - SPI_FRAME_TLEN;
    }

    SPI_Init(RadarPara.dataLen * 2, RadarPara.dataType); /*radar data received by spi dma, ping-pang buffer*/
}

uint8_t DataProc_NeedReconfig(void)
{    
    uint8_t channel = 0;
    uint8_t needReconfig = 0;
    RADAR_PARA_T radarParaTmp = {0};
    
    if (FAIL == GetRadarPara(&radarParaTmp))
    {
        //RunFailed((uint8_t *)__FILE__, __LINE__);
    }

    RadarPara.chirpNum = radarParaTmp.chirpNum;
    if ((radarParaTmp.dataType != RadarPara.dataType) || (radarParaTmp.dataLen != RadarPara.dataLen))
    {
        needReconfig = 1;
        RadarPara.dataType = radarParaTmp.dataType;
        RadarPara.dataLen = radarParaTmp.dataLen;
        memset(&RadarDataParse, 0 ,sizeof(RadarDataParse));
        for (channel = 0; channel < CHANNEL_MAX; channel++)
        {
            RadarDataParse[channel].needCopyLen = RadarPara.dataLen - SPI_FRAME_HLEN - SPI_FRAME_TLEN;
        }
    }
#if defined(DEVMB) || defined(DEVMB_PLUS)
	ResetFFTWindow();
	FFTZero_ResizeOffsetAndOutput();
#endif
    return needReconfig;
}

uint8_t DataProc_GetRadarDataType(void)
{    
    return RadarPara.dataType;
}

uint16_t DataProc_GetRadarDataLen(void)
{    
    return RadarPara.dataLen;
}

#ifdef STM32_PLATFORM
osThreadId dataProcTaskHandle;
QueueHandle_t radarDataQueue[CHANNEL_MAX];

void DataProc_Send2RadarDataQueue(uint8_t channel, void *radarData)
{
	BaseType_t xHigherPriorityTaskWoken = 0;

    if (channel >= CHANNEL_MAX || NULL == radarData)
    {
        return;
    }
    
	if (xQueueSendFromISR(radarDataQueue[channel], radarData, &xHigherPriorityTaskWoken) != pdPASS) 
    {
		Indicator_RadarDataRecvOverFlow();
	}

	portYIELD_FROM_ISR(xHigherPriorityTaskWoken); 
}


void DataProcTask(void const * argument)
{
	RADAR_DATA_T radarData = {0};
    uint16_t loop = 0;
        
    while(1)
    { 
        for (loop = 0; loop < g_ChannelCount; loop++)
        {
#if defined(DEVMB) || defined(DEVMB_PLUS)
			if(loop == 3)
				chipIndex++;
            if (DATA_TYPE_DFFT_PEAK == RadarPara.dataType)
            {
                if (loop % 2 == 0)
                {
                    continue;
                }
            }
#else
            if (DATA_TYPE_DFFT_PEAK == RadarPara.dataType)
            {
                if (loop % 2 != 0)
                {
                    continue;
                }
            }
#endif          
            if (xQueueReceive(radarDataQueue[loop], &radarData, portMAX_DELAY))
            {
                DataProcess(loop, radarData.dmaFlag, radarData.buf, radarData.len);
            }
        }
    } 
}

void DataProc_ResetRadarDataQueue(void)
{
	uint16_t loop = 0;
	for (loop = 0; loop < CHANNEL_MAX; loop++)
    {
		xQueueReset(radarDataQueue[loop]);
    }
}

void DataProc_TaskInit(void)
{
    uint16_t loop = 0;

    for (loop = 0; loop < CHANNEL_MAX; loop++)
    {
        radarDataQueue[loop] = xQueueCreate(RADAR_DATA_QUEUE_SIZE, sizeof(RADAR_DATA_T));
    }

    osThreadDef(dataProcTask, DataProcTask, osPriorityHigh, 0, DATA_PROC_STACK_SIZE);
    dataProcTaskHandle = osThreadCreate(osThread(dataProcTask), NULL);
    if (NULL == dataProcTaskHandle)
    {
        RunFailed((uint8_t *)__FILE__, __LINE__);
    }
}
#endif

