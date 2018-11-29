/**
 * @file capture_motion_detection.c
 *  This source code is capture_motion_detection algorithm
 * Copyright (C) 2013 GM Corp. (http://www.grain-media.com)
 *
 * $Revision: 1.8 $
 * $Date: 2014/07/18 07:01:45 $
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
#include "capture_motion_detection.h"
#include "parsing_mv_data.c"

pthread_mutex_t  mapping_mutex;
static int cap_motion_init_ok[MAX_CAP_MD_NUM] = {0};
static int user_motion_size[MAX_CAP_MD_NUM] = {0};
struct mdt_user_t mdt_param[MAX_CAP_MD_NUM];
struct mdt_res_t active[MAX_CAP_MD_NUM];
    
static int capture_motion_algo(struct mdt_user_t *mdt_param, struct mdt_res_t *active, int mdt_idx)
{
    int a, b;
    int blk_w_num;
    int blk_h_num;
 
    blk_w_num = mdt_param->mdt_alg.mb_w_num;
    blk_h_num = mdt_param->mdt_alg.mb_h_num;

    active->active_num = 0;
    for (a = 0; a < blk_h_num; a++) {
        for (b = 0; b < blk_w_num; b++) {
           int pixel_index = a * blk_w_num + b;

            if (active->active_flag[pixel_index] == 0) {
                int noise = 1;

                if ((a + 1) < blk_h_num)
                    if (active->active_flag[pixel_index + blk_w_num] == 0)
                        noise = 0;

                if ((a - 1) > -1)
                    if (active->active_flag[pixel_index - blk_w_num] == 0)
                        noise = 0;

                if ((b + 1) < blk_w_num)
                    if (active->active_flag[pixel_index + 1] == 0)
                        noise = 0;

                if ((b - 1) > -1)
                    if (active->active_flag[pixel_index - 1] == 0)
                        noise = 0;

                if (noise == 1)
                    active->active_flag[pixel_index] = 1;
                else {
                    active->active_flag[pixel_index] = 0;
                    if (mdt_param->mdt_alg.mb_cell_en[pixel_index] == 1)
                        active->active_num++;
                }
            }
        }
    }

#if M_DEBUG
    printf("blk_info\n");
    for (a = 0; a < blk_h_num; a++) {
        for (b = 0; b < blk_w_num; b++) {
            printf("%d",active->active_flag[a * blk_w_num + b]);
        }
        printf("\n");
    }    
#endif

    return 0;
}

/*
 * @brief capture motion detection algorithm
 *
 * @fn int do_capture_motion(gm_cap_md_info_t *cap_motion, struct mdt_user_t *mdt_param,
                       struct mdt_res_t *active, int mdt_idx)
 * @param cap_motion motion data buffer the region you want to detect motion
 * @param mdt_param motion detection parameters 
 * @param active result of motion detection
 * @param mdt_idx motion data channel num
 * @return 1 on motion detected, 0 on motion unreliable, 2 on motion undetected, <0 on motion error
*/
static int do_capture_motion(gm_cap_md_info_t *cap_motion, struct mdt_user_t *mdt_param, 
                       struct mdt_res_t *active, int mdt_idx)
{
    int ret = 0;

    if (cap_motion_init_ok[mdt_idx] == 0) {
        printf("capture motion not initial\n");
        ret = MOTION_INIT_ERROR;
        goto err_ext;
    }

    if ((mdt_param->mv_param.k_height == cap_motion->md_dim.height) &&
        (mdt_param->mv_param.k_width == cap_motion->md_dim.width) &&
        (cap_motion->is_valid == 1)) {
        if (mdt_param->mdt_alg.frame_count < mdt_param->mdt_alg.training_time)
            mdt_param->mdt_alg.frame_count++;
    } else {
        mdt_param->mdt_alg.frame_count = 0;
        mdt_param->mv_param.k_height = cap_motion->md_dim.height;
        mdt_param->mv_param.k_width = cap_motion->md_dim.width;
        mdt_param->mv_param.k_mb_height = cap_motion->md_mb.height; 
        mdt_param->mv_param.k_mb_width = cap_motion->md_mb.width;
    }
    
