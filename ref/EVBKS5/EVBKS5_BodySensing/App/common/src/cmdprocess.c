/**
  ******************************************************************************
  * @file           : cmdprocess.c
  * @author         : iclm team
  * @brief          : command process module
  ******************************************************************************
  */
#include <stdio.h>
#include <string.h>
#include "global_conf.h"
#include "platform.h"
#include "cmdprocess.h"
#include "banyan.h"
#include "config.h"
#include "utilities.h"
#include "system.h"
#include "usbd_cdc_if.h"

#include "bodysensing_types.h"
#include "body_presence.h"
#include "AlgorithmConfig.h"

static uint8_t enableResetAdc = 0;
static uint16_t resetDelayMs = RESET_DELAY_TIME;

static CMD_PARSE_T CmdDataParse;
static uint8_t CmdModeFlag = 0;
static uint8_t DebugModeFlag = 0;

static uint8_t CmdHead[CMD_HEAD_MAX] = {0xFD, 0xFC, 0xFB, 0xFA};
static uint8_t CmdTail[CMD_TAIL_MAX] = {0x04, 0x03, 0x02, 0x01};
static uint8_t CmdAck[CMD_LEN_MAX] __ALIGN(4) = {0};

static uint16_t masterProtocolVer = 0;

static uint8_t enableNopConfig = 0;
static uint8_t nopConfigStartRegisterIndex = 0;
static uint8_t nopConfigStopRegisterIndex = 0;
static uint16_t nopConfigStartMs = RESET_DELAY_TIME;
static uint16_t nopConfigStopMs = RESET_DELAY_TIME + 2;
int32_t nopConfigStartRegister[16] = {0};
int32_t nopConfigStopRegister[16] = {0};


void CmdProc_AdcReset(void)
{
    if (enableResetAdc && CmdModeFlag == 0)
    {                       
        Delay(resetDelayMs);
		uint16_t val = 0;
        I2C_Write(I2C_ADDR_BanYan_Chip0, 0x67, 0x1E00);   
        I2C_Read(I2C_ADDR_BanYan_Chip0, 0x67, &val);    
        I2C_Write(I2C_ADDR_BanYan_Chip0, 0x67, 0x1E40);   
		if(g_TxCount > 1)
		{
			I2C_Write(I2C_ADDR_BanYan_Chip1, 0x67, 0x1E00);   
			I2C_Read(I2C_ADDR_BanYan_Chip1, 0x67, &val);    
			I2C_Write(I2C_ADDR_BanYan_Chip1, 0x67, 0x1E40);   
		}			
    }
}

void CmdProc_NopConfig(void)
{
    if (enableNopConfig && CmdModeFlag == 0)
    {     
		uint16_t loop = 0;
        Delay(nopConfigStartMs);
		for(loop = 0; loop < nopConfigStartRegisterIndex; loop++)
		{
			int32_t val = nopConfigStartRegister[loop];
			CMD_REG_T *cmdReg = (CMD_REG_T *)(&val);
			I2C_Write(I2C_ADDR_BanYan_Chip0, cmdReg->addr, cmdReg->val); 
			//printf("CmdProc_NopConfig chip0: g_ChannelCount = %d  addr=0x%02x val=0x%02x\r\n", g_ChannelCount, cmdReg->addr, cmdReg->val);
			if(g_TxCount > 1)
			{
				I2C_Write(I2C_ADDR_BanYan_Chip1, cmdReg->addr, cmdReg->val); 
				//printf("CmdProc_NopConfig chip1: addr=0x%02x val=0x%02x\r\n", cmdReg->addr, cmdReg->val);
			}
			
		}
		Delay(nopConfigStopMs - nopConfigStartMs);
		for(loop = 0; loop < nopConfigStopRegisterIndex; loop++)
		{
			int32_t val = nopConfigStopRegister[loop];
			CMD_REG_T *cmdReg = (CMD_REG_T *)(&val);
			I2C_Write(I2C_ADDR_BanYan_Chip0, cmdReg->addr, cmdReg->val); 
			//printf("CmdProc_NopConfig chip0: addr=0x%02x val=0x%02x\r\n", cmdReg->addr, cmdReg->val);
			if(g_TxCount > 1)
			{
				I2C_Write(I2C_ADDR_BanYan_Chip1, cmdReg->addr, cmdReg->val); 
				//printf("CmdProc_NopConfig chip1: addr=0x%02x val=0x%02x\r\n", cmdReg->addr, cmdReg->val);
			}
		}
    }
}


