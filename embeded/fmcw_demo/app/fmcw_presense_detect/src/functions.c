#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "arm_math.h"
#include "functions.h"
#include "memalloc.h"

#define ROW_LEN 128
#define COL_LEN 16

#define FFT_ROW_NUM 256
#define FFT_COL_NUM 64

void td_fft(const uint16_t frame_data[COL_LEN][ROW_LEN], float *rfft_tar_sum, float v_fft_tar_abs[R_NUM][V_NUM])
{
    int i, j;
    float *fft_row_in, *fft_row_out, *fft_col;
    float (*r_fft_tar)[R_NUM * 2];
    float mean_f, temp_f;

    arm_rfft_fast_instance_f32 RFFT_S_256;
    arm_rfft_fast_init_f32(&RFFT_S_256, FFT_ROW_NUM);
    arm_cfft_instance_f32 CFFT_S_64;
    arm_cfft_init_f32(&CFFT_S_64, FFT_COL_NUM);

    r_fft_tar = alloc_mem(COL_LEN * R_NUM * 2 * sizeof(float));
    fft_row_in = alloc_mem(FFT_ROW_NUM * sizeof(float));
    fft_row_out = alloc_mem(FFT_ROW_NUM * sizeof(float));

    for (i = 0; i < COL_LEN; i++) {
        for (j = 0; j < ROW_LEN; j++) {
            fft_row_in[j] = frame_data[i][j];
        }
        for (j = ROW_LEN; j < FFT_ROW_NUM; j++) {
            fft_row_in[j] = 0;
        }
        arm_rfft_fast_f32(&RFFT_S_256, fft_row_in, fft_row_out, 0);

        for (j = 0; j < R_NUM * 2; j++) {
            r_fft_tar[i][j] = fft_row_out[R_MIN_INDEX * 2 + j] / FFT_ROW_NUM;
        }
    }

    free_mem(fft_row_out);
    free_mem(fft_row_in);

    memset(rfft_tar_sum, 0, R_NUM * 2 * sizeof(float));
    for (i = 0; i < COL_LEN; i++) {
        arm_add_f32(rfft_tar_sum, r_fft_tar[i], rfft_tar_sum, R_NUM * 2);
    }

    for (i = 0; i < R_NUM * 2; i++) {
        mean_f = rfft_tar_sum[i] / COL_LEN;
        for (j = 0; j < COL_LEN; j++) {
            r_fft_tar[j][i] -= mean_f;
        }
    }

    fft_col = alloc_mem(FFT_COL_NUM * 2 * sizeof(float));

    for (i = 0; i < R_NUM; i++) {
        for (j = 0; j < COL_LEN; j++) {
            fft_col[2 * j] = r_fft_tar[j][2 * i];
            fft_col[2 * j + 1] = r_fft_tar[j][2 * i + 1];
        }
        for (j = COL_LEN * 2; j < FFT_COL_NUM * 2; j++) {
            fft_col[j] = 0;
        }

        arm_cfft_f32(&CFFT_S_64, fft_col, 0, 1);
        for (j = 0; j < FFT_COL_NUM; j++) {
            temp_f = fft_col[j] / FFT_COL_NUM;
            fft_col[j] = fft_col[j + FFT_COL_NUM] / FFT_COL_NUM;
            fft_col[j + FFT_COL_NUM] = temp_f;
        }
        arm_cmplx_mag_f32(&fft_col[V_MIN_INDEX * 2], v_fft_tar_abs[i], V_NUM);
    }

    free_mem(fft_col);
    free_mem(r_fft_tar);
}

#define DIFF_ENERGY_LINE 0.5f
#define NOISE_COEFFICIENT 1.0f
#define BACKGROUND_UPDATE_WEIGHT 0.1f

