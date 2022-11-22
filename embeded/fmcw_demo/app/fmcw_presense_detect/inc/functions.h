#ifndef  __FUNCTIONS_H
#define __FUNCTIONS_H

#include "sys.h"
#include "fmcw_param.h"

#define ROW_LEN                         128
#define COL_LEN                         16


void td_fft(const uint16_t frame_data[COL_LEN][ROW_LEN], float *rfft_tar_sum, float v_fft_tar_abs[R_NUM][V_NUM]);
void background_update(float data[TRACKING_WIN_NUM][R_NUM][V_NUM], float background_data_matrix[R_NUM][V_NUM], float difference_matrix[R_NUM][V_NUM]);
int target_tracking(float difference_matrix[R_NUM][V_NUM], float *range, float *velocity);
int respiration_detection(float data[RESPIRATION_PROCESS_NUM][R_NUM * 2], int target_index[]);
#endif