uint16_t FillCmdAck(uint16_t *data, uint16_t dataLen, uint16_t cmdType, uint16_t status)
{
    uint16_t index = 0;

    if (dataLen * sizeof(uint16_t) > (CMD_LEN_MAX - CMD_OVERHEAD_LEN))
    {
        return 0;
    }
    
	memcpy(&CmdAck[index], CmdHead, sizeof(CmdHead));
    index += sizeof(CmdHead);

    if (data == NULL)
    {
        *((uint16_t*)(&CmdAck[index])) = CMD_TYPE_LEN + CMD_STATUS_LEN;
    }
	else if(masterProtocolVer != 0)
    {
        *((uint16_t*)(&CmdAck[index])) = CMD_TYPE_LEN + CMD_STATUS_LEN + (dataLen * sizeof(uint16_t));
    }
    else
    {
        *((uint16_t*)(&CmdAck[index])) = CMD_TYPE_LEN + (dataLen * sizeof(uint16_t));
    }
    index += sizeof(uint16_t);

    *((uint16_t*)(&CmdAck[index])) = cmdType | CMD_ACK_TYPE;
    index += sizeof(uint16_t);

	if(masterProtocolVer != 0 || data == NULL)
	{
		if(masterProtocolVer == 0)
		{
			status = (status == ACK_OK) ? ACK_OK_OLD : ACK_FAIL_OLD;
		}
		*((uint16_t*)(&CmdAck[index])) = status;
		index += sizeof(uint16_t);
	}
	
    if (data != NULL)
    {
        memcpy(&CmdAck[index], data, dataLen*sizeof(uint16_t));
        index += dataLen * sizeof(uint16_t);   
    }

	memcpy(&CmdAck[index], CmdTail, sizeof(CmdTail));
    index += sizeof(CmdTail);
    
	return index;
}

static uint16_t DoAdcReset(CMD_T *cmd, uint32_t cmdLen)
{
    uint16_t ackLen = 0;
    uint16_t status = ACK_OK;

    if (NULL == cmd || 0 == cmdLen)
    {
        return 0;
    }
        
    if (CmdModeFlag == 1)
    {
		enableResetAdc = (uint8_t)cmd->cmdData[0];
		resetDelayMs = cmd->cmdData[1];
#if defined(CONFIG_DEBUG) && defined(STM32_PLATFORM)
        printf("enableResetAdc:%d resetDelayMs:%d\r\n", enableResetAdc, resetDelayMs);
#endif
	} 
    else 
    {
        status = ACK_FAIL;
	}
    
    ackLen = FillCmdAck(NULL, 0, cmd->cmdType, status);

    return ackLen;
}

static uint16_t DoWriteReg(CMD_T *cmd, uint32_t cmdLen)
{
    uint16_t ackLen = 0;
    uint16_t status = ACK_OK;
    uint16_t regNum = 0;
    uint16_t loop = 0;
    uint16_t regVal = 0;
	uint16_t devAddress = I2C_ADDR_BanYan_Chip0;
    CMD_REG_T *cmdReg = NULL;

    if (NULL == cmd || 0 == cmdLen)
    {
        return 0;
    }
        
	if (CmdModeFlag) 
    {
		devAddress = cmd->cmdData[0] << 1;
		regNum = (cmdLen - CMD_TYPE_LEN - CMD_DEV_ADDR_LEN) / sizeof(CMD_REG_T);
		for (loop = 0; loop < regNum; loop++) 
        {
            cmdReg = (CMD_REG_T*)((uint8_t*)(cmd->cmdData) + CMD_DEV_ADDR_LEN + (loop * sizeof(CMD_REG_T)));
            I2C_Write(devAddress, (uint8_t)(cmdReg->addr), cmdReg->val);
			if(devAddress == I2C_ADDR_BanYan_Chip0)
			{//update master register
				Radar_UpdateReg(cmdReg->addr, cmdReg->val);
			}
#ifdef STM32_PLATFORM
			HAL_Delay(10); 
#endif
			
			I2C_Read(devAddress, (uint8_t)(cmdReg->addr), &regVal);
			if (regVal != cmdReg->val) 
            {
				status = ACK_FAIL;
			}
		}
	} 
    else 
    {
		status = ACK_FAIL;
	}
    
    ackLen = FillCmdAck(NULL, 0, cmd->cmdType, status);
    
    return ackLen;
}

