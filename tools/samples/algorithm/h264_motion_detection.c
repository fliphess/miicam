/**
 * @file h264_motion_detection.c
 *  Motion detection algorithm by H.264 motion data.
 * Copyright (C) 2011 GM Corp. (http://www.grain-media.com)
 *
 * $Revision: 1.1 $
 *
 * $Date: 2013/11/08 08:19:33 $
 * 
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory.h>
#include "h264_motion_detection.h"

#define abs_c(X)    (((X)>0)?(X):-(X))

int favc_motion_info_init(struct md_cfg *md_param, struct md_res *active, struct favc_md *favcmd,
        unsigned int mb_width, unsigned int mb_height)
{
    printf("\nMD_INIT.\n");

    favcmd->mb_width = mb_width;
    favcmd->mb_height = mb_height;

    favcmd->mvs0_x = NULL;
    favcmd->mvs0_y = NULL;
    favcmd->mvs1_x = NULL;
    favcmd->mvs1_y = NULL;
    favcmd->l_mvsp = NULL;
    favcmd->IMB_flag = NULL;
    favcmd->l_mvsi = NULL;
    active->active_flag = NULL; //I and P MB active
    md_param->mb_cell_en = NULL;
    md_param->mb_cell_th = NULL;

    favcmd->mvs0_x = (int *)malloc(sizeof(int) * mb_width * mb_height);
    favcmd->mvs0_y = (int *)malloc(sizeof(int) * mb_width * mb_height);
    favcmd->mvs1_x = (int *)malloc(sizeof(int) * mb_width * mb_height);
    favcmd->mvs1_y = (int *)malloc(sizeof(int) * mb_width * mb_height);
    favcmd->l_mvsp = (unsigned int *)malloc(sizeof(unsigned int) * mb_width * mb_height);
    favcmd->IMB_flag = (unsigned int *)malloc(sizeof(unsigned int) * mb_width * mb_height);
    favcmd->l_mvsi = (unsigned int *)malloc(sizeof(unsigned int) * mb_width * mb_height);
    active->active_flag = (unsigned char *)malloc(sizeof(unsigned char) * mb_width * mb_height);
    //< I and P MB active
    md_param->mb_cell_en = (unsigned char *)malloc(sizeof(unsigned char) * mb_width * mb_height);
    md_param->mb_cell_th = (unsigned char *)malloc(sizeof(unsigned char) * mb_width * mb_height);

    if ((favcmd->mvs0_x == NULL) || (favcmd->mvs0_y == NULL) || (favcmd->mvs1_x == NULL) ||
        (favcmd->mvs1_y == NULL))
        return 1;

    if ((favcmd->l_mvsp == NULL) || (favcmd->IMB_flag == NULL) || (favcmd->l_mvsi == NULL))
        return 1;

    if (active->active_flag == NULL)
        return 1;

    if ((md_param->mb_cell_en == NULL) || (md_param->mb_cell_th == NULL))
        return 1;

    memset(favcmd->mvs0_x, 0, (sizeof(int) * mb_width * mb_height));
    memset(favcmd->mvs0_y, 0, (sizeof(int) * mb_width * mb_height));
    memset(favcmd->mvs1_x, 0, (sizeof(int) * mb_width * mb_height));
    memset(favcmd->mvs1_y, 0, (sizeof(int) * mb_width * mb_height));
    memset(favcmd->l_mvsp, 0, (sizeof(unsigned int) * mb_width * mb_height));
    memset(favcmd->IMB_flag, 0, (sizeof(unsigned int) * mb_width * mb_height));
    memset(favcmd->l_mvsi, 0, (sizeof(unsigned int) * mb_width * mb_height));
    memset(active->active_flag, 0, (sizeof(unsigned char) * mb_width * mb_height));
    //< I and P MB active
    memset(md_param->mb_cell_en, 0, (sizeof(unsigned char) * mb_width * mb_height));
    memset(md_param->mb_cell_th, 0, (sizeof(unsigned char) * mb_width * mb_height));

    favcmd->sad16_cur = 0;
    favcmd->sad16_pre = 0;
    favcmd->sad16_Th = 0;
    favcmd->sad_mask = 0;
    md_param->usesad = 0;
    md_param->interlace_mode = 0;

    return 0;
}

//----------------------------------------------------------------------------
int favc_motion_info_end(struct md_cfg *md_param, struct md_res *active, struct favc_md *favcmd)
{
    printf("\nMD_END.\n");
    if (favcmd->mvs0_x != NULL)
        free(favcmd->mvs0_x);

    if (favcmd->mvs0_y != NULL)
        free(favcmd->mvs0_y);

    if (favcmd->mvs1_x != NULL)
        free(favcmd->mvs1_x);

    if (favcmd->mvs1_y != NULL)
        free(favcmd->mvs1_y);

    if (favcmd->l_mvsp != NULL)
        free(favcmd->l_mvsp);

    if (favcmd->IMB_flag != NULL)
        free(favcmd->IMB_flag);

    if (favcmd->l_mvsi != NULL)
        free(favcmd->l_mvsi);

    if (active->active_flag != NULL)
        free(active->active_flag);

    if (md_param->mb_cell_en != NULL)
        free(md_param->mb_cell_en);

    if (md_param->mb_cell_th != NULL)
        free(md_param->mb_cell_th);

    return 0;
}

//----------------------------------------------------------------------------
int favc_motion_detection(unsigned char *mb_array, struct md_cfg *md_param, struct md_res *active,
                          struct favc_md *favcmd)
{
    unsigned int x_pos, y_pos;
    unsigned int sad16 = 0, sad16h = 0, sad16l = 0;

    int mv0_x = 0, mv0_y = 0, mv1_x = 0, mv1_y = 0; //chri
    int mv0_odd_x, mv0_odd_y, mv1_odd_x, mv1_odd_y;
    unsigned int mv_thd = 0;
    unsigned int type1_odd_field = 0, type2_odd_field = 0;
    unsigned int mv_even_field = 0, mv_odd_field = 0;

    unsigned int *mb;
    unsigned int mb10 = 0, mb11 = 0;
    unsigned int type1 = 0, type2 = 0;
    //unsigned int *mb_odd_field;
    unsigned int sad16_odd_l, sad16_odd_h, sad16_odd_field = 0;

    int sad_diff = 0;
    unsigned int mbwidth, mbheight;

    active->active_num = 0;
    mbwidth = favcmd->mb_width;
    mbheight = favcmd->mb_height;
    //printf("md_param.interlace_mode = %d\n", md_param->interlace_mode);
    //printf("md_param.usesad = %d\n", md_param->usesad);
    //printf("md_param.mb_time_th = %d\n", md_param->mb_time_th);
    //printf("md_param.alarm_th = %d\n", md_param->alarm_th);

    if (md_param->interlace_mode) {
        //printf("interlace mode\n");
        for (y_pos = 0; y_pos < ((mbheight - 1) >> 1); y_pos++) {
            for (x_pos = 0; x_pos < mbwidth; x_pos++) {
                mb = (unsigned int *)(mb_array + (y_pos * mbwidth + x_pos) * 64);
                mb10 = mb[10];
                mb11 = mb[11];

                mv0_x = ((int)((mb10 & 0x0000007F) << 25)) >> 25;
                mv0_y = ((int)((mb10 & 0x00007F00) << 17)) >> 25;
                mv1_x = ((int)((mb11 & 0x0000007F) << 25)) >> 25;
                mv1_y = ((int)((mb11 & 0x00007F00) << 17)) >> 25;

                mv_even_field = (mv0_x * mv0_x + mv0_y * mv0_y) + (mv1_x * mv1_x +  mv1_y * mv1_y);

                if (md_param->usesad) {
                    sad16l = ((mb10 & 0x0FF00000) >> 20);
                    sad16h = ((mb11 & 0x0FF00000) >> 20);
                    sad16 = sad16l + (sad16h << 8);
                }
                type1 = mb10 & 0xF0000000;
                type2 = mb11 & 0xF0000000;

                mb =
                    (unsigned int *)(mb_array + ((y_pos + (mbheight >> 1)) * mbwidth + x_pos) * 64);
                mb10 = mb[10];
                mb11 = mb[11];

                mv0_odd_x = ((int)((mb10 & 0x0000007F) << 25)) >> 25;
                mv0_odd_y = ((int)((mb10 & 0x00007F00) << 17)) >> 25;
                mv1_odd_x = ((int)((mb11 & 0x0000007F) << 25)) >> 25;
                mv1_odd_y = ((int)((mb11 & 0x00007F00) << 17)) >> 25;

                mv_odd_field = (mv0_odd_x * mv0_odd_x + mv0_odd_y * mv0_odd_y) +
                    (mv1_odd_x * mv1_odd_x + mv1_odd_y * mv1_odd_y);

                if (md_param->usesad) {
                    sad16_odd_l = ((mb10 & 0x0FF00000) >> 20);
                    sad16_odd_h = ((mb11 & 0x0FF00000) >> 20);
                    sad16_odd_field = sad16_odd_l + (sad16_odd_h << 8);
                }

                type1_odd_field = mb10 & 0xF0000000;
                type2_odd_field = mb11 & 0xF0000000;

                //even_field
                if ((type1 == 0x50000000) || (type1 == 0x60000000)) { //I MB
                    favcmd->mvs0_x[((y_pos << 1) * mbwidth + x_pos)] =
                        (favcmd->mvs0_x[((y_pos << 1) * mbwidth + x_pos)] << 1) / 3;
                    favcmd->mvs0_y[((y_pos << 1) * mbwidth + x_pos)] =
                        (favcmd->mvs0_y[((y_pos << 1) * mbwidth + x_pos)] << 1) / 3;
                    favcmd->mvs1_x[((y_pos << 1) * mbwidth + x_pos)] =
                        (favcmd->mvs1_x[((y_pos << 1) * mbwidth + x_pos)] << 1) / 3;
                    favcmd->mvs1_y[((y_pos << 1) * mbwidth + x_pos)] =
                        (favcmd->mvs1_y[((y_pos << 1) * mbwidth + x_pos)] << 1) / 3;
                    favcmd->IMB_flag[((y_pos << 1) * mbwidth + x_pos)] += 1;
                } else {  // P MB
                    favcmd->mvs0_x[((y_pos << 1) * mbwidth + x_pos)] =
                        ((favcmd->mvs0_x[((y_pos << 1) * mbwidth + x_pos)] << 1) + mv0_x) / 3;
                    favcmd->mvs0_y[((y_pos << 1) * mbwidth + x_pos)] =
                        ((favcmd->mvs0_y[((y_pos << 1) * mbwidth + x_pos)] << 1) + mv0_y) / 3;
                    favcmd->mvs1_x[((y_pos << 1) * mbwidth + x_pos)] =
                        ((favcmd->mvs1_x[((y_pos << 1) * mbwidth + x_pos)] << 1) + mv1_x) / 3;
                    favcmd->mvs1_y[((y_pos << 1) * mbwidth + x_pos)] =
                        ((favcmd->mvs1_y[((y_pos << 1) * mbwidth + x_pos)] << 1) + mv1_y) / 3;
                    favcmd->IMB_flag[((y_pos << 1) * mbwidth + x_pos)] = 0;

                    if (mv_even_field == 0 && (md_param->usesad))
                        favcmd->sad16_cur = ((favcmd->sad16_cur * 3) + sad16) >> 2;
                }

                mv_thd = md_param->mb_cell_th[((y_pos << 1) * mbwidth + x_pos)];

                if (((unsigned int)(favcmd->mvs0_x[((y_pos << 1) * mbwidth + x_pos)] * 
                    favcmd->mvs0_x[((y_pos << 1) * mbwidth + x_pos)] + favcmd->mvs0_y[((y_pos << 1)
                    * mbwidth + x_pos)] * favcmd->mvs0_y[((y_pos << 1) * mbwidth + x_pos)]) > 
                        mv_thd) && ((unsigned int)(favcmd->mvs1_x[((y_pos << 1) * mbwidth + x_pos)] * 
                    favcmd->mvs1_x[((y_pos << 1) * mbwidth + x_pos)] + favcmd->mvs1_y[((y_pos << 1)
                    * mbwidth + x_pos)] * favcmd->mvs1_y[((y_pos << 1) * mbwidth + x_pos)]) > 
                    mv_thd))
                    favcmd->l_mvsp[((y_pos << 1) * mbwidth + x_pos)] = 1;
                else
                    favcmd->l_mvsp[((y_pos << 1) * mbwidth + x_pos)] = 0;

                //odd_field
                if ((type1_odd_field == 0x50000000) || (type1_odd_field == 0x60000000)) { //I MB
                    favcmd->mvs0_x[(((y_pos << 1) + 1) * mbwidth + x_pos)] =
                        (favcmd->mvs0_x[(((y_pos << 1) + 1) * mbwidth + x_pos)] << 1) / 3;
                    favcmd->mvs0_y[(((y_pos << 1) + 1) * mbwidth + x_pos)] =
                        (favcmd->mvs0_y[(((y_pos << 1) + 1) * mbwidth + x_pos)] << 1) / 3;
                    favcmd->mvs1_x[(((y_pos << 1) + 1) * mbwidth + x_pos)] =
                        (favcmd->mvs1_x[(((y_pos << 1) + 1) * mbwidth + x_pos)] << 1) / 3;
                    favcmd->mvs1_y[(((y_pos << 1) + 1) * mbwidth + x_pos)] =
                        (favcmd->mvs1_y[(((y_pos << 1) + 1) * mbwidth + x_pos)] << 1) / 3;
                    favcmd->IMB_flag[(((y_pos << 1) + 1) * mbwidth + x_pos)] += 1;
                } else {  // P MB
                    favcmd->mvs0_x[(((y_pos << 1) + 1) * mbwidth + x_pos)] =
                        ((favcmd->mvs0_x[(((y_pos << 1) + 1) * mbwidth + x_pos)] << 1) +
                        mv0_odd_x) / 3;
                    favcmd->mvs0_y[(((y_pos << 1) + 1) * mbwidth + x_pos)] =
                        ((favcmd->mvs0_y[(((y_pos << 1) + 1) * mbwidth + x_pos)] << 1) +
                        mv0_odd_y) / 3;
                    favcmd->mvs1_x[(((y_pos << 1) + 1) * mbwidth + x_pos)] =
                        ((favcmd->mvs1_x[(((y_pos << 1) + 1) * mbwidth + x_pos)] << 1) +
                        mv1_odd_x) / 3;
                    favcmd->mvs1_y[(((y_pos << 1) + 1) * mbwidth + x_pos)] =
                        ((favcmd->mvs1_y[(((y_pos << 1) + 1) * mbwidth + x_pos)] << 1) +
                        mv1_odd_y) / 3;
                    favcmd->IMB_flag[(((y_pos << 1) + 1) * mbwidth + x_pos)] = 0;

                    if (mv_odd_field == 0 && (md_param->usesad))
                        favcmd->sad16_cur = ((favcmd->sad16_cur * 3) + sad16_odd_field) >> 2;
                }

                mv_thd = md_param->mb_cell_th[(((y_pos << 1) + 1) * mbwidth + x_pos)];


                if (((unsigned int)(favcmd->mvs0_x[(((y_pos << 1) + 1) * mbwidth + x_pos)] *
                        favcmd->mvs0_x[(((y_pos << 1) + 1) * mbwidth + x_pos)] +
                        favcmd->mvs0_y[(((y_pos << 1) + 1) * mbwidth + x_pos)] *
                        favcmd->mvs0_y[(((y_pos << 1) + 1) * mbwidth + x_pos)]) > mv_thd)
                    && ((unsigned int)(favcmd->mvs1_x[(((y_pos << 1) + 1) * mbwidth + x_pos)] *
                    favcmd->mvs1_x[(((y_pos << 1) + 1) * mbwidth + x_pos)] +
                    favcmd->mvs1_y[(((y_pos << 1) + 1) * mbwidth + x_pos)] *
                    favcmd->mvs1_y[(((y_pos << 1) + 1) * mbwidth + x_pos)]) > mv_thd))
                    favcmd->l_mvsp[(((y_pos << 1) + 1) * mbwidth + x_pos)] = 1;
                else
                    favcmd->l_mvsp[(((y_pos << 1) + 1) * mbwidth + x_pos)] = 0;

                if (md_param->usesad)
                    favcmd->sad16_Th = favcmd->sad16_cur + 50;  //sad16_avc_Th

                if (md_param->usesad) {
                    if ((favcmd->IMB_flag[((y_pos << 1) * mbwidth + x_pos)] >= 
                            md_param->mb_time_th) && (sad16 >= favcmd->sad16_Th))
                        favcmd->l_mvsi[((y_pos << 1) * mbwidth + x_pos)] = md_param->mb_time_th;
                    else
                        favcmd->l_mvsi[((y_pos << 1) * mbwidth + x_pos)] = 0;

                    if ((favcmd->IMB_flag[(((y_pos << 1) + 1) * mbwidth + x_pos)] >= 
                            md_param->mb_time_th) && (sad16 >= favcmd->sad16_Th))
                        favcmd->l_mvsi[(((y_pos << 1) + 1) * mbwidth + x_pos)] = 
                        md_param->mb_time_th;
                    else
                        favcmd->l_mvsi[(((y_pos << 1) + 1) * mbwidth + x_pos)] = 0;

                    if (favcmd->l_mvsp[((y_pos << 1) * mbwidth + x_pos)] && 
                        (sad16 < favcmd->sad16_Th))
                        favcmd->l_mvsp[((y_pos << 1) * mbwidth + x_pos)] = 0;

                    if (favcmd->l_mvsp[(((y_pos << 1) + 1) * mbwidth + x_pos)] &&
                        (sad16 < favcmd->sad16_Th))
                        favcmd->l_mvsp[(((y_pos << 1) + 1) * mbwidth + x_pos)] = 0;


                } else {
                    if (favcmd->IMB_flag[((y_pos << 1) * mbwidth + x_pos)] >= md_param->mb_time_th)
                        favcmd->l_mvsi[((y_pos << 1) * mbwidth + x_pos)] = md_param->mb_time_th;
                    else
                        favcmd->l_mvsi[((y_pos << 1) * mbwidth + x_pos)] = 0;
                    if (favcmd->IMB_flag[(((y_pos << 1) + 1) * mbwidth + x_pos)] >=
                                md_param->mb_time_th)
                        favcmd->l_mvsi[(((y_pos << 1) + 1) * mbwidth + x_pos)] =
                                                                    md_param->mb_time_th;
                    else
                        favcmd->l_mvsi[(((y_pos << 1) + 1) * mbwidth + x_pos)] = 0;
                }
            }
        }
    } else { //non-interlaced
             //printf("non-interlace mode\n");
        for (y_pos = 0; y_pos < (mbheight - 1); y_pos++) {
            for (x_pos = 0; x_pos < mbwidth; x_pos++) {		
            		mb = (unsigned int *)(mb_array + (y_pos * mbwidth + x_pos) * 8);
            		mb10 = mb[1];
            		mb11 = mb[1];
            		
            		mv0_x = ((int)((mb10 & 0x000000FF) << 24)) >> 24;
                    mv0_y = ((int)((mb10 & 0x0001FE00) << 15)) >> 24;
                    mv1_x = ((int)((mb10 & 0x000000FF) << 24)) >> 24;
                    mv1_y = ((int)((mb10 & 0x0001FE00) << 15)) >> 24;
            		
            		mv_even_field = (mv0_x * mv0_x + mv0_y * mv0_y) + (mv1_x * mv1_x +  mv1_y * mv1_y);
            		
            		type1 = mb10 >> 31 ;
            		
            		if (md_param->usesad) {
                    sad16 = ((mb10 & 0x7FFC0000) << 1) >> 19 ;
                    //printf("sad16 = %d", sad16);
                }
                
                if ( type1 == 0 ) { //I MB
                    favcmd->mvs0_x[(y_pos * mbwidth + x_pos)] =
                        (favcmd->mvs0_x[(y_pos * mbwidth + x_pos)] << 1) / 3;
                    favcmd->mvs0_y[(y_pos * mbwidth + x_pos)] =
                        (favcmd->mvs0_y[(y_pos * mbwidth + x_pos)] << 1) / 3;
                    favcmd->mvs1_x[(y_pos * mbwidth + x_pos)] =
                        (favcmd->mvs1_x[(y_pos * mbwidth + x_pos)] << 1) / 3;
                    favcmd->mvs1_y[(y_pos * mbwidth + x_pos)] =
                        (favcmd->mvs1_y[(y_pos * mbwidth + x_pos)] << 1) / 3;
                    favcmd->IMB_flag[(y_pos * mbwidth + x_pos)] += 1;
                } else { // P MB
                    favcmd->mvs0_x[(y_pos * mbwidth + x_pos)] =
                        ((favcmd->mvs0_x[(y_pos * mbwidth + x_pos)] << 1) + mv0_x) / 3;
                    favcmd->mvs0_y[(y_pos * mbwidth + x_pos)] =
                        ((favcmd->mvs0_y[(y_pos * mbwidth + x_pos)] << 1) + mv0_y) / 3;
                    favcmd->mvs1_x[(y_pos * mbwidth + x_pos)] =
                        ((favcmd->mvs1_x[(y_pos * mbwidth + x_pos)] << 1) + mv1_x) / 3;
                    favcmd->mvs1_y[(y_pos * mbwidth + x_pos)] =
                        ((favcmd->mvs1_y[(y_pos * mbwidth + x_pos)] << 1) + mv1_y) / 3;
                    favcmd->IMB_flag[(y_pos * mbwidth + x_pos)] = 0;

                    if (mv_even_field == 0 && (md_param->usesad)) {
                        if (sad16)
                            favcmd->sad16_cur = ((favcmd->sad16_cur * 3) + sad16) >> 2;
                    }
                }
                
               	mv_thd = md_param->mb_cell_th[(y_pos * mbwidth + x_pos)];
                //printf("line %d, pixel %d mv_thd = %d\n", y_pos , x_pos,  mv_thd);

                if (((unsigned int)(favcmd->mvs0_x[(y_pos * mbwidth + x_pos)] *
                    favcmd->mvs0_x[(y_pos * mbwidth + x_pos)] +
                    favcmd->mvs0_y[(y_pos * mbwidth + x_pos)] *
                    favcmd->mvs0_y[(y_pos * mbwidth + x_pos)]) > mv_thd)
                    && ((unsigned int)(favcmd->mvs1_x[(y_pos * mbwidth + x_pos)] *
                    favcmd->mvs1_x[(y_pos * mbwidth + x_pos)] +
                    favcmd->mvs1_y[(y_pos * mbwidth + x_pos)] *
                    favcmd->mvs1_y[(y_pos * mbwidth + x_pos)]) > mv_thd))
                    favcmd->l_mvsp[(y_pos * mbwidth + x_pos)] = 1;
                else
                    favcmd->l_mvsp[(y_pos * mbwidth + x_pos)] = 0;

                if (md_param->usesad)
                    favcmd->sad16_Th = favcmd->sad16_cur + 50;

                if (md_param->usesad) {
                    if ((favcmd->IMB_flag[(y_pos * mbwidth + x_pos)] >= md_param->mb_time_th) &&
                        (sad16 >= favcmd->sad16_Th))
                        favcmd->l_mvsi[(y_pos * mbwidth + x_pos)] = md_param->mb_time_th;
                    else
                        favcmd->l_mvsi[(y_pos * mbwidth + x_pos)] = 0;

                    if (favcmd->l_mvsp[(y_pos * mbwidth + x_pos)] && (sad16 < favcmd->sad16_Th))
                        favcmd->l_mvsp[(y_pos * mbwidth + x_pos)] = 0;

                } else {
                    if (favcmd->IMB_flag[(y_pos * mbwidth + x_pos)] >= md_param->mb_time_th)
                        favcmd->l_mvsi[(y_pos * mbwidth + x_pos)] = md_param->mb_time_th;
                    else
                        favcmd->l_mvsi[(y_pos * mbwidth + x_pos)] = 0;
                }
            }
        }
    }  //non-interlaced end

    favc_do_noise_filtering(favcmd->l_mvsp, mbwidth, mbheight - 1);
    favc_do_IMB_filtering(favcmd->l_mvsi, mbwidth, mbheight - 1);

    //**********color rolling prevent**********//
    if (md_param->usesad) {
        sad_diff = (int)(favcmd->sad16_cur - favcmd->sad16_pre);

        if (abs_c(sad_diff) > md_param->rolling_prot_th) {
            //printf("\n<color rolling %d %d>\n", favcmd->sad16_pre, favcmd->sad16_cur);
            favcmd->sad16_pre = favcmd->sad16_cur;
            favcmd->sad_mask = 1;
            return 0;
        } else {
            favcmd->sad16_pre = (favcmd->sad16_pre + favcmd->sad16_cur) >> 1;
        }

        if (favcmd->sad_mask) {
            //printf("\n<mask next frame>\n");
            favcmd->sad_mask = 0;
            return 0;
        }

    }
    //****************************************//

    for (y_pos = 0; y_pos < (mbheight - 1); y_pos++) {
        for (x_pos = 0; x_pos < mbwidth; x_pos++) {
            active->active_flag[(y_pos * mbwidth + x_pos)] = 0; //clear

            if ((favcmd->l_mvsp[(y_pos * mbwidth + x_pos)]) ||
                (favcmd->l_mvsi[(y_pos * mbwidth + x_pos)])) {
                if (md_param->mb_cell_en[(y_pos * mbwidth + x_pos)]) {
                    active->active_num++;
                    active->active_flag[(y_pos * mbwidth + x_pos)] = 1;
                }
            }

        }
    }

    return 0;
}

