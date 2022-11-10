/**
  ******************************************************************************
  * @file           : banyan.c
  * @author         : iclm team
  * @brief          : banyan driver
  ******************************************************************************
  */
#include <stdio.h>
#include "global_conf.h"
#include "platform.h"
#include "banyan.h"
#include "config.h"
#include "banyan_para.c"
uint8_t g_ChannelCount = CHANNEL_MAX;
uint8_t g_maxChannelCount = CHANNEL_MAX;
uint8_t g_TxCount = (CHANNEL_MAX + 1) / 2;

static uint16_t rawPointMap[RAW_MAP_NUM] = 
{
	RAW_POINT_64,
    RAW_POINT_128,
    RAW_POINT_256,
    RAW_POINT_512,
    RAW_POINT_1024
};

static const RADAR_REG_T RegList_LowPWR[] =
{
	{0x70, 0x1020},
	{0x6C, 0x8880},
	{0x6D, 0x8800},
	{0x72, 0x0650},
	{0x67, 0x0000},
	{0x66, 0xF0F0},
	{0x6E, 0x03FC},
	{0x41, 0x4804},
	{0x00, 0x0000} 
};

static const RADAR_REG_T RegList_NormalPWR[] =
{
	{0x41, 0xc864},
	{0x72, 0x0653},
	{0x6C, 0x9990},
	{0x6D, 0x9940},
	{0x70, 0x32a0},
	{0x6E, 0xabFC},
	{0x66, 0x0a00},
	{0x67, 0x1840},
	{0x00, 0x0000} 
};
    
uint16_t Radar_GetFftPoint(void)
{
	uint16_t regVal = 64;
    I2C_Read(I2C_ADDR_BanYan_Chip0, BANYAN_DIG_FFT_NUM, &regVal);
	return regVal;
}

uint16_t Radar_GetRawPoint(void)
{
    uint16_t val = 256;
    uint16_t rawVal = 0;
    
    I2C_Read(I2C_ADDR_BanYan_Chip0, BANYAN_DIG_RAW_PEAK_NUM, &val);
    rawVal = (val >> BANYAN_RAW_POS) & BANYAN_RAW_MASK;

    if (rawVal < RAW_MAP_NUM)
    {
        return rawPointMap[rawVal];
    }
    else
    {
        return RAW_POINT_64;
    }
}

uint16_t Radar_GetOneFrameChirpNum(void)
{
	uint16_t val = 32;
    I2C_Read(I2C_ADDR_BanYan_Chip0, BANYAN_PAT_CHIRP_NUM, &val);
	return val;
}

uint16_t Radar_GetDfftDataNum(void)
{
    uint16_t val = 32;
    
    I2C_Read(I2C_ADDR_BanYan_Chip0, BANYAN_DIG_DFFT_DATA_NUM, &val);

    return (val >> BANYAN_DFFT_DATA_NUM_POS);
}

uint16_t Radar_GetDfftPeakSize(void)
{
    uint16_t val = 32;
    
    I2C_Read(I2C_ADDR_BanYan_Chip0, BANYAN_DIG_RAW_PEAK_NUM, &val);

    return ((val & BANYAN_PEAK_MASK) * 4); /*4--word length*/
}

uint16_t Radar_GetDfftChirpNum(void)
{
    uint16_t val = 32;
    
    I2C_Read(I2C_ADDR_BanYan_Chip0, BANYAN_DIG_DFFT_CHIRP_NUM, &val);

    return (val >> BANYAN_DFFT_CHIRP_NUM_POS);
}

uint8_t Radar_GetDataType(void)
{
    uint8_t dataType = DATA_TYPE_FFT;
    uint16_t val = BANYAN_FFT_DATA;
    
    I2C_Read(I2C_ADDR_BanYan_Chip0, BANYAN_DIG_FUN_SWITCH, &val);

    if (val & BANYAN_DFFT_DATA)
    {
        dataType = DATA_TYPE_DFFT;
    }
    else if (val & BANYAN_FFT_DATA)
    {
        dataType = DATA_TYPE_FFT;
    }
    else if (val & BANYAN_DFFT_PEAK_DATA)
    {
        dataType = DATA_TYPE_DFFT_PEAK;
    }
	else if (val & BANYAN_DSRAW_DATA)
    {
        dataType = DATA_TYPE_DSRAW;
    }
    else
    {
        dataType = DATA_TYPE_MAX;
    } 

    return dataType;
}

void Radar_Enter_LowPWR(uint16_t devAddr)
{
    uint16_t loop = 0;
    while(RegList_LowPWR[loop].addr) 
    {
        I2C_Write(devAddr, (uint8_t)(RegList_LowPWR[loop].addr), RegList_LowPWR[loop].val);
        loop++;
    } 	
}

void Radar_Exit_LowPWR(uint16_t devAddr)
{
    uint16_t loop = 0;
    while(RegList_NormalPWR[loop].addr) 
    {
        I2C_Write(devAddr, (uint8_t)(RegList_NormalPWR[loop].addr), RegList_NormalPWR[loop].val);
        loop++;
    } 		
}

void Radar_Init(void)
{
    uint16_t loop = 0;

#ifdef CONFIG_DEBUG
    printf("radar flash value:\r\n");
    while(InitRegList[loop].addr) 
    {
        printf("%02X=%04X\r\n", InitRegList[loop].addr, InitRegList[loop].val);
        loop++;
    } 
    loop = 0;
#endif

    while(InitRegList[loop].addr) 
    {
        I2C_Write(I2C_ADDR_BanYan_Chip0, (uint8_t)(InitRegList[loop].addr), InitRegList[loop].val);
        loop++;
    } 

	
#ifdef CONFIG_DEBUG
    loop = 0;
    printf("radar ic value:\r\n");
	  uint16_t val = 0;
    while(InitRegList[loop].addr) 
    {
		    I2C_Read(I2C_ADDR_BanYan_Chip0, InitRegList[loop].addr, &val);
        printf("%02X=%04X\r\n", InitRegList[loop].addr, val);
        loop++;
    } 
#endif
}

void Radar_UpdateReg(uint16_t addr, uint16_t val)/*currently only update existing reg*/
{
    uint16_t loop = 0;

    while(InitRegList[loop].addr) 
    {   
        if (loop < REG_FIX_NUM)
        {
            loop++;
            continue;
        }
        
        if (InitRegList[loop].addr == addr)
        {
            InitRegList[loop].val = val;
            Config_NeedFlashWrite();
            return;
        }
        loop++;
    }
}

void* Radar_GetRadarParaAddr(void)
{
    return (void*)&InitRegList;
}

uint32_t Radar_GetRadarParaLen(void)
{
    return sizeof(InitRegList);
}


