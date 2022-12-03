#include <stdint.h>
#include <string.h>
#include "fmcw_param.h"
#include "sys.h"
#include "memalloc.h"
#include "arm_math.h"
#include "presence_detection_fmcw.h"
#include "functions.h"

int frame_index = 0, respiration_judge_index = 0;
float respiration_data_matrix[RESPIRATION_PROCESS_NUM][R_NUM * 2];
float background_data_matrix[R_NUM][V_NUM];
float tracking_data_matrix[TRACKING_WIN_NUM][R_NUM][V_NUM];
int respiration_judge_matrix[RESPIRATION_TOTAL_TIMES][RESPIRATION_LOCS_NUM];
s16 frame_data[CHIRPS_PER_FRAME][SAMPLES_PER_CHIRP];
s16 data_ready = 0, presence_flag = 0, respiration_judge_flag = 0;
s16 tracking_check_times = 0, tracking_check_flag = 0, tracking_check_finished = 0;
s16 respiration_check_times = 0, respiration_check_flag = 0, respiration_check_finished = 0;
s16 delay_times = 0;

detect_param_t detect_param;

int inquire_tracking_check_status(void)
{
    return tracking_check_flag;
}

int inquire_tracking_check_finished(void)
{
    return tracking_check_finished;
}

int inquire_respiration_check_status(void)
{
    return respiration_check_flag;
}

int inquire_respiration_check_finished(void)
{
    return respiration_check_finished;
}

void set_respiration_check(u8 enable)
{
    respiration_check_times = RESPIRATION_CHECK_TIMES;
    respiration_check_finished = 0;
    if (enable) {
        memset(&detect_param.background_line, 0, sizeof(detect_param.background_line));
        respiration_check_flag = 1;
        presence_flag = 1;
    } else {
        respiration_check_flag = 0;
        presence_flag = 0;
    }
}

void set_tracking_check(u8 enable)
{
    tracking_check_times = TACKING_CHECK_TIMES;
    tracking_check_finished = 0;
    if (enable) {
        memset(&detect_param.diff_energy_line, 0, sizeof(detect_param.diff_energy_line));
        tracking_check_flag = 1;
    }
    else
        tracking_check_flag = 0;
}

void set_delay_time(int delay_time)
{
    detect_param.delay_times = delay_time;
}

void set_range_line(int range_line)
{
    detect_param.range_line = range_line;

}

void set_background_line(detect_param_t *current_detect_param)
{
    memcpy(detect_param.background_line,
            current_detect_param->background_line, sizeof(detect_param.background_line));
}

void set_diff_energy_line(detect_param_t *current_detect_param)
{
    memcpy(detect_param.diff_energy_line,
            current_detect_param->diff_energy_line, sizeof(detect_param.diff_energy_line));

}

void get_detect_param(detect_param_t *current_detect_param)
{
    *current_detect_param = detect_param;
}

int preprocess_fmcw(s16 *data) {
    int len, ret;
    float *v_fft_tar_abs;

    len = R_NUM * V_NUM;
    v_fft_tar_abs = alloc_mem(len * sizeof(float));

    memmove((void *)respiration_data_matrix, (void *)respiration_data_matrix[1],
            (RESPIRATION_PROCESS_NUM - 1) * R_NUM * 2 * (sizeof(float)));

    td_fft((void *)data, respiration_data_matrix[RESPIRATION_PROCESS_NUM - 1], (void *)v_fft_tar_abs);

    memmove((void *)tracking_data_matrix, (void *)tracking_data_matrix[1],
            len * (TRACKING_WIN_NUM - 1) * (sizeof(float)));
    memcpy((void *)tracking_data_matrix[TRACKING_WIN_NUM - 1],
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

    difference_matrix = alloc_mem(R_NUM * V_NUM * sizeof(float));

    if (frame_index % TRACKING_STEP_PROCESS_NUM == 0) {
        background_update(tracking_data_matrix, background_data_matrix, (void *)difference_matrix);

        if (tracking_check_flag) {
            tracking_check((void *)difference_matrix, detect_param.diff_energy_line);
            tracking_check_times -= 1;
            if (tracking_check_times == 0) {
                arm_scale_f32(detect_param.diff_energy_line, TACKING_CHECK_WEIGHT, detect_param.diff_energy_line, R_NUM);
                tracking_check_finished = 1;
            }
        }else if (presence_flag == 0) {
            target_tracking((void *)difference_matrix, &range, &velocity, detect_param.diff_energy_line);
            if (range < RANGE_LINE && range > 0) {
                presence_flag = 1;
                frame_index = 0;
            }
        }
    }
    free_mem(difference_matrix);

#ifdef HEAP_TEST
    presence_flag = 1;
#endif

    if (presence_flag == 1 && frame_index == RESPIRATION_STEP_PROCESS_NUM) {
        if (respiration_check_flag) {
            respiration_check(respiration_data_matrix, detect_param.background_line);
            respiration_check_times -= 1;
            if (respiration_check_times == 0) {
                arm_scale_f32(detect_param.background_line, RESPIRATION_CHECK_WEIGHT, detect_param.background_line, R_NUM);
                respiration_check_finished = 1;
                presence_flag = 0;
            }
        } else {
            respiration_locs = alloc_mem(RESPIRATION_LOCS_NUM * sizeof(int));

            respiration_state = respiration_detection(respiration_data_matrix, respiration_locs);

            memcpy(respiration_judge_matrix[respiration_judge_index],
                 respiration_locs, RESPIRATION_LOCS_NUM * sizeof(int));

            respiration_judge_index += 1;

            if (respiration_judge_index >= RESPIRATION_TOTAL_TIMES) {
                respiration_judge_flag = 1;
                respiration_judge_index = 0;
                delay_times = ceil(detect_param.delay_times / RESPIRATION_STEP_TIME);
            }

            if (respiration_judge_flag) {
                max_data = get_max(respiration_judge_matrix);
                if (max_data >= RESPIRATION_EFFECTIVE_TIMES)
                    presence_flag = 1;
                else {
                    delay_times -= 1;
                    if (delay_times == 0) {
                        presence_flag = 0;
                        respiration_judge_flag = 0;
                        respiration_judge_index = 0;
                    }
                }
            }
        }
        frame_index = 0;

        free_mem(respiration_locs);
    }
}

s16 get_max(int data[RESPIRATION_TOTAL_TIMES][RESPIRATION_LOCS_NUM]) {
    s16 sum_data[RESPIRATION_LOCS_NUM], output, sum;
    u32 p;
    int i, j;

    for (i = 0; i < RESPIRATION_LOCS_NUM; i++) {
        sum = 0;
        for (j = 0; j < RESPIRATION_TOTAL_TIMES; j++)
            sum += data[j][i];
        sum_data[i] = sum;
    }

    arm_max_q15(sum_data, RESPIRATION_LOCS_NUM, &output, &p);

    return output;
}

void detect(s16 *data, output_result_t_fmcw *output_result)
{
    int ret;
    ret = preprocess_fmcw(data);

    if (ret) {
        detect_presence();
        *output_result = presence_flag;
    }
}