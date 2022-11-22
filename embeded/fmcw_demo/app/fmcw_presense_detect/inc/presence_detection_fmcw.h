#ifndef __PRESENCE_DETECTION_FMCW_H__
#define __PRESENCE_DETECTION_FMCW_H__

#include "sys.h"
#include "fmcw_param.h"
#include "memalloc.h"

typedef enum
{
    //    NO_PERSON = 0,
    NO = 0,
    PERSON,
} output_result_t_fmcw;

int preprocess_fmcw(s16 *data);
void detect(s16 *data, output_result_t_fmcw *output_result);
void detect_presence(void);
void detect_respiration(void);
s16 get_max(int data[RESPIRATION_TOTAL_TIMES][CHIRPS_PER_FRAME]);

#endif