//----------------------------------------------------------------------------
void favc_do_noise_filtering(unsigned int *lmvs, unsigned int mbwidth, unsigned int mbheight)
{
    unsigned int x_pos, y_pos;
    unsigned int *l_mvs_ptr;
    l_mvs_ptr = lmvs - 1;

    for (y_pos = 0; y_pos < mbheight; y_pos++) {
        for (x_pos = 0; x_pos < mbwidth; x_pos++) {

            l_mvs_ptr++;

            if (*l_mvs_ptr == 0)
                continue;

            if (!((x_pos == 0) || (y_pos == 0)))
                if (*(l_mvs_ptr - mbwidth - 1))
                    continue;

            if (y_pos != 0)
                if (*(l_mvs_ptr - mbwidth))
                    continue;

            if (!((x_pos == (mbwidth - 1)) || (y_pos == 0)))
                if (*(l_mvs_ptr - mbwidth + 1))
                    continue;

            if (x_pos != 0)
                if (*(l_mvs_ptr - 1))
                    continue;

            if (x_pos != (mbwidth - 1))
                if (*(l_mvs_ptr + 1))
                    continue;

            if (!((x_pos == 0) || (y_pos == (mbheight - 1))))
                if (*(l_mvs_ptr + mbwidth - 1))
                    continue;

            if (y_pos != (mbheight - 1))
                if (*(l_mvs_ptr + mbwidth))
                    continue;

            if (!((x_pos == (mbwidth - 1)) || (y_pos == (mbheight - 1))))
                if (*(l_mvs_ptr + mbwidth + 1))
                    continue;

            *l_mvs_ptr = 0; //adjoining points are all zero

        }
    }
    return;
}

