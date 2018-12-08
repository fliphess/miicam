/**
 * @file capture_tamper_detection.c
 *  This source code is capture_tamper_detection algorithm
 * Copyright (C) 2013 GM Corp. (http://www.grain-media.com)
 *
 * $Revision: 1.4 $
 * $Date: 2014/04/29 00:04:55 $
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <signal.h>
#include "gmlib.h"
#include "capture_tamper_detection.h"
#include "parsing_mv_data.c"

pthread_mutex_t  mapping_mutex;
static int previous_frame_motion_size[MAX_CAP_MD_NUM][3];
static int previous_tamper_det_result[MAX_CAP_MD_NUM];
static int user_motion_size[MAX_CAP_MD_NUM] = {0};
struct tp_user_t tp_param[MAX_CAP_MD_NUM];
struct tp_res_t active[MAX_CAP_MD_NUM];

int capture_tamper_algo(struct tp_user_t *tp_param, struct tp_res_t *active, int tp_idx)
{
    int ret = 0;
    int blk_num; 
    int s_threshold;
    int t_threshold;
    double ratio;
    int c_motion;
    int p_motion_1;
    int p_motion_2;
    int p_motion_3;

    blk_num     = tp_param->tp_alg.mb_w_num * tp_param->tp_alg.mb_h_num; 
    s_threshold = blk_num * 0.3;
    t_threshold = tp_param->tp_alg.t_sensitive_th;
    ratio = 1 + 2 * ((100 - tp_param->tp_alg.s_sensitive_th) / 100.0);
    c_motion = active->active_num;
    p_motion_1 = previous_frame_motion_size[tp_idx][0];
    p_motion_2 = previous_frame_motion_size[tp_idx][1];
    p_motion_3 = previous_frame_motion_size[tp_idx][2];
    
    if (c_motion > s_threshold) {
        if (previous_tamper_det_result[tp_idx] > 0) {
            if (previous_tamper_det_result[tp_idx] < t_threshold) {
                previous_tamper_det_result[tp_idx]++;
                ret = NO_TAMPER;
            } else {
                ret = TAMPER_DETECTED;
            }
        } else if ( (c_motion > (p_motion_1 * ratio)) ||
                    (c_motion > (p_motion_2 * ratio)) ||
                    (c_motion > (p_motion_3 * ratio))  ) {
            previous_tamper_det_result[tp_idx]++;
            ret = NO_TAMPER;
        } else {
            previous_tamper_det_result[tp_idx] = 0;
            ret = NO_TAMPER;
        }  
    } else {
        previous_tamper_det_result[tp_idx] = 0;
        ret = NO_TAMPER;
    }
    
    previous_frame_motion_size[tp_idx][2] = previous_frame_motion_size[tp_idx][1];
    previous_frame_motion_size[tp_idx][1] = previous_frame_motion_size[tp_idx][0]; 
    previous_frame_motion_size[tp_idx][0] = c_motion;
          
#if T_DEBUG
    printf("%d %d %d %d %d",c_motion,s_threshold,p_motion_1,p_motion_2,p_motion_3);
#endif

    return ret;
}

/*
 * @brief capture tamper detection algorithm
 *
 * @fn int do_capture_tamper(gm_cap_md_info_t *cap_motion, struct tp_user_t *tp_param, 
                       struct tp_res_t *active, int tp_idx)
 * @param cap_motion motion data buffer the region you want to detect motion
 * @param tp_param tamper detection parameters
 * @param active result of motion detection 
 * @param tp_idx motion data channel num
 * @return 1 on tamper detected, 2 on no tamper, <0 on tamper error
*/
int do_capture_tamper(gm_cap_md_info_t *cap_motion, struct tp_user_t *tp_param, 
                       struct tp_res_t *active, int tp_idx)
{
    int ret = 0;

