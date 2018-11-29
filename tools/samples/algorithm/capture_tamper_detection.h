#ifndef _CAPTURE_TAMPER_DETECTION_H_
#define _CAPTURE_TAMPER_DETECTION_H_
#include "parsing_mv_data.h"

#define T_DEBUG             0 
#define MAX_CAP_MD_NUM      32 

#define TAMPER_DETECTED     1
#define NO_TAMPER           2

#define TAMPER_INIT_ERROR  -1
#define TAMPER_ALGO_ERROR  -2

struct tp_result_t {
    void *bindfd;
    int result;
};

struct tp_alg_t {
    unsigned int u_height;          /*  user efine height
                                        suggest not change default value    */
    unsigned int u_width;           /*  user define width
                                        suggest not change default value    */
    unsigned int u_mb_height;       /*  user define mb height (suggest value = k_mb_widthh * 2)
                                        too large would affect the accuracy of motion detection */
    unsigned int u_mb_width;        /*  user define mb width (suggest value = k_mb_widthh * 2)
                                        too large would affect the accuracy of motion detection */
    unsigned int sensitive_th;      /*  control the transform form kernel mv data to user mv data
                                        (suggest value = 60)  */
    unsigned int frame_count;
    unsigned int training_time;     /*  Needed training frames number for motion detection 
                                        application  */
    unsigned int s_sensitive_th;    /*  spatail threshold for tamper detection */
    unsigned int t_sensitive_th;    /*  temporal threshold for tamper detection */

    unsigned int    mb_w_num;       /* The macroblock number of width */
    unsigned int    mb_h_num;       /* The macroblock number of height */
};

struct tp_user_t {
    struct mv_cfg_t mv_param;       /*  param for motion data */
    struct tp_alg_t tp_alg;         /*  param for tamper detection application */
    pthread_mutex_t tp_mutex;       /*  mutex for resource protection */
    void *bindfd;                   /*  for check only */
};

struct tp_res_t {
    unsigned int    active_num;     /*  total motion Macroblock number of captured frame */
    unsigned char   *active_flag;   /*  motion detection result of each Macroblock */
};

int tamper_detection_update(void *bindfd, struct tp_alg_t *tp_alg);


#endif

