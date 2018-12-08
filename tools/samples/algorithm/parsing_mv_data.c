/**
 * @file parsing_mv_data.c
 *  This source code is parsing capture motion data
 * Copyright (C) 2013 GM Corp. (http://www.grain-media.com)
 *
 * $Revision: 1.8 $
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
#include "parsing_mv_data.h"

static int is_parsing_init_ok[MAX_CAP_MD_NUM] = {MD_S_NOT_READY};
static int *transform_matrix[MAX_CAP_MD_NUM] = {NULL};
static unsigned char *kernel_mv_data[MAX_CAP_MD_NUM] = {NULL};
static int g_blk_w_num[MAX_CAP_MD_NUM], 
           g_blk_h_num[MAX_CAP_MD_NUM], 
           g_user_width[MAX_CAP_MD_NUM], 
           g_user_height[MAX_CAP_MD_NUM], 
           g_mb_w_num[MAX_CAP_MD_NUM], 
           g_mb_h_num[MAX_CAP_MD_NUM];
static int parsing_user_motion_size[MAX_CAP_MD_NUM] = {0};

static int parsing_mv(unsigned char *event_buf, struct mv_cfg_t *mv_param, int mv_idx)
{
    int a, b, ret = 0;
    int mb_w_num;
    int mb_h_num;
    int byte_per_width;

    if (is_parsing_init_ok[mv_idx] == MD_S_NOT_READY) {
        printf("parsing not initial\n");
        return -1;
    }

    mb_w_num = mv_param->k_width  / mv_param->k_mb_width;
    mb_h_num = mv_param->k_height  / mv_param->k_mb_height;
    byte_per_width = (mb_w_num - 1) / 4 + 1;
    
    if (kernel_mv_data[mv_idx] == NULL)
        return -1;

    memset(kernel_mv_data[mv_idx], 0, sizeof(unsigned char) * CAP_MOTION_SIZE);

    //data transform, transfer mv_data
    for (a = 0; a < mb_h_num; a++) {
        for (b = 0; b < byte_per_width; b++) {
            if ((event_buf + b) == NULL) {
                printf("event_buf == NULL\n");
                return -1;
            } else {
                unsigned char mv_event = event_buf[b];
                int pixel_index = a * mb_w_num + b * 4;

                if (mv_event == 85) {
                    kernel_mv_data[mv_idx][pixel_index + 0] = 1;
                    kernel_mv_data[mv_idx][pixel_index + 1] = 1;
                    kernel_mv_data[mv_idx][pixel_index + 2] = 1;
                    kernel_mv_data[mv_idx][pixel_index + 3] = 1;
                } else {
                    kernel_mv_data[mv_idx][pixel_index + 0] = (mv_event >> 0) & 0x3;
                    kernel_mv_data[mv_idx][pixel_index + 1] = (mv_event >> 2) & 0x3;
                    kernel_mv_data[mv_idx][pixel_index + 2] = (mv_event >> 4) & 0x3;
                    kernel_mv_data[mv_idx][pixel_index + 3] = (mv_event >> 6) & 0x3;
                }
            }
        }
        event_buf += 32;
    }
    
    for (a = 1; a < (mb_h_num - 1); a++) {
        for (b = 1; b < (mb_w_num - 1); b++) {
            int pixel_index = a * mb_w_num + b;
            
            if (kernel_mv_data[mv_idx][pixel_index] == 0) {
                int noise = 1;

                if (kernel_mv_data[mv_idx][pixel_index + mb_w_num] == 0)
                    noise = 0;
                if (kernel_mv_data[mv_idx][pixel_index - mb_w_num] == 0)
                    noise = 0;
                if (kernel_mv_data[mv_idx][pixel_index + 1] == 0)
                    noise = 0;
                if (kernel_mv_data[mv_idx][pixel_index - 1] == 0)
                    noise = 0;

                if (noise == 1)
                    kernel_mv_data[mv_idx][pixel_index] = 1;
            }
        }
    }

#if M_DEBUG
    printf("mb_info\n");
    for (a = 0; a < mb_h_num; a++) {
        for (b = 0; b < mb_w_num; b++) {
            printf("%d",kernel_mv_data[mv_idx][a * mb_w_num + b]);
        }
        printf("\n");
    }    
#endif
    
    return ret;
}

static int transform(struct mv_cfg_t *mv_param, unsigned int u_width, unsigned int u_height,
                     unsigned int u_mb_width, unsigned int u_mb_height, unsigned int sensitive_th,
                     unsigned int frame_count, unsigned int mb_w_num, unsigned int mb_h_num,
                     int mv_idx)
{
    int h, w, a, b, ret = 0;
    int k_height, k_width;
    int k_mb_height, k_mb_width;
    
    if (is_parsing_init_ok[mv_idx] == MD_S_NOT_READY) {
        printf("parsing not initial\n");
        return -1;
    }
    
    if (transform_matrix[mv_idx] == NULL)
        return -1;

    k_height = mv_param->k_height;
    k_width  = mv_param->k_width;
    k_mb_height = mv_param->k_mb_height;
    k_mb_width  = mv_param->k_mb_width;
  
    if (frame_count == 0) {
        
        g_blk_w_num[mv_idx] = mb_w_num;
        g_blk_h_num[mv_idx] = mb_h_num;
        g_user_width[mv_idx]  = g_blk_w_num[mv_idx] * u_mb_width;
        g_user_height[mv_idx] = g_blk_h_num[mv_idx] * u_mb_height;
        g_mb_w_num[mv_idx] = k_width / k_mb_width;
        g_mb_h_num[mv_idx] = k_height / k_mb_height;
        
        for (h = 0; h < g_blk_h_num[mv_idx]; h++) {
            for (w = 0; w < g_blk_w_num[mv_idx]; w++) {
            int start_x = ((w * u_mb_width) * k_width) / 
                          (g_user_width[mv_idx] * k_mb_width);
            int end_x   = (((w + 1) * u_mb_width - 1) * k_width) / 
                          (g_user_width[mv_idx] * k_mb_width);               
            int start_y = ((h * u_mb_height) * k_height) / 
                          (g_user_height[mv_idx] * k_mb_height);
            int end_y   = (((h + 1) * u_mb_height - 1) * k_height) / 
                          (g_user_height[mv_idx] * k_mb_height);
      
            transform_matrix[mv_idx][(h * g_blk_w_num[mv_idx] + w) * 4 + 0] = start_x;
            transform_matrix[mv_idx][(h * g_blk_w_num[mv_idx] + w) * 4 + 1] = end_x;
            transform_matrix[mv_idx][(h * g_blk_w_num[mv_idx] + w) * 4 + 2] = start_y;
            transform_matrix[mv_idx][(h * g_blk_w_num[mv_idx] + w) * 4 + 3] = end_y;
            }
        }
    }

    for (h = 0; h < g_blk_h_num[mv_idx]; h++) {
        for (w = 0; w < g_blk_w_num[mv_idx]; w++) {
            int pixel_pos = h * g_blk_w_num[mv_idx] + w;
            int pixel_pos4 = pixel_pos << 2;
            int start_x = transform_matrix[mv_idx][pixel_pos4 + 0];
            int end_x   = transform_matrix[mv_idx][pixel_pos4 + 1];
            int start_y = transform_matrix[mv_idx][pixel_pos4 + 2];
            int end_y   = transform_matrix[mv_idx][pixel_pos4 + 3];
            int mb_num = 0;
            int mb_count = 0;
      
            for (a = start_y; a <= end_y; a++) {
                for (b = start_x; b <= end_x; b++) {
                    if (kernel_mv_data[mv_idx][a * g_mb_w_num[mv_idx] + b] == 0)
                        mb_count++;
                    mb_num++;
                }
            }
    
            if ( (mb_count * 100) > ((100 - sensitive_th) * mb_num) ) {
                mv_param->active_flag[pixel_pos] = 0;
                mv_param->active_num++;
            } else {
                mv_param->active_flag[pixel_pos] = 1;
            }
        }
    }

#if 0
    printf("blk_info\n");
    for (a = 0; a < g_blk_h_num[mv_idx]; a++) {
        for (b = 0; b < g_blk_w_num[mv_idx]; b++) {
            printf("%d",mv_param->active_flag[a * g_blk_w_num[mv_idx] + b]);
        }
        printf("\n");
    }    
#endif
    
    return ret;
}

int parsing_mv_data(unsigned char *event_buf, struct mv_cfg_t *mv_param,  unsigned int u_width, 
                    unsigned int u_height, unsigned int u_mb_width, unsigned int u_mb_height, 
                    unsigned int sensitive_th, unsigned int frame_count, unsigned int mb_w_num,
                    unsigned int mb_h_num, int mv_idx)
{
    int ret = 0;

    ret = parsing_mv(event_buf, mv_param, mv_idx);
    if (ret < 0) {
        printf("parsing mv error\n");
        return -1;
    }

    ret = transform(mv_param, u_width, u_height, u_mb_width, u_mb_height, sensitive_th,
                    frame_count, mb_w_num, mb_h_num, mv_idx);
    if (ret < 0) {
        printf("transform error\n");
        return -1;
    }
    
    return 0;
}

int parsing_mv_initial(unsigned int mb_w_num, unsigned int mb_h_num, int mv_idx)
{
    int blk_w_num;
    int blk_h_num;
    int user_motion_size_in;

    blk_w_num = mb_w_num;
    blk_h_num = mb_h_num;
    user_motion_size_in = blk_w_num * blk_h_num;
    if (parsing_user_motion_size[mv_idx] < user_motion_size_in) {
        if (parsing_user_motion_size[mv_idx] != 0) {
            free(transform_matrix[mv_idx]);
            free(kernel_mv_data[mv_idx]);
        }
        transform_matrix[mv_idx] = (int *)malloc(sizeof(int) * user_motion_size_in * 4);
        if (transform_matrix[mv_idx] == NULL)
            return -1;
    
        kernel_mv_data[mv_idx] = (unsigned char *)malloc(sizeof(unsigned char) * 
                                                         CAP_MOTION_SIZE);
        if (kernel_mv_data[mv_idx] == NULL)
            return -1;
        parsing_user_motion_size[mv_idx] = user_motion_size_in;
    }

    memset(transform_matrix[mv_idx],0,(sizeof(int) * user_motion_size_in * 4));
    memset(kernel_mv_data[mv_idx],0,(sizeof(unsigned char) * CAP_MOTION_SIZE));

    g_blk_w_num[mv_idx] = 0; 
    g_blk_h_num[mv_idx] = 0; 
    g_user_width[mv_idx] = 0; 
    g_user_height[mv_idx] = 0; 
    g_mb_w_num[mv_idx] = 0; 
    g_mb_h_num[mv_idx] = 0;
   
    is_parsing_init_ok[mv_idx] = MD_S_INIT_OK;
  
    return 0;
}

int parsing_mv_end(struct mv_cfg_t *mv_param, int mv_idx)
{
    is_parsing_init_ok[mv_idx] = MD_S_NOT_READY;

    if (transform_matrix[mv_idx] != NULL) {
        free(transform_matrix[mv_idx]);
        transform_matrix[mv_idx] = NULL;
    }

    if (kernel_mv_data[mv_idx] != NULL) {
        free(kernel_mv_data[mv_idx]);
        kernel_mv_data[mv_idx]  = NULL;
    }

    parsing_user_motion_size[mv_idx] = 0;

    return 0;
}