    mdt_param->mv_param.active_flag = active->active_flag;
    mdt_param->mv_param.active_num = 0;

    ret = parsing_mv_data((unsigned char *)cap_motion->md_buf,
                          &mdt_param->mv_param,
                          mdt_param->mdt_alg.u_width,
                          mdt_param->mdt_alg.u_height,
                          mdt_param->mdt_alg.u_mb_width,
                          mdt_param->mdt_alg.u_mb_height,
                          mdt_param->mdt_alg.sensitive_th,
                          mdt_param->mdt_alg.frame_count,
                          mdt_param->mdt_alg.mb_w_num,
                          mdt_param->mdt_alg.mb_h_num,
                          mdt_idx);
    if (ret < 0) {
        ret = MOTION_PARSING_ERROR;
        goto err_ext;
    }
    active->active_num = mdt_param->mv_param.active_num;
    
    ret = capture_motion_algo(mdt_param, active, mdt_idx);
    if (ret < 0) {
        ret = MOTION_ALGO_ERROR;
        goto err_ext;
    }

    if (mdt_param->mdt_alg.frame_count < mdt_param->mdt_alg.training_time) {
        ret = MOTION_IS_TRAINING;
        goto err_ext;
    }

    if (active->active_num > mdt_param->mdt_alg.alarm_th) {
        ret = MOTION_DETECTED;
        goto err_ext;
    } else {
        ret = NO_MOTION;
        goto err_ext;
    }

err_ext:
    
    return ret;
}

static int capture_motion_init(struct mdt_user_t *mdt_param, struct mdt_res_t *active, int mdt_idx)
{
    int ret = 0;
    int u_height;
    int u_width;
    int u_mb_height;
    int u_mb_width;
    int blk_h_num;
    int blk_w_num;
    int user_motion_size_init;

    u_height    = mdt_param->mdt_alg.u_height;
    u_width     = mdt_param->mdt_alg.u_width;
    u_mb_height = mdt_param->mdt_alg.u_mb_height;
    u_mb_width  = mdt_param->mdt_alg.u_mb_width;
    blk_h_num   = (u_height + (u_mb_height - 1)) / u_mb_height;
    blk_w_num   = (u_width + (u_mb_width - 1)) / u_mb_width;
    mdt_param->mdt_alg.mb_h_num = blk_h_num;
    mdt_param->mdt_alg.mb_w_num = blk_w_num;    
    user_motion_size_init = blk_h_num * blk_w_num;
    mdt_param->mdt_alg.alarm_th = user_motion_size_init * 5 / 100;

    mdt_param->mv_param.k_height = 0;
    mdt_param->mv_param.k_width = 0;
    mdt_param->mv_param.k_mb_height = 0;
    mdt_param->mv_param.k_mb_width = 0;

    active->active_flag = (unsigned char *)malloc(sizeof(unsigned char) * user_motion_size_init);
    if (active->active_flag == NULL) {
        ret = MOTION_INIT_ERROR;
        goto err_ext;
    }
    memset(active->active_flag, 0, sizeof(unsigned char) * user_motion_size_init);
    active->active_num = 0;

    ret = parsing_mv_initial(mdt_param->mdt_alg.mb_w_num, mdt_param->mdt_alg.mb_h_num, mdt_idx);
    if (ret < 0) {
        parsing_mv_end(&mdt_param->mv_param, mdt_idx);
        ret = MOTION_INIT_ERROR;
        goto err_ext;
    }

    cap_motion_init_ok[mdt_idx] = 1;
    user_motion_size[mdt_idx] = user_motion_size_init;

err_ext:

    return 0 ;
}

