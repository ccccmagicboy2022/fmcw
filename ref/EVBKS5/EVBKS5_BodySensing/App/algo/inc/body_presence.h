#ifndef BODY_PRESENCE_H
#define BODY_PRESENCE_H

#include "bodysensing_types.h"
#include "bodysensing_dsp.h"
#include "stdio.h"
#include "math.h"
#include "arm_math.h"
#include "arm_const_structs.h"
#include "cmdprocess.h"
/********************************************************************************/
/* for data report */
#define PROTOCOL_HEAD                    (0XF1F2F3F4)
#define PROTOCOL_TAIL                    (0XF5F6F7F8)
#define REPORT_DEBUG_TYPE                (0x01)
#define REPORT_TARGET_TYPE               (0x02)

#define DATA_HEAD                        (0xAA)
#define DATA_TAIL                        (0x55)

#define REPORT_HEAD_LENGTH               (4)
#define DATA_LEN_LENGTH                  (2)
#define DATA_TYPE_LENGTH                 (1)
#define DATA_HEAD_LENGTH                 (1)
#define TARGET_DATA_LENGTH               (7)
#define MAX_MOTION_BINS_LENGTH           (1)
#define MAX_MOTIONLESS_BINS_LENGTH       (1)
#define MOTION_DATA_RANGE_BIN_LENGTH     (ONEFFT_POINT - 1)
#define MOTIONLESS_DATA_RANGE_BIN_LENGTH (ONEFFT_POINT - 1)
#define DATA_TAIL_LENGTH                 (1)
#define DATA_CHECK_LENGTH                (1)
#define REPORT_TAIL_LENGTH               (4)

#define BODYSENSING_START_ADDR           ADDR_FLASH_SECTOR_5   

#define REPORT_LENGTH    (REPORT_HEAD_LENGTH +  \
                          DATA_LEN_LENGTH + \
                          DATA_TYPE_LENGTH + \
                          DATA_HEAD_LENGTH + \
                          TARGET_DATA_LENGTH + \
                          MAX_MOTION_BINS_LENGTH + \
                          MAX_MOTIONLESS_BINS_LENGTH + \
                          MOTION_DATA_RANGE_BIN_LENGTH + \
                          MOTIONLESS_DATA_RANGE_BIN_LENGTH + \
                          DATA_TAIL_LENGTH + \
                          DATA_CHECK_LENGTH+REPORT_TAIL_LENGTH)
/********************************************************************************/

typedef enum FlagStatus 
{
    STOP    =   0,
    START   =   1,
    MAX_FLAGSTATUS
}T_FlagStatus;

typedef enum DETECTSTATUS
{
    NOBODY = 0,
    BODYMOTION = 1,
    BODYMOTIONLESS = 2,
    BOTH_STATUS = 3,
    ENERGYSATURATION = 4
}DetStatus;

typedef struct RESULT
{
    DetStatus status;
    uint16_t Motion_Distance;
    uint8_t Motion_MaxVal;
    uint16_t MotionLess_Distance;
    uint8_t MotionLess_MaxVal;
    uint16_t offtime;
    uint8_t bNeedReport;
}Result;

#pragma pack(4)
typedef struct ALGORITHM_PARAM 
{
    uint32_t nMaxMotionRangeBin;
    uint32_t nMaxMotionLessRangeBin;
    uint32_t nThresholdValOfMotion[ONEFFT_POINT - 1];
    uint32_t nThresholdValOfMotionLess[ONEFFT_POINT - 1];
    uint32_t nOffTime;
}ALGORITHM_PARAM_T;
#pragma pack()

extern ALGORITHM_PARAM_T gAlgorithmParam;


Result BodyPresence(uint8_t* dataBuf, uint16_t index, uint8_t motion_max, uint8_t motionless_max, 
                    uint32_t *pMotion_sensitivity,uint32_t* pMotionless_sensitivity,
                    uint32_t off_time);

void RegisterCallback(uint8_t (*debugfunc)(uint8_t* , uint16_t ));
void EnableDebugMode(void);
void DisableDebugMode(void);
uint8_t IsInDebugMode(void);


//Algorithm param
void BodySensing_ParaInit(void);
uint16_t BodySensing_ParaUpdate(CMD_T *cmd, uint32_t cmdLen, uint8_t CmdModeFlag);
uint16_t DoReadBodySensingPara(CMD_T *cmd, uint32_t cmdLen, uint8_t CmdModeFlag);
#endif