static uint16_t DoReadReg(CMD_T *cmd, uint32_t cmdLen)
{
    uint16_t ackLen = 0;
    uint16_t status = ACK_OK;
    uint16_t regNum = 0;
    uint16_t loop = 0;
    uint16_t *readBuf = NULL;
	uint16_t devAddress = I2C_ADDR_BanYan_Chip0;
	
    if (NULL == cmd || 0 == cmdLen)
    {
        return 0;
    }
        
    if (CmdModeFlag) 
    {
		devAddress = cmd->cmdData[0] << 1;
        regNum = (cmdLen - CMD_TYPE_LEN - CMD_DEV_ADDR_LEN) / CMD_REG_ADDR_LEN;
        
        if (regNum > CMD_REG_MAX)
        {
			regNum = 0;
            status = ACK_FAIL;
        }
        else
        {
			uint16_t dataIndex = (masterProtocolVer != 0) ? CMD_DATA_POS : CMD_DATA_POS_OLD;
            readBuf = (uint16_t*)(&CmdAck[dataIndex]);
            for (loop = 0; loop < regNum; loop++) 
            {
				uint16_t regVal = 0;
                I2C_Read(devAddress, (uint8_t)(cmd->cmdData[loop + 1]), &regVal);
				readBuf[loop] = regVal;
            }
        }   
    } 
    else 
    {
        status = ACK_FAIL;
    }
    
    ackLen = FillCmdAck(readBuf, regNum, cmd->cmdType, status);

    return ackLen;
}

__weak int8_t MTT_ParaUpdate(uint16_t type, int32_t val)
{
    return -1;
}

__weak int8_t fftzerofill_ParaUpdate(uint16_t type, int32_t val)
{
    return -1;
}

int8_t NopCofig_ParaUpdate(uint16_t type, int32_t val)
{
    switch (type)  
    {
		case NOPCONFIG_ENABLE:
			enableNopConfig = (int8_t)val;
			nopConfigStartRegisterIndex = 0;
			nopConfigStopRegisterIndex = 0;
			//printf("enablenopreg:%d\r\n", enableNopConfig);
			break;
		case NOPCONFIG_STARTTIME:
			nopConfigStartMs = (uint16_t)val;
			//printf("starttime:%d\r\n", nopConfigStartMs);
			break;
		case NOPCONFIG_STOPTIME:
			nopConfigStopMs = (uint16_t)val;
			//printf("stoptime:%d\r\n", nopConfigStopMs);
			break;
		case NOPCONFIG_STARTREGISTER:
			nopConfigStartRegister[nopConfigStartRegisterIndex++] = val;
			break;
		case NOPCONFIG_STOPREGISTER:
			nopConfigStopRegister[nopConfigStopRegisterIndex++] = val;
			break;        
        default:
            return -1;
    }
    return 0;
}

static uint16_t DoWritePara(CMD_T *cmd, uint32_t cmdLen)
{
    uint16_t ackLen = 0;
    uint16_t status = ACK_OK;
    uint16_t paraNum = 0;
    uint16_t loop = 0;
    CMD_PARA_T *cmdPara = NULL;
    int8_t ret = 0;

    if (NULL == cmd || 0 == cmdLen)
    {
        return 0;
    }
        
	if (CmdModeFlag) 
    {
		paraNum = (cmdLen - CMD_TYPE_LEN) / sizeof(CMD_PARA_T);
		for (loop = 0; loop < paraNum; loop++) 
        {
            cmdPara = (CMD_PARA_T*)((uint8_t*)(cmd->cmdData) + (loop * sizeof(CMD_PARA_T)));
            switch (cmd->cmdType)
            {
                case WRITE_MTT_CMD: 
                    ret = MTT_ParaUpdate(cmdPara->type, cmdPara->val);
                    break;

                case WRITE_SYS_CMD: 
#ifdef SUPPORT_DYNAMIC_SYS_MODE
					if (cmdPara->type == SYS_SYSTEM_MODE &&
						System_GetSysMode() != (int16_t)cmdPara->val)
					{
						Config_ReloadAlgoRegister((int16_t)cmdPara->val);
						Radar_Init();
					}		
#endif				
                    ret = System_ParaUpdate(cmdPara->type, cmdPara->val);
                    break;
                case FFT_ZEROFILL_CMD:
					ret = fftzerofill_ParaUpdate(cmdPara->type, cmdPara->val);
					break;
				case NOP_CONFIG_CMD:
					ret = NopCofig_ParaUpdate(cmdPara->type, cmdPara->val);
					break;
                default:
                    ret = -1;
                    break;
            }
            if (ret)
            {
                status = ACK_FAIL;
            }
			else
			{
				Config_NeedFlashWrite();
			}
		}
	} 
    else 
    {
		status = ACK_FAIL;
	}
    
    ackLen = FillCmdAck(NULL, 0, cmd->cmdType, status);
    
    return ackLen;
}

