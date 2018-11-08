#ifndef _CAPTURE_MOTION_DETECTION_H_
#define _CAPTURE_MOTION_DETECTION_H_
#include "parsing_mv_data.h"

#define M_DEBUG             0
#define MAX_CAP_MD_NUM      32 

#define MOTION_IS_TRAINING  0
#define MOTION_DETECTED     1
#define NO_MOTION           2

#define MOTION_INIT_ERROR  -1
#define MOTION_ALGO_ERROR  -2
#define MOTION_DATA_ERROR  -3

struct mdt_result_t {
    void *bindfd;
    int result;
};

struct mdt_alg_t {
    unsigned int    u_height;           /*  user efine height
                                         suggest not change default value    */
    unsigned int    u_width;            /*  user define width
                                         suggest not change default value    */
    unsigned int    u_mb_height;        /*  user define mb height (suggest value = k_mb_widthh * 2)
                                         too large would affect the accuracy of motion detection */
    unsigned int    u_mb_width;         /*  user define mb width (suggest value = k_mb_widthh * 2)
                                         too large would affect the accuracy of motion detection */
    unsigned int    sensitive_th;       /*  control the transform form kernel mv data to user mv data
                                         (suggest value = 60)  */
    unsigned int    frame_count;
    unsigned int    training_time;      /*  Needed training frames number for motion detection 
                                         application  */
    unsigned int    mb_w_num;           /* The macroblock number of width */
    unsigned int    mb_h_num;           /* The macroblock number of height */
    unsigned char   *mb_cell_en;        /* enable motion detection of each Macroblock */
    unsigned int    alarm_th;           /* alarm threshold of total motion Macroblock number */
};

struct mdt_user_t {
    struct mv_cfg_t  mv_param;          /*  param for motion data */
    struct mdt_alg_t mdt_alg;           /*  param for motion detection application */
    pthread_mutex_t  mdt_mutex;         /*  mutex for resource protection */
    void *bindfd;                       /*  for check only */
};

struct mdt_res_t {
    unsigned int     active_num;        /*  total motion Macroblock number of captured frame */
    unsigned char    *active_flag;      /*  motion detection result of each Macroblock */
};

int motion_detection_init(void);
int motion_detection_end(void);
int motion_detection_update(void *bindfd, struct mdt_alg_t *mdt_alg);
int motion_detection_handling(gm_multi_cap_md_t *cap_md, struct mdt_result_t *mdt_result, int num_cap_md);


#endif

