/**
  ******************************************************************************
  * @file    global_conf.h
  * @author  iclm team
  * @brief   global config for whole project
  ******************************************************************************
  */
#ifndef __GLOBAL_CONF_H__
#define __GLOBAL_CONF_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include "global_def.h"

/**********function switch*************/
#ifdef EVBKS5
#define SUPPORT_DYNAMIC_SYS_MODE
#define SYS_MODE_DEFAULT          SYS_MODE_PASSTHROUGH
#define SUPPORT_DATA_PASSTHROUGH
#endif


/**********para config****************/
#ifdef EVBKS5
#define UPLOAD_SAMPLE_RATE        (1)
#define RADAR_DATA_MAX_LEN        (4096)
#endif


#define DEBUG_MODE_DEFAULT        DEBUG_MODE_OFF

/**********debug**********************/
//#define CONFIG_DEBUG

#ifdef __cplusplus
}
#endif

#endif