    if ((tp_param->mv_param.k_height == cap_motion->md_dim.height) &&
        (tp_param->mv_param.k_width == cap_motion->md_dim.width) &&
        (cap_motion->is_valid == 1)) {
        if (tp_param->tp_alg.frame_count < tp_param->tp_alg.training_time)
            tp_param->tp_alg.frame_count++;
    } else {
        tp_param->tp_alg.frame_count = 0;
        tp_param->mv_param.k_height = cap_motion->md_dim.height;
        tp_param->mv_param.k_width = cap_motion->md_dim.width;
        tp_param->mv_param.k_mb_height = cap_motion->md_mb.height; 
        tp_param->mv_param.k_mb_width = cap_motion->md_mb.width;
    }
    
    tp_param->mv_param.active_flag = active->active_flag;
    tp_param->mv_param.active_num = 0;
    ret = parsing_mv_data((unsigned char *)cap_motion->md_buf, &tp_param->mv_param,
                           tp_param->tp_alg.u_width, tp_param->tp_alg.u_height, 
                           tp_param->tp_alg.u_mb_width, tp_param->tp_alg.u_mb_height, 
                           tp_param->tp_alg.sensitive_th, tp_param->tp_alg.frame_count,
                           tp_param->tp_alg.mb_w_num, tp_param->tp_alg.mb_h_num, tp_idx);
    if (ret < 0) {
        ret = MOTION_PARSING_ERROR;
        goto ext;
    }
    active->active_num = tp_param->mv_param.active_num;
    
    ret = capture_tamper_algo(tp_param, active, tp_idx);
    if (ret < 0) {
        ret = TAMPER_ALGO_ERROR;
        goto ext;
    }

ext:

    return ret;
}

int capture_tamper_init(struct tp_user_t *tp_param, struct tp_res_t *active, int tp_idx)
{
    int i = 0;
    int j = 0;
    int ret = 0;
    int u_height;
    int u_width;
    int u_mb_height;
    int u_mb_width;
    int blk_h_num;
    int blk_w_num;
    int user_motion_size_init;

    u_height    = tp_param->tp_alg.u_height;
    u_width     = tp_param->tp_alg.u_width;
    u_mb_height = tp_param->tp_alg.u_mb_height;
    u_mb_width  = tp_param->tp_alg.u_mb_width;
    blk_h_num   = (u_height + (u_mb_height - 1)) / u_mb_height;
    blk_w_num   = (u_width + (u_mb_width - 1)) / u_mb_width;
    user_motion_size_init = blk_h_num * blk_w_num;

    tp_param->tp_alg.mb_w_num = blk_w_num;
    tp_param->tp_alg.mb_h_num = blk_h_num;
    
    tp_param->mv_param.k_height = 0;
    tp_param->mv_param.k_width = 0;
    tp_param->mv_param.k_mb_height = 0;
    tp_param->mv_param.k_mb_width = 0;

    active->active_flag = (unsigned char *) malloc(sizeof(unsigned char) * user_motion_size_init);
    if (active->active_flag == NULL)
        return TAMPER_INIT_ERROR;
    memset(active->active_flag, 0, sizeof(unsigned char) * user_motion_size_init);
    active->active_num = 0;

    ret = parsing_mv_initial(tp_param->tp_alg.mb_w_num, tp_param->tp_alg.mb_h_num, tp_idx);
    if (ret < 0) {
        parsing_mv_end(&tp_param->mv_param, tp_idx);
        return TAMPER_INIT_ERROR;
    }

    for (i = 0; i < 32; i++) {
        for (j = 0; j < 3; j++) {
            previous_frame_motion_size[i][j] = 0;    
            previous_tamper_det_result[i] = 0;
        }
    }

    user_motion_size[tp_idx] = user_motion_size_init;

    return 0 ;
}

