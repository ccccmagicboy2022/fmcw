#ifndef __FMCW_PARAM_H__
#define __FMCW_PARAM_H__

/* FMCW param */
#define FC                                  (24.025f)           /* GHz */
#define BW                                  (200U)              /* MHz */
#define PW                                  (4e-4f)             /* s */
#define PRT                                 (6e-4f)             /* s */
#define IFF                                 (320000U)           /* Hz */
#define FRAME_TIME                          (0.0396f)           /* s */
#define SAMPLES_PER_CHIRP                   (128U)
#define CHIRPS_PER_FRAME                    (16U)

/* Operate param */
#define R_FFT_NUM                           (SAMPLES_PER_CHIRP * 2)         /* 256 */
#define R_MIN                               (0.5f)
#define R_MAX                               (6.0f)
#define V_FFT_NUM                           (CHIRPS_PER_FRAME * 4)          /* 64 */
#define V_MIN                               (-2.0f)
#define V_MAX                               (2.0f)
#define BACKGROUND_WIN_TIME                 (2.5344f)
#define TRACKING_WIN_TIME                   (0.1584f)
#define TRACKING_STEP_TIME                  (0.0792f)
#define RANGE_LINE                          (4.0f)
#define RESPIRATION_WIN_TIME                (5.0688f)
#define RESPIRATION_STEP_TIME               (2.5344f)
//#define NOISE_COEFFICIENT                   (1)
#define NOISE_UPDATE_WEIGHT                 (0.1f)
//#define DIFF_ENERGY_LINE                    (0.5f)
#define BACKGROUND_LINE                     (70)
#define RESPIRATION_TOTAL_TIMES             (4)
#define RESPIRATION_EFFECTIVE_TIMES         (2)

/* Const param */
#define LIGHT_SPEED                         (3e8)
#define SLOPE                               (BW * 1e6 / PW)
#define LAMDA                               (LIGHT_SPEED / (FC * 1e9))
#define FS_RESPIRATION                      (1 / FRAME_TIME)
#define BACKGROUND_PROCESS_NUM              ((int)(BACKGROUND_WIN_TIME / FRAME_TIME))
#define TRACKING_PROCESS_NUM                ((int)(TRACKING_WIN_TIME / FRAME_TIME))
#define TRACKING_STEP_PROCESS_NUM           ((int)(TRACKING_STEP_TIME / FRAME_TIME))
#define RESPIRATION_PROCESS_NUM             ((int)(RESPIRATION_WIN_TIME / FRAME_TIME))
#define RESPIRATION_STEP_PROCESS_NUM        ((int)(RESPIRATION_STEP_TIME / FRAME_TIME))


/* Range param */
#define RBIN                                (LIGHT_SPEED * (IFF / R_FFT_NUM) / (2 * SLOPE))

/* doppler param */
#define FS_DOP                              (1 / PRT)
#define VBIN                                (LAMDA / 2 * (FS_DOP / V_FFT_NUM))

#define RESPIRATION_LOCS_NUM                (16)

#define TRACKING_WIN_NUM                    (4)
#define R_MAX_INDEX                         (16)
#define R_MIN_INDEX                         (1)
#define R_NUM                               (R_MAX_INDEX - R_MIN_INDEX + 1)
#define V_MAX_INDEX                         (44)
#define V_MIN_INDEX                         (20)
#define V_NUM                               (V_MAX_INDEX - V_MIN_INDEX + 1)

#endif
