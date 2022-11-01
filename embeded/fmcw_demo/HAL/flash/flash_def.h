#ifndef  __FLASH_DEF_H
#define __FLASH_DEF_H
#include "sys.h"

#define     FLASH_PAGE_SIZE         ((uint16_t)0x800u)  //2k
#define     FLASH_SECTOR61_ADDR     ((uint32_t)0x0007A000u)
#define     FLASH_SECTOR62_ADDR     ((uint32_t)0x0807C000u)

#define     RADAR_RANGE_FIRST       (1u)
#define     RADAR_RANGE_SECOND      (2u)
#define     RADAR_RANGE_THIRD       (3u)
#define     RADAR_RANGE_FOURTH      (4u)

#define     RADAR_ENTRANCE_FIRST    (1u)
#define     RADAR_ENTRANCE_SECOND   (2u)
#define     RADAR_ENTRANCE_THIRD    (3u)
#define     RADAR_ENTRANCE_FOURTH   (4u)
#define     RADAR_ENTRANCE_FIFTH    (5u)
#define     RADAR_ENTRANCE_SIXTH    (6u)

#if defined(ZHIXIN) || defined(GONGNIU)
#define     DELAY_TIME              (12U)
#else
#define     DELAY_TIME              (64U)
#endif
#define     LIGHE_HOLD              (4095)
#define     FLASH_READ_WORD_BLOCK   (20)

#define     num_sub_A               50   // The maximum allowable error is 200
#define     num_sub_N               20   // Take the mean of the 20 numbers

#define     FLASH_ADDR_OFFSET_CRC   (1u)
#define     FLASH_LENGTH_CRC_MAGIC  (8U)
#define     MAGIC_NUM               (0x5AA5A55A)

// FLASH related parameters combined variables
typedef struct {
    uint32_t magic_num;
    uint32_t crc;
    uint32_t delay_time;
    uint32_t upload_duty;
    uint16_t light_threshold;
    uint8_t entrance_range;
    uint8_t presence_range;
    uint8_t status_upload_enable;
    uint8_t overthreshold_upload_enable;
    uint8_t led_enable;
    uint8_t respiratory_upload_enable;
    uint8_t heartbeat_upload_enable;
} prensence_info_t;

extern void Flash_Write(uint8_t u8Len, prensence_info_t *current_detect_param);
extern void Flash_SectorErase(void);

// Report related parameter structure variables
extern tramp paratramp;

typedef union {
    float floatdate;
    unsigned long data;
} check_result;


typedef union {
    int lightnum;
    unsigned long data;
} light_check;

typedef struct {
    float time_times;
    float time_add;
    float freq_times;
} entrance_param_t;

typedef struct {
    float time_times;
    float time_add;
    float freq_times;
    float respiration_times;
    float offset_max;
    float offset_min;
} presence_param_t;

extern check_result  DateType1;
extern check_result  DateType2;
extern check_result  DateType3;
extern light_check   DateType4;

void respiration_adaptive_param_upload(void);
void respiration_adaptive_radar_range(void);
void sampling_status_upload(void);
void calc_data_upload(void);
void product_test_status_upload(void);
// extern void para_upload_init(void);
extern void LightData_TaskHandle(TESK_ID_TYPEDEF Task_ID, Task_TYPEDEF Task_State);
void set_entrance_range(int range);
extern void set_radar_range(int range);
extern void Flash_Init_62(void);
extern void FFT_analysis(s16 *analysis_data, const float *win, int time_data_size, float noisemean);
#endif
