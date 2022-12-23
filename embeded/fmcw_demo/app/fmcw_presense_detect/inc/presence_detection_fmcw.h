#ifndef __PRESENCE_DETECTION_FMCW_H__
#define __PRESENCE_DETECTION_FMCW_H__

#include "sys.h"

typedef enum
{
    NO_PERSON = 0,
    PERSON,
} output_result_t_fmcw;

typedef struct {
    uint32_t delay_times;
    float range_line;
    float background_line[16];
    float diff_energy_line[16];
} detect_param_t;

int inquire_tracking_check_status(void);
int inquire_tracking_check_finished(void);
int inquire_respiration_check_status(void);
int inquire_respiration_check_finished(void);

void set_tracking_check(u8 enable);
void set_respiration_check(u8 enable);
void set_delay_time(int delay_time);
void set_range_line(int range_line);
void set_background_line(detect_param_t *current_detect_param);
void set_diff_energy_line(detect_param_t *current_detect_param);
void get_detect_param(detect_param_t *current_detect_param);

int preprocess_fmcw(s16 *data);
void detect(s16 *data, output_result_t_fmcw *output_result);
void detect_presence(void);
void detect_respiration(void);
s16 get_max(int data[RESPIRATION_TOTAL_TIMES][CHIRPS_PER_FRAME]);
#endif
