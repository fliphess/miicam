/**
 * @file encode_with_capture_tamper_detection.c
 *
 * Support platform: GM8210, GM8287, GM8139
 * This sample code is demo tamper detection by capture motion data
 * 
 * Copyright (C) 2012 GM Corp. (http://www.grain-media.com)
 *
 * $Revision: 1.9 $
 * $Date: 2014/04/29 00:04:55 $
 *
 */
/**
 * @example encode_with_capture_tamper_detection.c
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <sys/time.h>
#include "gmlib.h"
#include "algorithm/capture_tamper_detection.c"

#define BITSTREAM_LEN       (720 * 576 * 3 / 2)
#define MD_DATA_LEN         (720 * 576 / 4)
#define MAX_CH_NUM          1
#define MAX_BITSTREAM_NUM   1

gm_system_t gm_system;
void *groupfd;       // return of gm_new_groupfd()
void *bindfd[MAX_CH_NUM];        // return of gm_bind()
void *capture_object;
void *encode_object[MAX_CH_NUM];
pthread_t enc_thread_id = 0, motion_thread_id = 0;
int enc_exit = 0;   // Notify program exit
unsigned int dest_width = 0, dest_height = 0;

/*** tamper detection global variable */
struct tp_alg_t tp_alg;
struct tp_result_t tp_result[MAX_CH_NUM];

static void set_tamper_param(int ch)
{
    tp_alg.u_width = gm_system.cap[ch].dim.width;
    tp_alg.u_height = gm_system.cap[ch].dim.height;
    tp_alg.u_mb_width = 32;
    tp_alg.u_mb_height = 32;
    tp_alg.training_time = 15;
    tp_alg.frame_count = 0;
    tp_alg.sensitive_th = 80;
    tp_alg.mb_w_num   = (tp_alg.u_width + (tp_alg.u_mb_width - 1)) / tp_alg.u_mb_width;
    tp_alg.mb_h_num   = (tp_alg.u_height + (tp_alg.u_mb_height - 1)) / tp_alg.u_mb_height;
    tamper_detection_update(bindfd[ch], &tp_alg);
}

static void *motion_thread(void *arg)
{
    int ch;
    int ret;
    gm_multi_cap_md_t *cap_md = NULL;
   
    cap_md = (gm_multi_cap_md_t *) malloc(sizeof(gm_multi_cap_md_t) * MAX_CH_NUM);
    if (cap_md == NULL) {
        printf("Error to allocate capture motion info!\n");
        goto thread_exit;
    }
    memset((void *) cap_md, 0, (sizeof(gm_multi_cap_md_t) * MAX_CH_NUM));
    for (ch = 0; ch < MAX_CH_NUM; ch++) {
        cap_md[ch].bindfd = bindfd[ch];
        cap_md[ch].cap_md_info.md_buf_len = CAP_MOTION_SIZE;
        cap_md[ch].cap_md_info.md_buf = (char *) malloc(CAP_MOTION_SIZE);
        if (cap_md[ch].cap_md_info.md_buf == NULL) {
            printf("Error to allocate capture motion buffer!\n");
            goto thread_exit;
        }
    } 
    
    while (enc_exit == 0) {
        ret = gm_recv_multi_cap_md(cap_md, MAX_CH_NUM);
        if (ret < 0) { //-1:error,0:sucess
            printf("Error parameter to get motion data\n");
            continue; 
        }
        ret = tamper_detection_handling(cap_md, &tp_result[0], MAX_CH_NUM);
        if (ret < 0) { //-1:error,0:sucess
            printf("Error to do tamper_detection_handling\n");
            goto thread_exit;
        }
        for (ch = 0; ch < MAX_CH_NUM; ch++) {
            if (tp_result[ch].result == TAMPER_DETECTED) {
                printf("[---Tamper Detected---at CH(%d)]\n", ch);
            } else if (tp_result[ch].result == NO_TAMPER) {
                //No Tamper Detected
            } else if (tp_result[ch].result == TAMPER_ALGO_ERROR) {
                printf("[---Tamper Algorithm Error---at CH(%d)]\n", ch);
            } else if (tp_result[ch].result == MOTION_PARSING_ERROR) {
                printf("[---Paring data error--- at CH(%d)]\n", ch);
            } else {
                printf("[---Tamper Message not Defined Error--- at CH(%d)]\n", ch);
            }
        }
        usleep(200000);   //two second period to detect motion
    }
   
thread_exit:

    if (cap_md) {
        for (ch = 0 ; ch < MAX_CH_NUM; ch++) {
            if (cap_md[ch].cap_md_info.md_buf)
                free(cap_md[ch].cap_md_info.md_buf);
        }
    }
    if (cap_md)
        free(cap_md);

    tamper_detection_end();
    
    return 0;
}