static int capture_motion_update(struct mdt_user_t *mdt_param, struct mdt_res_t *active, int mdt_idx)
{
    int ret = 0;
    int blk_h_num;
    int blk_w_num;
    int user_motion_size_update;

    if (cap_motion_init_ok[mdt_idx] == 0) {
        printf("capture motion not initial\n");
        ret = MOTION_INIT_ERROR;
        goto err_ext;
    }

    blk_h_num   = mdt_param->mdt_alg.mb_h_num;
    blk_w_num   = mdt_param->mdt_alg.mb_w_num;
    user_motion_size_update = blk_h_num * blk_w_num;
 
    if (user_motion_size[mdt_idx] < user_motion_size_update) {
        free(active->active_flag);
        active->active_flag = (unsigned char *) malloc(sizeof(unsigned char) * 
                                                       user_motion_size_update);
        if (active->active_flag == NULL) {
            ret = MOTION_INIT_ERROR;
            goto err_ext;
        }

        user_motion_size[mdt_idx] = user_motion_size_update;
    }

    mdt_param->mv_param.k_height = 0;
    mdt_param->mv_param.k_width = 0;
    mdt_param->mv_param.k_mb_height = 0;
    mdt_param->mv_param.k_mb_width = 0;

    memset(active->active_flag, 0, sizeof(unsigned char) * user_motion_size_update);
    active->active_num = 0;

    ret = parsing_mv_initial(mdt_param->mdt_alg.mb_w_num, mdt_param->mdt_alg.mb_h_num, mdt_idx);
    if (ret < 0) {
        parsing_mv_end(&mdt_param->mv_param, mdt_idx);
        ret = MOTION_INIT_ERROR;
        goto err_ext;
    }

err_ext:

    return ret;
}

static int capture_motion_end(int mdt_idx)
{

    cap_motion_init_ok[mdt_idx] = 0;

    if (mdt_param[mdt_idx].mdt_alg.mb_cell_en != NULL) {
        free(mdt_param[mdt_idx].mdt_alg.mb_cell_en);
        mdt_param[mdt_idx].mdt_alg.mb_cell_en = NULL;
    }   

    if (active[mdt_idx].active_flag != NULL) {
        free(active[mdt_idx].active_flag);
        active[mdt_idx].active_flag = NULL;
    }

    cap_motion_init_ok[mdt_idx] = 0;
    user_motion_size[mdt_idx] = 0;

    parsing_mv_end(&mdt_param[mdt_idx].mv_param, mdt_idx);

    return 0;
}

int bindfd_to_mdt_idx(void *bindfd)
{
    int mdt_idx;

    pthread_mutex_lock(&mapping_mutex);
 
    for (mdt_idx = 0; mdt_idx < MAX_CAP_MD_NUM; mdt_idx++) {
        if (mdt_param[mdt_idx].bindfd == bindfd)
            break;
    }
    if (mdt_idx >= MAX_CAP_MD_NUM) {
        for (mdt_idx = 0; mdt_idx < MAX_CAP_MD_NUM; mdt_idx++) {
            if (mdt_param[mdt_idx].bindfd == NULL) {
                mdt_param[mdt_idx].bindfd = bindfd;
                break;
            }
        }
        if (mdt_idx >= MAX_CAP_MD_NUM) {
            printf("Error, find no available mdt_idx, out of resource\n");
            mdt_idx = -1;
            goto err_ext;
        }
    }

err_ext:

    pthread_mutex_unlock(&mapping_mutex);

    return mdt_idx;
}

int mdt_idx_to_cap_md_idx(int mdt_idx, gm_multi_cap_md_t *cap_md, int num_cap_md)
{
    int cap_md_idx;
    int ret = 0;

    pthread_mutex_lock(&mapping_mutex);

    for (cap_md_idx = 0; cap_md_idx < num_cap_md; cap_md_idx++) {
        if(mdt_param[mdt_idx].bindfd == cap_md[cap_md_idx].bindfd)
            break;
    }

    if (cap_md_idx >= num_cap_md)
        ret = -1;
    else
        ret = cap_md_idx;

    pthread_mutex_unlock(&mapping_mutex);

    return ret;
}