int capture_tamper_update(struct tp_user_t *tp_param, struct tp_res_t *active, int tp_idx)
{
    int i = 0;
    int j = 0;
    int ret = 0;
    int user_motion_size_update;

    user_motion_size_update = tp_param->tp_alg.mb_w_num * tp_param->tp_alg.mb_h_num;
    
    if (user_motion_size[tp_idx] < user_motion_size_update) {
        free(active->active_flag);
        active->active_flag = (unsigned char *)malloc(sizeof(unsigned char) * 
                                                      user_motion_size_update); 
        if (active->active_flag == NULL)
            return TAMPER_INIT_ERROR;
        user_motion_size[tp_idx] = user_motion_size_update;
    }
    
    tp_param->mv_param.k_height = 0;
    tp_param->mv_param.k_width = 0;
    tp_param->mv_param.k_mb_height = 0;
    tp_param->mv_param.k_mb_width = 0;

    memset(active->active_flag, 0, sizeof(unsigned char) * user_motion_size_update);
    active->active_num = 0;

    ret = parsing_mv_initial(tp_param->tp_alg.mb_w_num, tp_param->tp_alg.mb_h_num, tp_idx);
    if (ret < 0) {
        parsing_mv_end(&tp_param->mv_param, tp_idx);
        return TAMPER_INIT_ERROR;
    }

    for (i = 0; i < 32; i++) {
        for (j = 0; j < 3; j++) {
            previous_frame_motion_size[i][j] = 0;    
            previous_tamper_det_result[i] = 0;
        }
    }

    return 0 ;
}

int capture_tamper_end(struct tp_user_t *tp_param, struct tp_res_t *active, int tp_idx)
{
    if (active->active_flag != NULL) {
        free(active->active_flag);
        active->active_flag = NULL;
    }

    parsing_mv_end(&tp_param->mv_param, tp_idx);

    return 0;
}

int bindfd_to_tp_idx(void *bindfd)
{
    int tp_idx;

    pthread_mutex_lock(&mapping_mutex);
 
    for (tp_idx = 0; tp_idx < MAX_CAP_MD_NUM; tp_idx++) {
        if (tp_param[tp_idx].bindfd == bindfd)
            break;
    }
    if (tp_idx >= MAX_CAP_MD_NUM) {
        for (tp_idx = 0; tp_idx < MAX_CAP_MD_NUM; tp_idx++) {
            if (tp_param[tp_idx].bindfd == NULL) {
                tp_param[tp_idx].bindfd = bindfd;
                break;
            }
        }
        if (tp_idx >= MAX_CAP_MD_NUM) {
            printf("Error, find no available tp_idx, out of resource\n");
            tp_idx = -1;
            goto err_ext;
        }
    }

err_ext:

    pthread_mutex_unlock(&mapping_mutex);

    return tp_idx;
}

int tp_idx_to_cap_md_idx(int tp_idx, gm_multi_cap_md_t *cap_md, int num_cap_md)
{
    int cap_md_idx;
    int ret = 0;

    pthread_mutex_lock(&mapping_mutex);

    for (cap_md_idx = 0; cap_md_idx < num_cap_md; cap_md_idx++) {
        if(tp_param[tp_idx].bindfd == cap_md[cap_md_idx].bindfd)
            break;
    }

    if (cap_md_idx >= num_cap_md)
        ret = -1;
    else
        ret = cap_md_idx;

    pthread_mutex_unlock(&mapping_mutex);

    return ret;
}

static int tamper_detection_init(void)
{
    int tp_idx, ret = 0;

    for (tp_idx = 0; tp_idx < MAX_CAP_MD_NUM; tp_idx++) {

        /*motion detection*/
        pthread_mutex_init(&tp_param[tp_idx].tp_mutex, NULL);
        tp_param[tp_idx].tp_alg.u_height = 1920;
        tp_param[tp_idx].tp_alg.u_width = 1080;
        tp_param[tp_idx].tp_alg.u_mb_height = 32;
        tp_param[tp_idx].tp_alg.u_mb_width = 32;
        tp_param[tp_idx].tp_alg.training_time = 15;
        tp_param[tp_idx].tp_alg.frame_count = 0;
        tp_param[tp_idx].tp_alg.sensitive_th = 80; 
        tp_param[tp_idx].tp_alg.s_sensitive_th = 75; 
        tp_param[tp_idx].tp_alg.t_sensitive_th = 1; 
        tp_param[tp_idx].bindfd = NULL;
        
        ret = capture_tamper_init(&tp_param[tp_idx], &active[tp_idx], tp_idx);
        if (ret < 0) {
            printf("tamper_detection_init error!\n");
            capture_tamper_end(&tp_param[tp_idx], &active[tp_idx], tp_idx); //motion detectione end
            ret = -1;
            continue;
        }
    }

    return ret;
}