__weak int32_t MTT_ParaRead(uint16_t type)
{
    return 0x7fffffff; /*invalid value*/
}

static uint16_t DoReadPara(CMD_T *cmd, uint32_t cmdLen)
{
    uint16_t ackLen = 0;
    uint16_t status = ACK_OK;
    uint16_t paraNum = 0;
    uint16_t loop = 0;
    uint32_t *readBuf = NULL;

    if (NULL == cmd || 0 == cmdLen)
    {
        return 0;
    }
        
    if (CmdModeFlag) 
    {
        paraNum = (cmdLen - CMD_TYPE_LEN) / CMD_PARA_NAME_LEN;
        
        if (paraNum > CMD_PARA_MAX)
        {
			paraNum = 0;
            status = ACK_FAIL;
        }
        else
        {
			uint16_t dataIndex = (masterProtocolVer != 0) ? CMD_DATA_POS : CMD_DATA_POS_OLD;
            readBuf = (uint32_t*)(&CmdAck[dataIndex]);
            for (loop = 0; loop < paraNum; loop++) 
            {
                switch (cmd->cmdType)
                {
                    case READ_MTT_CMD:
                        readBuf[loop] = MTT_ParaRead(cmd->cmdData[loop]);
                        break;
                    
                    case READ_SYS_CMD:
                        readBuf[loop] = System_ParaRead(cmd->cmdData[loop]);
                        break;
                    
                    default:
                        break;
                }
            }
        }   
    } 
    else 
    {
        status = ACK_FAIL;
    }
    
    ackLen = FillCmdAck((uint16_t*)readBuf, paraNum*2, cmd->cmdType, status);
    
    return ackLen;
}

static uint16_t DoReadSn(CMD_T *cmd)
{
    uint16_t ackLen = 0;
    uint16_t status = ACK_OK;
    uint16_t sn[SN_LEN] = {0};

    if (NULL == cmd)
    {
        return 0;
    }
        
    if (CmdModeFlag) 
    {   
        sn[0] = RADAR_DEV_MODEL_TYPE_SN;
        *(uint32_t*)(&sn[1]) = Config_GetSN();      
    } 
    else 
    {
        status = ACK_FAIL;
    }
    
    ackLen = FillCmdAck(sn, sizeof(sn), cmd->cmdType, status);
    return ackLen;
}

static uint16_t DoCascadingMode(CMD_T *cmd, uint32_t cmdLen)
{
    uint16_t ackLen = 0;
    uint16_t status = ACK_OK;

    if (NULL == cmd || 0 == cmdLen)
    {
        return 0;
    }
#ifdef STM32_PLATFORM      
    if (CmdModeFlag == 1)
    {
		//uint8_t cascadingMode = (uint8_t)cmd->cmdData[0];
		uint8_t tRMode = cmd->cmdData[1]; 
		uint8_t tempCount = MIN(tRMode, g_maxChannelCount);
		g_ChannelCount = tempCount;
		g_TxCount = (g_ChannelCount + 1) / 2;
		if((cmdLen - CMD_TYPE_LEN) == 6)
		{
			g_TxCount = cmd->cmdData[2];
		}
	} 
    else 
    {
        status = ACK_FAIL;
	}
#endif  
    ackLen = FillCmdAck(NULL, 0, cmd->cmdType, status);

    return ackLen;
}