void background_update(float data[TRACKING_WIN_NUM][R_NUM][V_NUM], float background_data_matrix[R_NUM][V_NUM], float difference_matrix[R_NUM][V_NUM])
{
    int i, j, k;
    float temp, diff;

    for (i = 0; i < R_NUM; i++) {
        for (j = 0; j < V_NUM; j++) {
            temp = 0;
            for (k = 0; k < TRACKING_WIN_NUM; k++) {
                temp += data[k][i][j];
            }
            temp = temp / TRACKING_WIN_NUM;
            difference_matrix[i][j] = temp - background_data_matrix[i][j];
            if (difference_matrix[i][j] < background_data_matrix[i][j] * NOISE_COEFFICIENT && \
                difference_matrix[i][j] > -background_data_matrix[i][j] * NOISE_COEFFICIENT) {
                background_data_matrix[i][j] = background_data_matrix[i][j] * (1 - BACKGROUND_UPDATE_WEIGHT) \
                                               + temp * BACKGROUND_UPDATE_WEIGHT;
            }
        }
    }
}

int target_tracking(float difference_matrix[R_NUM][V_NUM], float *range, float *velocity)
{
    int ret;
    int i, j;
    float max_val;
    int v_max_val_index, r_max_val_index;

    max_val = difference_matrix[0][0];
    r_max_val_index = 0;
    v_max_val_index = 0;

    for (i = 0; i < R_NUM; i++) {
        for (j = 0; j < V_NUM; j++) {
            if (difference_matrix[i][j] > max_val) {
                max_val = difference_matrix[i][j];
                r_max_val_index = i;
                v_max_val_index = j;
            }
        }
    }

    if (max_val > DIFF_ENERGY_LINE) {
        *range = 0.3750 + 0.3750 * r_max_val_index;
        *velocity = -1.9511 + 0.1626 * v_max_val_index;
        ret = 1;
    } else {
        *range = 0;
        *velocity = 0;
        ret = 0;
    }

    return ret;
}

#define FS_RESPIRATORY          25.2525
#define RESPIRATION_FFT_NUM     128
#define FREQ_LIMIT_LOW          0.1f
#define FREQ_LIMIT_HIGH         3.0f
#define FREQ_LIMIT_LOW_NUM      ((int)(FREQ_LIMIT_LOW / (FS_RESPIRATORY / RESPIRATION_FFT_NUM)))
#define FREQ_LIMIT_HIGH_NUM     ((int)(FREQ_LIMIT_HIGH / (FS_RESPIRATORY / RESPIRATION_FFT_NUM)))
#define FREQ_LIMIT_NUM          (FREQ_LIMIT_HIGH_NUM - FREQ_LIMIT_LOW_NUM + 1)


int respiration_detection(float data[RESPIRATION_PROCESS_NUM][R_NUM * 2], int target_index[])
{
    int i = 0, j = 0, target_index_len = 0;

    float lowfreq_amp = 0;
    float *fft_data;

    fft_data = alloc_mem(RESPIRATION_FFT_NUM * 2 * sizeof(float));

    arm_cfft_instance_f32 CFFT_S_128;
    arm_cfft_init_f32(&CFFT_S_128, RESPIRATION_FFT_NUM);

    for (i = 0; i < R_NUM; i++) {
        for (j = 0; j < RESPIRATION_PROCESS_NUM; j++) {
            fft_data[2 * j] = data[j][2 * i];
            fft_data[2 * j + 1] = data[j][2 * i + 1];
        }
        for (j = RESPIRATION_PROCESS_NUM * 2; j < RESPIRATION_FFT_NUM * 2; j++) {
            fft_data[j] = 0;
        }

        arm_cfft_f32(&CFFT_S_128, fft_data, 0, 1);
        fft_data[0] = 0;
        fft_data[1] = 0;
        fft_data[2] = 0;
        fft_data[3] = 0;
        arm_cmplx_mag_f32(&fft_data[FREQ_LIMIT_LOW_NUM * 2], fft_data, FREQ_LIMIT_NUM);

        for (j = 0; j < FREQ_LIMIT_NUM; j++) {
            lowfreq_amp += fft_data[j];
        }
        lowfreq_amp /= RESPIRATION_FFT_NUM;
        if (lowfreq_amp > BACKGROUND_LINE) {
            target_index[i] = 1;
            target_index_len++;
        } else {
            target_index[i] = 0;
        }
    }

    free_mem(fft_data);
    return target_index_len;
}
