/*********************************************************************************************************************
 * COPYRIGHT NOTICE
 * Copyright (c) 2022, ����΢
 * All rights reserved.
 *
 * ���̼����������ݰ�Ȩ��������΢����
 * @file           main
 * @company        ����΢
 * @author         RSS
 * @version        v1.0.0
 * @Target core    S5KM312CL, STM32F429
 * @date           2022-03-02
 ******************************************************************
 */
#include <stdio.h>
#include "global_conf.h"
#include "platform.h"
#include "banyan.h"
#include "dataprocess.h"
#include "cmdprocess.h"
#include "config.h"
#ifdef STM32_PLATFORM
#include "rtos.h"
#include "cmsis_os.h"
#endif

int main(void)
{   
    Platform_Init();
	  delay_ms(8);
    Config_Init();
    Radar_Init();

#ifdef STM32_PLATFORM
    RTOS_Init();
#endif

    DataProc_Init(); 
    CmdProc_Init();
    //printf("system init finished!\r\n");
    
    osKernelStart();
    while (1);
}