static void CmdExec(CMD_T *cmd, uint32_t cmdLen)
{
    uint16_t ackLen = 0;
    uint16_t devVersion[] = {RADAR_DEV_TYPE, RADAR_DEV_FIRMWARE_TYPE, RADAR_DEV_VER_MAJAR, RADAR_DEV_VER_MINOR, RADAR_DEV_VER_PATCH};
	uint16_t boardInfomation[] = {RADAR_PROTOCOL_VERSION, CMD_LEN_MAX};
//#ifdef STM32_PLATFORM
//    USB_DATA_T usbDataQ = {0};
//#endif

    if (NULL == cmd || 0 == cmdLen)
    {
        return;
    }

	switch(cmd->cmdType)
    {
		/* Enable debug mode, Send all rangebin mag to PC */
    case ENABLE_DEBUG_CMD:
        if(1 == CmdModeFlag)
        {
            DebugModeFlag = 1;
            ackLen = FillCmdAck(NULL, 0, cmd->cmdType, ACK_OK);
        }
        break;
    
	  /* Disable debug mode, only send target information to PC */
    case DISABLE_DEBUG_CMD:
        if(1 == CmdModeFlag)
        {
            ackLen = FillCmdAck(NULL, 0, cmd->cmdType, ACK_OK);
            DebugModeFlag = 0;
        }
        break;
				
		case START_CFG_CMD:
			CmdModeFlag = 1;
			if(cmdLen > CMD_TYPE_LEN)
			{
				masterProtocolVer = cmd->cmdData[0];
				ackLen = FillCmdAck(boardInfomation, ARRAY_SIZE(boardInfomation), cmd->cmdType, ACK_OK);
			}
			else
			{
				masterProtocolVer = 0;
				ackLen = FillCmdAck(NULL, 0, cmd->cmdType, ACK_OK);
			}
		    break;
        
		case FINISH_CFG_CMD:
            Config_SavePara2Flash();
            System_Reconfig();
			      ackLen = FillCmdAck(NULL, 0, cmd->cmdType, ACK_OK);
            CmdModeFlag = 0;
		    break;

        case ADC_RESET_CMD:         
            ackLen = DoAdcReset(cmd, cmdLen);
            break;
            
		case WRITE_REG_CMD:
            ackLen = DoWriteReg(cmd, cmdLen);
		    break;
            
		case READ_REG_CMD:
            ackLen = DoReadReg(cmd, cmdLen);
		    break;
            
		case READ_VER_CMD:
			if(masterProtocolVer != 0)
			{
				ackLen = FillCmdAck(devVersion, ARRAY_SIZE(devVersion), cmd->cmdType, ACK_OK);
			}
			else
			{
				ackLen = FillCmdAck(&devVersion[2], ARRAY_SIZE(devVersion) - 2 , cmd->cmdType, ACK_OK);
			}
            
            break;
		
    /* config para */
    case PARAM_CFG_CMD: 			
    case THRESHOLD_SET_CMD:
			 ackLen = BodySensing_ParaUpdate(cmd, cmdLen, CmdModeFlag);
       break;
		/* Read all para */
    case PARAM_READ_CMD: 
       ackLen = DoReadBodySensingPara(cmd, cmdLen, CmdModeFlag);
       break;
        
		case WRITE_MTT_CMD:
    case WRITE_SYS_CMD:
		case FFT_ZEROFILL_CMD:
		case NOP_CONFIG_CMD:
            ackLen = DoWritePara(cmd, cmdLen);
		    break;

        case READ_MTT_CMD:
        case READ_SYS_CMD:
            ackLen = DoReadPara(cmd, cmdLen);
		    break;
        
        case READ_SN_CMD:
            ackLen = DoReadSn(cmd);
		    break;
		
		case CASCADING_MODE_CMD:
			ackLen = DoCascadingMode(cmd, cmdLen);
            break;
		
		default:
		    break;
	}
	
    if (ackLen > 0)
    {
#if defined(CONFIG_DEBUG) && defined(STM32_PLATFORM)
        printf("ackLen:%d\r\n", ackLen);
        for (int i = 0; i < ackLen; i++)
        {
            printf("0x%02x ", CmdAck[i]);
        }
        printf("\r\n");
#endif

//#ifdef STM32_PLATFORM
//        usbDataQ.buf = CmdAck;
//        usbDataQ.len = ackLen;
//        UsbTransfer_Send2UsbDataQueue(&usbDataQ);
//#else
        //UART0_DmaSend(CmdAck, ackLen);
			
			  CDC_Transmit_HS(CmdAck, ackLen);  
//#endif
    }
}


uint8_t CmdProc_IsInDebugMode(void)
{
    return DebugModeFlag;
}

uint8_t CmdProc_InCmdMode(void)
{
    return CmdModeFlag;
}