int motion_detection_init(void)
{
    int mdt_idx;
    int ret = 0;

    pthread_mutex_init(&mapping_mutex, NULL);
    for (mdt_idx = 0; mdt_idx < MAX_CAP_MD_NUM; mdt_idx++) {
        /*motion detection*/
        pthread_mutex_init(&mdt_param[mdt_idx].mdt_mutex, NULL);
        mdt_param[mdt_idx].mdt_alg.u_height = 1920;
        mdt_param[mdt_idx].mdt_alg.u_width = 1080;
        mdt_param[mdt_idx].mdt_alg.u_mb_height = 32;
        mdt_param[mdt_idx].mdt_alg.u_mb_width = 32;
        mdt_param[mdt_idx].mdt_alg.training_time = 15;
        mdt_param[mdt_idx].mdt_alg.frame_count = 0;
        mdt_param[mdt_idx].mdt_alg.sensitive_th = 80;
        mdt_param[mdt_idx].mdt_alg.mb_cell_en = NULL;
        mdt_param[mdt_idx].mdt_alg.mb_w_num = 0;
        mdt_param[mdt_idx].mdt_alg.mb_h_num = 0;
                
        ret = capture_motion_init(&mdt_param[mdt_idx], &active[mdt_idx], mdt_idx);
        if (ret < 0) {
            printf("MD_Test: capture_motion_info_init error at mdt_idx(%d)\n", mdt_idx);
            capture_motion_end(mdt_idx); //motion detectione end
            ret = -1;
            continue;
        }
    }

    return ret;
}

int motion_detection_end(void)
{
    int mdt_idx;
    int ret = 0;

    for (mdt_idx = 0; mdt_idx < MAX_CAP_MD_NUM; mdt_idx++) {
        /*motion detection end*/
        pthread_mutex_destroy(&mdt_param[mdt_idx].mdt_mutex);
        ret = capture_motion_end(mdt_idx);
        if (ret < 0) {
            printf("MD_Test: capture_motion_end error at mdt_idx(%d)\n", mdt_idx);
            ret = -1;
            continue;
        }
    }
    pthread_mutex_destroy(&mapping_mutex);
    return ret;
}

int motion_detection_update(void *bindfd, struct mdt_alg_t *mdt_alg)
{
    int ret = 0;
    int mdt_idx;
    int mb_cell_size_ori;
    int mb_cell_size_new;

    mdt_idx = bindfd_to_mdt_idx(bindfd);
    if (mdt_idx == -1) {
        printf("The array depth of mdt_param was not enough for bindfd(0x%p)\n", bindfd);
        ret = -1;
        return ret;
    }

    pthread_mutex_lock(&mdt_param[mdt_idx].mdt_mutex);
    mdt_param[mdt_idx].mdt_alg.u_height = mdt_alg->u_height;
    mdt_param[mdt_idx].mdt_alg.u_width = mdt_alg->u_width;
    mdt_param[mdt_idx].mdt_alg.u_mb_height = mdt_alg->u_mb_height;
    mdt_param[mdt_idx].mdt_alg.u_mb_width = mdt_alg->u_mb_width;
    mdt_param[mdt_idx].mdt_alg.training_time = mdt_alg->training_time;
    mdt_param[mdt_idx].mdt_alg.frame_count = mdt_alg->frame_count;
    mdt_param[mdt_idx].mdt_alg.sensitive_th = mdt_alg->sensitive_th;
    mdt_param[mdt_idx].mdt_alg.alarm_th = mdt_alg->alarm_th;
    mdt_param[mdt_idx].bindfd = bindfd;

    mb_cell_size_ori = sizeof(unsigned char) * mdt_param[mdt_idx].mdt_alg.mb_w_num * 
                       mdt_param[mdt_idx].mdt_alg.mb_h_num;
    mb_cell_size_new = sizeof(unsigned char) * mdt_alg->mb_w_num * mdt_alg->mb_h_num;

    if (mb_cell_size_new == 0) {
        printf("Error, mb_w_num(%d) * mb_h_num(%d) = 0 at bindfd(0x%p)\n", mdt_alg->mb_w_num,
               mdt_alg->mb_h_num, bindfd);
        ret = -1;
        goto err_ext;
    }
    if ((mb_cell_size_ori < mb_cell_size_new) || (mdt_param[mdt_idx].mdt_alg.mb_cell_en == NULL)) { 
        if (mdt_param[mdt_idx].mdt_alg.mb_cell_en)
            free(mdt_param[mdt_idx].mdt_alg.mb_cell_en);
        mdt_param[mdt_idx].mdt_alg.mb_cell_en = (unsigned char *) malloc(mb_cell_size_new);
        if (mdt_param[mdt_idx].mdt_alg.mb_cell_en == NULL) {
            printf("Error to allocate mb_cell_en\n");
            ret = -1;
            goto err_ext;
        }
    }

    memcpy((void *)mdt_param[mdt_idx].mdt_alg.mb_cell_en, (void *)mdt_alg->mb_cell_en, 
            mb_cell_size_new);
    mdt_param[mdt_idx].mdt_alg.mb_w_num = mdt_alg->mb_w_num;
    mdt_param[mdt_idx].mdt_alg.mb_h_num = mdt_alg->mb_h_num;

    ret = capture_motion_update(&mdt_param[mdt_idx], &active[mdt_idx], mdt_idx);
    if (ret < 0) {
        printf("MD_Test: capture_motion_info_init error at mdt_idx(%d)\n", mdt_idx);
        capture_motion_end(mdt_idx); //motion detectione end
        ret = -1;
        goto err_ext;
    }
    
err_ext:
    pthread_mutex_unlock(&mdt_param[mdt_idx].mdt_mutex);

    return ret;
}

