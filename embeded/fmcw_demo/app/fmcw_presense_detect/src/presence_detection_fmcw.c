#include <stdint.h>
#include <string.h>
#include "fmcw_param.h"
#include "sys.h"
#include "memalloc.h"
#include "arm_math.h"
#include "presence_detection_fmcw.h"
#include "functions.h"

int frame_index = 0, respiration_judge_index = 0;
float respiration_data_matrix[ARRAY_LENGTH_MAX][ARRAY_LENGTH_THIRTY_TWO];
float background_data_matrix[ARRAY_LENGTH_SIXTEEN][ARRAY_LENGTH_TWENTY_FIVE];
float tracking_data_matrix[ARRAY_LENGTH_FOUR][ARRAY_LENGTH_SIXTEEN][ARRAY_LENGTH_TWENTY_FIVE];
int respiration_judge_matrix[ARRAY_LENGTH_FOUR][ARRAY_LENGTH_SIXTEEN];
s16 frame_data[ARRAY_LENGTH_SIXTEEN][ARRAY_LENGTH_MAX];
s16 data_ready = 0, presence_flag = 0, respiration_judge_flag = 0;

int preprocess_fmcw(s16 *data) {
    int len, ret;
    float *v_fft_tar_abs;

    len = ARRAY_LENGTH_SIXTEEN * ARRAY_LENGTH_TWENTY_FIVE;
    v_fft_tar_abs = alloc_mem(len * sizeof(float));

    memmove((void *)respiration_data_matrix, (void *)respiration_data_matrix[1],
            (ARRAY_LENGTH_MAX - 1) * ARRAY_LENGTH_THIRTY_TWO * (sizeof(float)));

    td_fft((void *)data, respiration_data_matrix[ARRAY_LENGTH_MAX - 1], (void *)v_fft_tar_abs);

    memmove((void *)tracking_data_matrix, (void *)tracking_data_matrix[1],
            len * (ARRAY_LENGTH_FOUR - 1) * (sizeof(float)));
    memcpy((void *)tracking_data_matrix[ARRAY_LENGTH_FOUR - 1],
            v_fft_tar_abs, len * sizeof(float));

    frame_index += 1;

    if (!data_ready) {
        if (frame_index > RESPIRATION_PROCESS_NUM - BACKGROUND_PROCESS_NUM) {
            arm_scale_f32(v_fft_tar_abs, 1.0 / BACKGROUND_PROCESS_NUM, v_fft_tar_abs, len);
            arm_add_f32((void *)background_data_matrix, v_fft_tar_abs, (void *)background_data_matrix, len);
        }
        if (frame_index == RESPIRATION_PROCESS_NUM) {
            data_ready = 1;
            frame_index = 0;
        }
        ret = 0;
    } else {
        ret = 1;
    }

    free_mem(v_fft_tar_abs);
    return ret;
}

void detect_presence(void) {
    float range, velocity, *difference_matrix;
    int respiration_state, *respiration_locs;
    s16 max_data;

    difference_matrix = alloc_mem(ARRAY_LENGTH_SIXTEEN * ARRAY_LENGTH_TWENTY_FIVE * sizeof(float));

    if (frame_index % TRACKING_STEP_PROCESS_NUM == 0) {
        background_update(tracking_data_matrix, background_data_matrix, (void *)difference_matrix);
        if (presence_flag == 0) {
            target_tracking((void *)difference_matrix, &range, &velocity);
            if (range < RANGE_LINE && range > 0) {
                presence_flag = 1;
                frame_index = 0;
            }
        }
    }
    free_mem(difference_matrix);

    if (presence_flag == 1 && frame_index == RESPIRATION_STEP_PROCESS_NUM) {
        respiration_locs = alloc_mem(ARRAY_LENGTH_SIXTEEN * sizeof(int));

        respiration_state = respiration_detection(respiration_data_matrix, respiration_locs);

        memcpy(respiration_judge_matrix[respiration_judge_index],
             respiration_locs, ARRAY_LENGTH_SIXTEEN * sizeof(int));

        respiration_judge_index += 1;

        if (respiration_judge_index >= RESPIRATION_TOTAL_TIMES) {
            respiration_judge_flag = 1;
            respiration_judge_index = 0;
        }

        if (respiration_judge_flag) {
            max_data = get_max(respiration_judge_matrix);
            if (max_data >= RESPIRATION_EFFECTIVE_TIMES)
                presence_flag = 1;
            else {
                presence_flag = 0;
                respiration_judge_flag = 0;
                respiration_judge_index = 0;
            }
        }
        frame_index = 0;

        free_mem(respiration_locs);
    }
}

s16 get_max(int data[ARRAY_LENGTH_FOUR][ARRAY_LENGTH_SIXTEEN]) {
    s16 sum_data[ARRAY_LENGTH_SIXTEEN], output, sum;
    u32 p;
    int i, j;

    for (i = 0; i < ARRAY_LENGTH_SIXTEEN; i++) {
        sum = 0;
        for (j = 0; j < ARRAY_LENGTH_FOUR; j++)
            sum += data[j][i];
        sum_data[i] = sum;
    }

    arm_max_q15(sum_data, ARRAY_LENGTH_SIXTEEN, &output, &p);

    return output;
}

void detect(s16 *data, output_result_t_fmcw *output_result)
{
    int ret;
    ret = preprocess_fmcw(data);

    if (ret) {
        detect_presence();
    }
}