void CmdProc_InCmdModeClear(void)
{
    CmdModeFlag = 0;
}

static void CmdProcess(uint8_t* buf, uint16_t len)
{
    uint16_t loop = 0;

    if (NULL == buf || 0 == len)
    {
        return;
    }
    
#if defined(CONFIG_DEBUG) && defined(STM32_PLATFORM)
    printf("recvLen:%d\r\n", len);
    for (int i = 0; i < len; i++)
    {
        printf("0x%02x ", buf[i]);
    }
    printf("\r\n");
#endif

	for (loop = 0; loop < len; loop++) 
    {
		switch(CmdDataParse.state)
		{                    
			case CMD_STATE_HEAD0:
				if (buf[loop] == CmdHead[CMD_HEAD_0]) 
                {
                    CmdDataParse.curIndex = 0;
					CmdDataParse.state = CMD_STATE_HEAD1;
				}
				break;
                
			case CMD_STATE_HEAD1:
				if (buf[loop] == CmdHead[CMD_HEAD_1])
                {
                    CmdDataParse.state = CMD_STATE_HEAD2;
				}
                else
                {
                    CmdDataParse.state = CMD_STATE_HEAD0;
                }
				break;

            case CMD_STATE_HEAD2:
				if (buf[loop] == CmdHead[CMD_HEAD_2])
                {
                    CmdDataParse.state = CMD_STATE_HEAD3;
				}
                else
                {
                    CmdDataParse.state = CMD_STATE_HEAD0;
                }
				break;

            case CMD_STATE_HEAD3:
				if (buf[loop] == CmdHead[CMD_HEAD_3])
                {
                    CmdDataParse.state = CMD_STATE_LEN0;
				}
                else
                {
                    CmdDataParse.state = CMD_STATE_HEAD0;
                }
				break;

            case CMD_STATE_LEN0:
                CmdDataParse.len = buf[loop];
                CmdDataParse.state = CMD_STATE_LEN1;
				break;

            case CMD_STATE_LEN1:
                CmdDataParse.len += buf[loop] << CMD_LEN_HIGH_POS;
                if (CmdDataParse.len <= CMD_BUF_LEN)
                {
                    CmdDataParse.state = CMD_STATE_DATA;
                }
                else
                {
                    CmdDataParse.state = CMD_STATE_HEAD0;
                }
				break;
                
            case CMD_STATE_DATA:
                CmdDataParse.buf[CmdDataParse.curIndex++] = buf[loop];
				if (CmdDataParse.curIndex == CmdDataParse.len)
                {
					CmdDataParse.state = CMD_STATE_TAIL0;
				}
				break;
                
            case CMD_STATE_TAIL0:
				if (buf[loop] == CmdTail[CMD_TAIL_0])
                {
                    CmdDataParse.state = CMD_STATE_TAIL1;
				}
                else
                {
                    CmdDataParse.state = CMD_STATE_HEAD0;
                }
				break;

            case CMD_STATE_TAIL1:
				if (buf[loop] == CmdTail[CMD_TAIL_1])
                {
                    CmdDataParse.state = CMD_STATE_TAIL2;
				}
                else
                {
                    CmdDataParse.state = CMD_STATE_HEAD0;
                }
				break;
                
            case CMD_STATE_TAIL2:
				if (buf[loop] == CmdTail[CMD_TAIL_2])
                {
                    CmdDataParse.state = CMD_STATE_TAIL3;
				}
                else
                {
                    CmdDataParse.state = CMD_STATE_HEAD0;
                }
				break;

            case CMD_STATE_TAIL3:
				if (buf[loop] == CmdTail[CMD_TAIL_3])
                {
                    CmdExec((CMD_T*)(CmdDataParse.buf), CmdDataParse.len);   
				}
                CmdDataParse.state = CMD_STATE_HEAD0;
				break; 
			
			default:
				CmdDataParse.state = CMD_STATE_HEAD0;
				break;

		}
	}

}


void CmdProc_Init(void)
{
    memset(&g_cmdRecv, 0, sizeof(g_cmdRecv));
	
	  BodySensing_ParaInit();
}


void CmdProc_Recv(void)
{       
    if (!g_cmdRecv.cmdReady)
    {
        return;
    }

    CmdProcess(g_cmdRecv.buf[g_cmdRecv.bufProc], g_cmdRecv.bufLen);

    g_cmdRecv.cmdReady = 0;
}
