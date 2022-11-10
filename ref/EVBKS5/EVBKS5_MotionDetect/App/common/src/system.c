/**
  ******************************************************************************
  * @file           : system.c
  * @author         : iclm team
  * @brief          : system module
  ******************************************************************************
  */
#include <stdio.h>
#include <string.h>
#include "global_conf.h"
#include "utilities.h"
#include "system.h"
#include "platform.h"
#include "dataprocess.h"

static SYS_PARA_T sysPara __ALIGN(4);

void* System_GetSysParaAddr(void)
{
    return (void*)&sysPara;
}

uint32_t System_GetSysParaLen(void)
{
    return sizeof(sysPara);
}

uint16_t System_GetSysMode(void)
{
    return sysPara.systemMode;
}

uint16_t System_GetUploadSampleRate(void)
{
    return sysPara.uploadSampleRate;
}

uint16_t System_GetDebugMode(void)
{
    return sysPara.debugMode;
}

void System_ParaInit(void)
{
#ifdef SUPPORT_DYNAMIC_SYS_MODE
    sysPara.systemMode = SYS_MODE_DEFAULT;
#endif
    sysPara.uploadSampleRate = UPLOAD_SAMPLE_RATE;
    sysPara.debugMode = DEBUG_MODE_DEFAULT;
}

int8_t System_ParaUpdate(uint16_t type, int32_t val)
{
    switch (type)  
    {
#ifdef SUPPORT_DYNAMIC_SYS_MODE
        case SYS_SYSTEM_MODE:
            sysPara.systemMode = (int16_t)val;
            break;
#endif
			
#if defined(SUPPORT_DATA_PASSTHROUGH) || defined(SUPPORT_DYNAMIC_SYS_MODE)
        case SYS_UPLOAD_SP_RATE:
            sysPara.uploadSampleRate = (int16_t)val;
            DataProc_ResetRecvCnt();
            break;
#endif
		
       case SYS_DEBUG_MODE:
            sysPara.debugMode = (int16_t)val;
            break;
        
        default:
            return -1;
    }

    return 0;
}

int32_t System_ParaRead(uint16_t type)
{
    switch (type)  
    {
        case SYS_SYSTEM_MODE:
            return sysPara.systemMode;

        case SYS_UPLOAD_SP_RATE:
            return sysPara.uploadSampleRate;

       case SYS_DEBUG_MODE:
            return sysPara.debugMode;

        default:
            return 0x7fffffff; /*invalid value*/
    }
}

void System_Reconfig(void)
{
    uint16_t dmaBufLen = 0;
    uint8_t dataType = 0;

#if defined(CONFIG_DEBUG)
    printf("system reconfig check...\r\n");
#endif
	
	  DataProc_NeedReconfig();

#if defined(CONFIG_DEBUG)
    printf("system now do reconfig!\r\n");
#endif
    dmaBufLen = DataProc_GetRadarDataLen() * 2;
    dataType = DataProc_GetRadarDataType();
    SPI_DeInit();

    SPI_Init(dmaBufLen, dataType);
}