static void *encode_thread(void *arg)
{
    int i, ret;
    int ch = (int)arg;
    FILE *record_file;
    char filename[50];
    char *bitstream_data;
    gm_pollfd_t poll_fds[MAX_BITSTREAM_NUM];
    gm_enc_multi_bitstream_t multi_bs[MAX_BITSTREAM_NUM];
    
    // open file for write(打开录相文件)
    bitstream_data = (char *)malloc(BITSTREAM_LEN);
    memset(bitstream_data, 0, BITSTREAM_LEN);

    sprintf(filename, "ch%d_%dx%d_cap_motion.264", ch, gm_system.cap[ch].dim.width,
        gm_system.cap[ch].dim.height);
    record_file = fopen(filename, "wb");
    if (record_file == NULL) {
        printf("open file error(%s)! \n", filename);
        exit(1);
    }

    memset(poll_fds, 0, sizeof(poll_fds));

    poll_fds[ch].bindfd = bindfd[ch];
    poll_fds[ch].event = GM_POLL_READ;
    while (enc_exit == 0) {
        /** poll bitstream until 500ms timeout */
        ret = gm_poll(poll_fds, MAX_BITSTREAM_NUM, 500);
        if (ret == GM_TIMEOUT) {
            printf("Poll timeout!!");
            continue;
        }

        memset(multi_bs, 0, sizeof(multi_bs));  //clear all mutli bs
        for (i = 0; i < MAX_BITSTREAM_NUM; i++) {
            if (poll_fds[i].revent.event != GM_POLL_READ)
                continue;
            if (poll_fds[i].revent.bs_len > BITSTREAM_LEN) {
                printf("bitstream buffer length is not enough! %d, %d\n",
                    poll_fds[i].revent.bs_len, BITSTREAM_LEN);
                continue;
            }
            multi_bs[i].bindfd = poll_fds[i].bindfd;
            multi_bs[i].bs.bs_buf = bitstream_data;  // set buffer point(指定输出缓冲指针位置)
            multi_bs[i].bs.bs_buf_len = BITSTREAM_LEN;  // set buffer length(指定缓冲长度)
            multi_bs[i].bs.mv_buf = 0;  // not to recevie MV data
            multi_bs[i].bs.mv_buf_len = 0;  // not to recevie MV data
        }
        
        if ((ret = gm_recv_multi_bitstreams(multi_bs, MAX_BITSTREAM_NUM)) < 0) {
            printf("Error return value %d\n", ret);
        } else {
            for (i = 0; i < MAX_BITSTREAM_NUM; i++) {
                if ((multi_bs[i].retval < 0) && multi_bs[i].bindfd) {
                    printf("CH%d Error to receive bitstream. ret=%d\n", i, multi_bs[i].retval);
                } else if (multi_bs[i].retval == GM_SUCCESS) {
#if 0
                    printf("<CH%d, mv_len=%d bs_len=%d, keyframe=%d, newbsflag=0x%x>\n",
                            i, multi_bs[i].bs.mv_len, multi_bs[i].bs.bs_len,
                            multi_bs[i].bs.keyframe, multi_bs[i].bs.newbs_flag);
#endif
                    fwrite(multi_bs[i].bs.bs_buf,1, multi_bs[i].bs.bs_len, record_file);
                    fflush(record_file);
                }
            }
        }
    }

    fclose(record_file);
    free(bitstream_data);
    return 0;
}

int main(void)
{
    int ch, ret = 0;
    int key;
    DECLARE_ATTR(cap_attr, gm_cap_attr_t);
    DECLARE_ATTR(h264e_attr, gm_h264e_attr_t);

    gm_init(); //gmlib initial(GM库初始化)
    gm_get_sysinfo(&gm_system);
    
    groupfd = gm_new_groupfd(); // create new record group fd (获取groupfd)

    ch = 0; // use capture virtual channel 0
    capture_object = gm_new_obj(GM_CAP_OBJECT); // new capture object(获取捕捉对象)
    encode_object[ch] = gm_new_obj(GM_ENCODER_OBJECT);  // create encoder object (获取编码对象)
    cap_attr.cap_vch = ch;

    //GM8210 capture path 0(liveview), 1(CVBS), 2(can scaling), 3(can scaling)
    //GM8139/GM8287 capture path 0(liveview), 1(CVBS), 2(can scaling), 3(can't scaling down)
    cap_attr.path = 3;
    cap_attr.enable_mv_data = 1;
    gm_set_attr(capture_object, &cap_attr); // set capture attribute (设置捕捉属性)   

    dest_width = gm_system.cap[ch].dim.width;
    dest_height = gm_system.cap[ch].dim.height;

    h264e_attr.dim.width = dest_width;
    h264e_attr.dim.height = dest_height;
    h264e_attr.frame_info.framerate = gm_system.cap[ch].framerate;
    h264e_attr.ratectl.mode = GM_CBR;
    h264e_attr.ratectl.gop = 60;
    h264e_attr.ratectl.bitrate = 2048;  // 2Mbps
    h264e_attr.b_frame_num = 0;  // B-frames per GOP (H.264 high profile)
    h264e_attr.enable_mv_data = 0;  // disable H.264 motion data output
    gm_set_attr(encode_object[ch], &h264e_attr);
            
    // bind channel recording (绑定捕捉对象到编码对象)
    bindfd[ch] = gm_bind(groupfd, capture_object, encode_object[ch]);
    if (gm_apply(groupfd) < 0) { // active setting (使生效)
        perror("Error! gm_apply fail, AP procedure something wrong!");
        exit(0);
    }

    tamper_detection_init();//tamper detection initial(移动侦测初始化)
    set_tamper_param(ch);
    
    ret = pthread_create(&enc_thread_id, NULL, encode_thread, (void *)ch);
    if (ret < 0) {
        perror("create encode thread failed");
        goto end;
    }

    ret = pthread_create(&motion_thread_id, NULL, motion_thread, (void *)ch);
    if (ret < 0) {
        perror("create motion thread failed");
        goto end;
    }

    printf("Enter q to quit.\n");
    while (1) {
        key = getchar();
        if(key == 'q') {
            enc_exit = 1;
            break;
        }
    }

end:
    if (enc_thread_id)
        pthread_join(enc_thread_id, NULL);
    if (motion_thread_id)
        pthread_join(motion_thread_id, NULL);
    tamper_detection_end();
    for (ch = 0; ch < MAX_CH_NUM; ch++) {
        gm_unbind(bindfd[ch]);
        gm_apply(groupfd);
    }
    gm_delete_obj(capture_object);
    for (ch = 0; ch < MAX_CH_NUM; ch++)
        gm_delete_obj(encode_object[ch]);
    gm_delete_groupfd(groupfd);
    gm_release();
    return 0;
}

