#ifndef _SYS_H
#define _SYS_H

#include "stm32f4xx.h"
#include "arm_math.h"
#include "bluetooth.h"
#include "ringbuffer.h"
#include "memalloc.h"

#include "mcu429_hal.h"
#include "uart_hal.h"
#include "radar_hal.h"
#include "usb_hs_hal.h"
#include "fpga_hal.h"

#include "SEGGER_RTT.h"
#include "sysclk.h"
#include "adc.h"

#include "usb_core.h"
#include "usb_conf.h"
#include "usbd_conf.h"
#include "usbd_cdc_core.h"
#include "usbd_usr.h"
#include "usbd_desc.h"
#include "usb_bsp.h"
#include "usbd_cdc_vcp.h"

#include "presence_detection_fmcw.h"

#define SEGGER_RTT_IN_RAM   1

///////////////////////////
#define FREQ_MIN 24025  // MHz
#define FREQ_MAX 24225  // MHz
#define FREQ_OUT_DIV         (65536 * 16)

#ifndef QUICK_CALC_FREQ
    #define FREQ_CAP_MEAN_NUM   150
    #define ERR_LIMIT           (0.0015f)
    #define KP                  (-1.56f)
    #define KI                  (-0.03f)
    #define KD                  (0.6f)
    #define OK_TIMER_NUM        2
#else
    #define FREQ_CAP_MEAN_NUM   80
    #define ERR_LIMIT           (0.005f)
    #define KP                  (-1.56f)
    #define KI                  (-0.03f)
    #define KD                  (0.6f)
    #define OK_TIMER_NUM        1
#endif

#define FREQ_CHECK_MEAN_NUM  200
#define SAMPLE_NUM_PER_CHIRP 128
#define NUM_CHIRPS_PER_FRAME  16
#define DAC_WORK_RESOLUTION  128
#define CHIRP_WORK_TIME      400    // us
#define CHIRP_IDLE_TIME      200    // us
#define FRAME_TIME_D           (40 * 1000)  // us

#define DAC_IDEL_RESOLUTION  (DAC_WORK_RESOLUTION * CHIRP_IDLE_TIME / CHIRP_WORK_TIME)
#define DAC_ALL_RESOLUTION   (DAC_WORK_RESOLUTION + DAC_IDEL_RESOLUTION)
///////////////////////////
typedef struct{
    arm_pid_instance_f32  S;
    float                 out;
}PidCtrlTypedef;

#define CV_LOG(fmt, ...) \
do \
{ \
        SEGGER_RTT_printf(0, fmt, ##__VA_ARGS__); \
}while(0)

// Report related parameters
typedef struct
{
    volatile uint8_t check_final;                    // is there anybody  0,No  1,BIG0/TINY0  2,TINY1  3,BIG1
    volatile uint8_t instant_start;                  // Timely detection
    volatile uint8_t find_me_flag;                   // Find equipment
    volatile uint8_t check_status;                   // Detection status reporting
    volatile uint8_t light_enable_flag;              // Control O output flag
    volatile uint8_t light_status_flag;              // Lamp status report
    volatile uint8_t g_work_mode;                    // Working mode reporting
    volatile uint8_t sampling_status;                // Normal mode Test mode switchover
    volatile uint8_t test_result_upload;             // Querying test data
    volatile uint8_t production_and_test;            //
    volatile uint16_t m_light_min;                   // Minimum photosensitive value
    volatile uint16_t light_adc_data;                // Real-time light sensitivity
    volatile uint16_t light_adc_temp;                // Real-time light sensitivity
    volatile uint8_t instant_response_upload_enable;
    volatile uint8_t trigger_breakin_calc_once;
    volatile uint8_t trigger_exist_calc_once;
} tramp;

enum person_check_result {  // Detection of human state enumeration
    NO_PERSON_STATUS = 0,
    PERSON_BIG0_TINY0 = 1,
    PERSON_TINY1 = 2,
    PERSON_BIG1 = 3,
};

enum work_mode {          // work pattern
    NORMAL = 0,           // default
    FAST_CHECK_ONLY,      // Check fast detection only
    SLOW_CHECK_ONLY,      // Check for slow detection only
    MODE_OTHER
};

enum check_status
{
    FAST_CHECK=0,
    SLOW_CHECK=1,
    INIT=2,
    OTHER=3
};

enum light_mode_status {
    LIGHT_OFF = 0,
    LIGHT_ON  = 1,
};

extern void arm_absmax_q31(
  const q31_t * pSrc,
        uint32_t blockSize,
        q31_t * pResult,
        uint32_t * pIndex);

#endif
