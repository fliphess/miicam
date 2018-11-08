/**
 * @file encode_with_capture_motion_detection2.c
 *
 * Support platform: GM8210, GM8287, GM8139
 * This sample code is demo motion detection by capture motion data
 *
 * Copyright (C) 2012 GM Corp. (http://www.grain-media.com)
 *
 * $Revision: 1.4 $
 * $Date: 2014/05/12 05:49:58 $
 *
 */
/**
 * @example encode_with_capture_motion_detection2.c
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <sys/time.h>
#include "gmlib.h"
#include "algorithm/capture_motion_detection2.c"

#define BITSTREAM_LEN       (720 * 576 * 3 / 2)
#define MD_DATA_LEN         (720 * 576 / 4)
#define MAX_CH_NUM          1
#define MAX_BITSTREAM_NUM   1
#define MAX_SUB_REGION_NUM  4

gm_system_t gm_system;
void *groupfd;       // return of gm_new_groupfd()
void *bindfd[MAX_CH_NUM];        // return of gm_bind()
void *capture_object;
void *encode_object[MAX_CH_NUM];
pthread_t enc_thread_id = 0, motion_thread_id = 0;
int enc_exit = 0;   // Notify program exit
unsigned int dest_width = 0, dest_height = 0;
struct mdt_alg_t mdt_alg[MAX_CH_NUM] = {{sub_region: NULL}};
struct mdt_result_t mdt_result[MAX_CH_NUM] = {{sub_region: NULL}};


static int set_cap_motion(int cap_vch, unsigned int id, unsigned int value)
{
    int ret = 0;
    gm_cap_motion_t cap_motion;

    cap_motion.id = id; //alpha
    cap_motion.value = value;

    ret = gm_set_cap_motion(cap_vch, &cap_motion);
    if (ret < 0) {
        printf("MD_Test: gm_set_cap_motion error!\n");
        return -1;
    }
    return 0;
}


static int set_interesting_region(int ch)
{
    int ret = 0;
    int mb_w_num, mb_h_num;
    int reg_idx;

    mdt_alg[ch].sub_region = (struct mdt_reg_t *) malloc(sizeof(struct mdt_reg_t) * MAX_SUB_REGION_NUM);

    if (mdt_alg[ch].sub_region == NULL)
    {
        printf("Error to allocate sub_region\n");
        ret = -1;
        goto err_ext;
    }

    memset(mdt_alg[ch].sub_region, 0, (sizeof(struct mdt_reg_t) * MAX_SUB_REGION_NUM));

    mdt_alg[ch].u_width = gm_system.cap[ch].dim.width;
    mdt_alg[ch].u_height = gm_system.cap[ch].dim.height;

    mdt_alg[ch].u_mb_width = 32;
    mdt_alg[ch].u_mb_height = 32;
    mdt_alg[ch].training_time = 15;
    mdt_alg[ch].frame_count = 0;
    mdt_alg[ch].sensitive_th = 80;

    mb_w_num  = (mdt_alg[ch].u_width + (mdt_alg[ch].u_mb_width - 1)) / mdt_alg[ch].u_mb_width;
    mb_h_num  = (mdt_alg[ch].u_height + (mdt_alg[ch].u_mb_height - 1)) / mdt_alg[ch].u_mb_height;

    mdt_alg[ch].mb_w_num = mb_w_num;
    mdt_alg[ch].mb_h_num = mb_h_num;
    reg_idx = 0;

    // * set interesting region[0]
    mdt_alg[ch].sub_region[reg_idx].is_enabled    = 1;
    mdt_alg[ch].sub_region[reg_idx].start_block_x = 0;
    mdt_alg[ch].sub_region[reg_idx].start_block_y = 0;
    mdt_alg[ch].sub_region[reg_idx].end_block_x   = mb_w_num / 2;
    mdt_alg[ch].sub_region[reg_idx].end_block_y   = mb_h_num / 2;
    mdt_alg[ch].sub_region[reg_idx].alarm_th      = 80;
    mdt_alg[ch].sub_region[reg_idx].alarm         = NO_MOTION;
    reg_idx++;

    // * set interesting region[1]
    mdt_alg[ch].sub_region[reg_idx].is_enabled    = 1;
    mdt_alg[ch].sub_region[reg_idx].start_block_x = mb_w_num / 2;
    mdt_alg[ch].sub_region[reg_idx].start_block_y = 0;
    mdt_alg[ch].sub_region[reg_idx].end_block_x   = mb_w_num - 1;
    mdt_alg[ch].sub_region[reg_idx].end_block_y   = mb_h_num / 2;
    mdt_alg[ch].sub_region[reg_idx].alarm_th      = 80;
    mdt_alg[ch].sub_region[reg_idx].alarm         = NO_MOTION;
    reg_idx++;

    // * set interesting region[2]
    mdt_alg[ch].sub_region[reg_idx].is_enabled    = 1;
    mdt_alg[ch].sub_region[reg_idx].start_block_x = 0;
    mdt_alg[ch].sub_region[reg_idx].start_block_y = mb_h_num / 2;
    mdt_alg[ch].sub_region[reg_idx].end_block_x   = mb_w_num / 2;
    mdt_alg[ch].sub_region[reg_idx].end_block_y   = mb_h_num - 1;
    mdt_alg[ch].sub_region[reg_idx].alarm_th      = 80;
    mdt_alg[ch].sub_region[reg_idx].alarm         = NO_MOTION;
    reg_idx++;

    // * set interesting region[3]
    mdt_alg[ch].sub_region[reg_idx].is_enabled    = 1;
    mdt_alg[ch].sub_region[reg_idx].start_block_x = mb_w_num / 2;
    mdt_alg[ch].sub_region[reg_idx].start_block_y = mb_h_num / 2;
    mdt_alg[ch].sub_region[reg_idx].end_block_x   = mb_w_num - 1;
    mdt_alg[ch].sub_region[reg_idx].end_block_y   = mb_h_num - 1;
    mdt_alg[ch].sub_region[reg_idx].alarm_th      = 80;
    mdt_alg[ch].sub_region[reg_idx].alarm         = NO_MOTION;
    reg_idx++;

    mdt_alg[ch].sub_region_num = reg_idx;

    set_cap_motion(ch, 0, 32);      // alpha
    set_cap_motion(ch, 1, 7371);    // tbg
    set_cap_motion(ch, 2, 7);       // init val
    set_cap_motion(ch, 3, 9);       // tb
    set_cap_motion(ch, 4, 11);      // sigma
    set_cap_motion(ch, 5, 15);      // prune
    set_cap_motion(ch, 7, 0x9ffb0); // alpha accuracy
    set_cap_motion(ch, 8, 9);       // tg
    set_cap_motion(ch, 10, 0x7fe0); // one min alpha

    ret = motion_detection_update(bindfd[ch], &mdt_alg[ch]);

    if (ret != 0) {
        printf("Error to do motion_detection_update at CH(%d)\n", ch);
        ret = -1;
        goto err_ext;
    }

err_ext:

    if (mdt_alg[ch].sub_region)
        free(mdt_alg[ch].sub_region);

    return ret;

}


static void *motion_thread(void *arg)
{
    int ch;
    int ret;
    gm_multi_cap_md_t *cap_md = NULL;
    int reg_idx;

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

    for (ch = 0; ch < MAX_CH_NUM; ch++) {
        mdt_result[ch].sub_region = (struct mdt_reg_result_t *)

        malloc(sizeof(struct mdt_reg_result_t) * MAX_SUB_REGION_NUM);

        if (mdt_result[ch].sub_region == NULL) {
            printf("Error to allocate result of sub_region info!\n");
            goto thread_exit;
        }

        mdt_result[ch].sub_region_num = MAX_SUB_REGION_NUM;
    }

    while (enc_exit == 0) {
        ret = gm_recv_multi_cap_md(cap_md, MAX_CH_NUM);

        if (ret < 0)
        {   // -1:error, 0:sucess
            printf("Error parameter to get motion data\n");
            continue;
        }

        ret = motion_detection_handling(cap_md, &mdt_result[0], MAX_CH_NUM);

        if (ret < 0)
        {   // -1:error, 0:sucess
            printf("Error to do motion_detection_handling\n");
            goto thread_exit;
        }

        for (ch = 0; ch < MAX_CH_NUM; ch++)
        {
            if (mdt_result[ch].ch_result == MOTION_IS_TRAINING)
            {
                printf("[---Motion Is training---] at CH(%d)\n", ch);
            }

            else if (mdt_result[ch].ch_result == MOTION_ALGO_ERROR)
            {
                printf("[---Motion Algorithm Error---] at CH(%d)\n", ch);
            }

            else if (mdt_result[ch].ch_result == MOTION_PARSING_ERROR)
            {
                printf("[---Paring data error---] at CH(%d)\n", ch);
            }
            else if (mdt_result[ch].ch_result == MOTION_IS_READY)
            {
                // sub-region MD
                for (reg_idx = 0; reg_idx < MAX_SUB_REGION_NUM; reg_idx++)
                {
                    if (mdt_result[ch].sub_region[reg_idx].reg_result == MOTION_DETECTED)
                    {
                        printf("[---sub-region %d Has Motion Detected at CH(%d)---]\n",
                                reg_idx, ch);
                    }
                }
            }
            else
            {
                printf("[---Motion Message not Defined Error---] at CH(%d)\n", ch);
            }
        }
        usleep(200000);   // two second period to detect motion
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

    for (ch = 0; ch < MAX_CH_NUM; ch++) {
        if (mdt_result[ch].sub_region) {
            free(mdt_result[ch].sub_region);
            mdt_result[ch].sub_region = NULL;
        }
    }

    motion_detection_end();

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

    // open file for write(��¼���ļ�)
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
        // * poll bitstream until 500ms timeout
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
            multi_bs[i].bs.bs_buf = bitstream_data;  // set buffer point(ָ���������ָ��λ��)
            multi_bs[i].bs.bs_buf_len = BITSTREAM_LEN;  // set buffer length(ָ�����峤��)
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

    gm_init(); //gmlib initial(GM���ʼ��)
    gm_get_sysinfo(&gm_system);

    groupfd = gm_new_groupfd(); // create new record group fd (��ȡgroupfd)

    ch = 0; // use capture virtual channel 0
    capture_object = gm_new_obj(GM_CAP_OBJECT); // new capture object(��ȡ��׽����)
    encode_object[ch] = gm_new_obj(GM_ENCODER_OBJECT);  // create encoder object (��ȡ�������)

    cap_attr.cap_vch = ch;

    //GM8210/GM8287 capture path 0(liveview), 1(reserved, don't use), 2(substream), 3(mainstream)
    //GM8139 capture path 0(liveview), 1(substream), 2(substream), 3(mainstream)

    cap_attr.path = 3;
    cap_attr.enable_mv_data = 1;
    gm_set_attr(capture_object, &cap_attr); // set capture attribute (���ò�׽����)

    dest_width = gm_system.cap[ch].dim.width;
    dest_height = gm_system.cap[ch].dim.height;

    h264e_attr.dim.width = dest_width;
    h264e_attr.dim.height = dest_height;
    h264e_attr.frame_info.framerate = gm_system.cap[ch].framerate;
    h264e_attr.ratectl.mode = GM_CBR;
    h264e_attr.ratectl.gop = 60;
    h264e_attr.ratectl.bitrate = 2048;  // 2Mbps
    h264e_attr.b_frame_num = 0;         // B-frames per GOP (H.264 high profile)
    h264e_attr.enable_mv_data = 0;      // disable H.264 motion data output
    gm_set_attr(encode_object[ch], &h264e_attr);

    // bind channel recording (�󶨲�׽���󵽱������)
    bindfd[ch] = gm_bind(groupfd, capture_object, encode_object[ch]);
    if (gm_apply(groupfd) < 0) {        // active setting (ʹ��Ч)
        perror("Error! gm_apply fail, AP procedure something wrong!");
        exit(0);
    }

    motion_detection_init();    // * motion detection initial(�ƶ�����ʼ��)

    // * set left area
    ret = set_interesting_region(ch);

    if (ret != 0) {
        perror("Error to do set_interesting_region!\n");
        goto end;
    }

    ret = pthread_create(&enc_thread_id, NULL, encode_thread, (void *)ch);

    if (ret < 0) {
        perror("create encode thread failed");
        goto end;
    }

    ret = pthread_create(&motion_thread_id, NULL, motion_thread, (void *) NULL);

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

    motion_detection_end();

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