//----------------------------------------------------------------------------
void favc_do_IMB_filtering(unsigned int *lmvs, unsigned int mbwidth, unsigned int mbheight)
{
    unsigned int x_pos, y_pos;
    unsigned int *l_mvs_ptr;
    l_mvs_ptr = lmvs - 1;
    //int a, b, c, d, e, f, g, h;

    for (y_pos = 0; y_pos < mbheight; y_pos++) {
        for (x_pos = 0; x_pos < mbwidth; x_pos++) {

            l_mvs_ptr++;
            if ((x_pos == 0) || (x_pos == (mbwidth - 1)) ||
                (y_pos == 0) || (y_pos == (mbheight - 1))) {
                *l_mvs_ptr = 0; //edge points clear to zero
                continue;
            }

            if (*l_mvs_ptr == 0)
                continue;

            if (*(l_mvs_ptr - 1))
                if (*(l_mvs_ptr - mbwidth - 1))
                    if (*(l_mvs_ptr - mbwidth))
                        continue;

            if (*(l_mvs_ptr - mbwidth))
                if (*(l_mvs_ptr - mbwidth + 1))
                    if (*(l_mvs_ptr + 1))
                        continue;

            if (*(l_mvs_ptr + 1))
                if (*(l_mvs_ptr + mbwidth + 1))
                    if (*(l_mvs_ptr + mbwidth))
                        continue;

            if (*(l_mvs_ptr + mbwidth))
                if (*(l_mvs_ptr + mbwidth - 1))
                    if (*(l_mvs_ptr - 1))
                        continue;

            *l_mvs_ptr = 0; //adjoin points are all zero

        }
    }
    return;
}