int motion_detection_handling(gm_multi_cap_md_t *cap_md, struct mdt_result_t *mdt_result,
                              int num_cap_md)
{
    int ret = 0;
    int mdt_idx; 
    int cap_md_idx;
    int result;

    for (mdt_idx = 0; mdt_idx < MAX_CAP_MD_NUM; mdt_idx++) {
        pthread_mutex_lock(&mdt_param[mdt_idx].mdt_mutex);
        cap_md_idx = mdt_idx_to_cap_md_idx(mdt_idx, cap_md, num_cap_md);
        if (cap_md_idx < 0) {
            pthread_mutex_unlock(&mdt_param[mdt_idx].mdt_mutex);
            continue;
        }
        if (cap_md[cap_md_idx].bindfd != mdt_param[mdt_idx].bindfd) {
            printf("check bindfd failed between cap_md and mdt_param at bindfd(0x%p)\n", 
                    cap_md[cap_md_idx].bindfd);
            mdt_result[cap_md_idx].bindfd = NULL;
            mdt_result[cap_md_idx].result = MOTION_INIT_ERROR;
            pthread_mutex_unlock(&mdt_param[mdt_idx].mdt_mutex);
            continue;
        }
        if (cap_md[cap_md_idx].retval < 0) {
            printf("Error to get capure motion data from this bindfd(0x%p).\n", cap_md[cap_md_idx].bindfd);
            mdt_result[cap_md_idx].bindfd = cap_md[cap_md_idx].bindfd;
            mdt_result[cap_md_idx].result = MOTION_DATA_ERROR;
            pthread_mutex_unlock(&mdt_param[mdt_idx].mdt_mutex);
            continue;
        }
        if (cap_md[cap_md_idx].cap_md_info.is_valid == 0) {
            printf("Motion data has not been ready! please get it again at bindfd(0x%p).\n", 
                    cap_md[cap_md_idx].bindfd);
            mdt_result[cap_md_idx].bindfd = cap_md[cap_md_idx].bindfd;
            mdt_result[cap_md_idx].result = NO_MOTION;
            pthread_mutex_unlock(&mdt_param[mdt_idx].mdt_mutex);
            continue;
        }
        result = do_capture_motion(&cap_md[cap_md_idx].cap_md_info, &mdt_param[mdt_idx],
                                   &active[mdt_idx], mdt_idx);
        mdt_result[cap_md_idx].bindfd = cap_md[cap_md_idx].bindfd;
        mdt_result[cap_md_idx].result = result;
        pthread_mutex_unlock(&mdt_param[mdt_idx].mdt_mutex);
    }
   
    return ret;
}

