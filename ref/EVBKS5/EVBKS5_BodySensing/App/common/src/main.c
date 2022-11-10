/*********************************************************************************************************************
 * COPYRIGHT NOTICE
 * Copyright (c) 2022, 矽典微
 * All rights reserved.
 *
 * 本固件包所有内容版权均属矽典微所有
 * @file           main
 * @company        矽典微
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
#include "utilities.h"


int main(void)
{   
    Platform_Init();
	  SOC_Enable();
	  delay_ms(8);
    Config_Init();
    Radar_Init();

    DataProc_Init(); 
    CmdProc_Init();
	
	  
    while (1)
		{
			  DataProc_Recv();
			  CmdProc_Recv();
		}
}