static int tamper_detection_end(void)
{
    int tp_idx;
    int ret = 0;

    for (tp_idx = 0; tp_idx < MAX_CAP_MD_NUM; tp_idx++) {
        /*motion detection end*/
        pthread_mutex_destroy(&tp_param[tp_idx].tp_mutex);
        ret = capture_tamper_end(&tp_param[tp_idx], &active[tp_idx], tp_idx);
        if (ret < 0) {
            printf("tamper_detection_end error at tp_idx(%d)\n", tp_idx);
            ret = -1;
            continue;
        }
        user_motion_size[tp_idx] = 0;
    }
    return ret;
}

int tamper_detection_update(void *bindfd, struct tp_alg_t *tp_alg)
{
    int ret = 0;
    int tp_idx;

    tp_idx = bindfd_to_tp_idx(bindfd);
     
    pthread_mutex_lock(&tp_param[tp_idx].tp_mutex);
    tp_param[tp_idx].tp_alg.u_height = tp_alg->u_height;
    tp_param[tp_idx].tp_alg.u_width = tp_alg->u_width;
    tp_param[tp_idx].tp_alg.u_mb_height = tp_alg->u_mb_height;
    tp_param[tp_idx].tp_alg.u_mb_width = tp_alg->u_mb_width;
    tp_param[tp_idx].tp_alg.training_time = tp_alg->training_time;
    tp_param[tp_idx].tp_alg.frame_count = tp_alg->frame_count;
    tp_param[tp_idx].tp_alg.sensitive_th = tp_alg->sensitive_th; 
    tp_param[tp_idx].tp_alg.s_sensitive_th = tp_alg->s_sensitive_th; 
    tp_param[tp_idx].tp_alg.t_sensitive_th = tp_alg->t_sensitive_th; 
    tp_param[tp_idx].tp_alg.mb_w_num = tp_alg->mb_w_num;
    tp_param[tp_idx].tp_alg.mb_h_num = tp_alg->mb_h_num;
    tp_param[tp_idx].bindfd = bindfd;
    
    ret = capture_tamper_update(&tp_param[tp_idx], &active[tp_idx], tp_idx);
    if (ret < 0) {
        printf("capture_tamper_update error at bindfd(0x%p)\n", bindfd);
        capture_tamper_end(&tp_param[tp_idx], &active[tp_idx], tp_idx); //motion detectione end
        ret = -1;
        goto err_ext;
    }

err_ext:
    pthread_mutex_unlock(&tp_param[tp_idx].tp_mutex);

    return ret;
}

static int tamper_detection_handling(gm_multi_cap_md_t *cap_md, struct tp_result_t *tp_result,
                                     int num_cap_md)
{   
    int tp_idx, ret = 0;
    int cap_md_idx;
    int result;

    for (tp_idx = 0; tp_idx < MAX_CAP_MD_NUM; tp_idx++) {
        pthread_mutex_lock(&tp_param[tp_idx].tp_mutex);
        cap_md_idx = tp_idx_to_cap_md_idx(tp_idx, cap_md, num_cap_md);
        if (cap_md_idx < 0) {
            pthread_mutex_unlock(&tp_param[tp_idx].tp_mutex);
            continue;
        }
        if (cap_md[cap_md_idx].retval < 0) {
            printf("Error to get capure motion data from this channel.\n");
            pthread_mutex_unlock(&tp_param[tp_idx].tp_mutex);
            continue;
        }
        if (cap_md[cap_md_idx].cap_md_info.is_valid == 0) {
            printf("Motion data has not been ready! please get it again.\n");
            pthread_mutex_unlock(&tp_param[tp_idx].tp_mutex);
            continue;
        }
         
        result = do_capture_tamper(&cap_md[cap_md_idx].cap_md_info, &tp_param[tp_idx],
                                   &active[tp_idx], tp_idx);
        
        tp_result[cap_md_idx].bindfd = cap_md[cap_md_idx].bindfd;
        tp_result[cap_md_idx].result = result;
        pthread_mutex_unlock(&tp_param[tp_idx].tp_mutex);
    }

    return ret;
}


