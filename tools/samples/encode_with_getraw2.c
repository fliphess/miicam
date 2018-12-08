/**
 * @file encode_with_getraw2.c
 *
 * Support platform: GM813x only
 * This sample code is to get 1 channel raw YUV422 data from another capture path
 *
 * Copyright (C) 2014 GM Corp. (http://www.grain-media.com)
 *
 * $Revision: 1.7 $
 * $Date: 2014/07/22 03:17:35 $
 *
 */
/**
 * @example encode_with_getraw2.c
 */

/*
  Please note, this only supoprt ONE channel capture raw data output.
  To support this mode of getraw from specified capture path, it must modify /mnt/mtd/gmlib.cfg

[RAW_OUT]                                                                                        
; CONFIG1 = channels/total_fps/ddr_channel, ...                                                  
CONFIG1 = 2M/1/30/0

...

[BUFFER_RAW]                                                       
raw_out_buffer_time = 66

  and reboot system.
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <sys/time.h>
#include "gmlib.h"

#define BITSTREAM_LEN       (720 * 576 * 3 / 2)
#define MAX_BITSTREAM_NUM   2

#define YUV422_WIDTH    720
#define YUV422_HEIGHT   480
#define YUV422_LEN      (YUV422_WIDTH * YUV422_HEIGHT * 2)

gm_system_t gm_system;
void *groupfd;   // return of gm_new_groupfd()
void *bindfd;    // return of gm_bind()
void *bindfd_raw; // return of gm_bind()
void *capture_object;
void *capture_raw_object;
void *encode_object;
void *encode_raw_object;
pthread_t enc_thread_id;
int enc_exit = 0;   // Notify program exit

static void *encode_thread(void *arg)
{
    int i, ret, max_len[2] = {0,0};
    char *bitstream_data;
    char *yuv_data;
    char filename[50];
    FILE *record_file[2];
    gm_pollfd_t poll_fds[MAX_BITSTREAM_NUM];
    gm_enc_multi_bitstream_t multi_bs[MAX_BITSTREAM_NUM];

    sprintf(filename, "getraw2_%dx%d.264", gm_system.cap[0].dim.width, gm_system.cap[0].dim.height);
    record_file[0] = fopen(filename, "wb");
    if (record_file[0] == NULL) {
        printf("open file error(%s)! \n", filename);
        exit(1);
    }

    sprintf(filename, "getraw2_%dx%d.yuv", YUV422_WIDTH, YUV422_HEIGHT);
    record_file[1] = fopen(filename, "wb");
    if (record_file[1] == NULL) {
        printf("open file error(%s)! \n", filename);
        exit(1);
    }

    // open file for write(打开录相文件)
    bitstream_data = (char *)malloc(BITSTREAM_LEN);
    memset(bitstream_data, 0, BITSTREAM_LEN);

    yuv_data = (char *)malloc(YUV422_LEN);
    memset(yuv_data, 0, YUV422_LEN);

    memset(poll_fds, 0, sizeof(poll_fds));
    poll_fds[0].bindfd = bindfd;
    poll_fds[0].event = GM_POLL_READ;
    max_len[0]=BITSTREAM_LEN;
    
    poll_fds[1].bindfd = bindfd_raw;
    poll_fds[1].event = GM_POLL_READ;
    max_len[1]=YUV422_LEN;
    
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

            if (poll_fds[i].revent.bs_len > max_len[i]) {
                printf("bitstream buffer length is not enough! ch%d return len %u, buffer len %u\n",
                    i, poll_fds[i].revent.bs_len, max_len[i]);
                continue;
            }
            multi_bs[i].bindfd = poll_fds[i].bindfd;

            if (i == 0) { //h264
                multi_bs[i].bs.bs_buf = bitstream_data;  // set buffer point(指定输出缓冲指针位置)
                multi_bs[i].bs.bs_buf_len = BITSTREAM_LEN;  // set buffer length(指定缓冲长度)
                multi_bs[i].bs.mv_buf = 0;  // not to recevie MV data
                multi_bs[i].bs.mv_buf_len = 0;  // not to recevie MV data
            } else if (i == 1) { //yuv422
                multi_bs[i].bs.bs_buf = yuv_data;  // set buffer point(指定输出缓冲指针位置)
                multi_bs[i].bs.bs_buf_len = YUV422_LEN;  // set buffer length(指定缓冲长度)
            }
        }
        
        if ((ret = gm_recv_multi_bitstreams(multi_bs, MAX_BITSTREAM_NUM)) < 0) {
            printf("Error return value %d\n", ret);
        } else {
            for (i = 0; i < MAX_BITSTREAM_NUM; i++) {
                if ((multi_bs[i].retval < 0) && multi_bs[i].bindfd) {
                    printf("CH%d Error to receive bitstream. ret=%d\n", i, multi_bs[i].retval);
                } else if (multi_bs[i].retval == GM_SUCCESS) {
                    static int print_round = 0;
                    print_round++;
                    if ((print_round & 0x1f) == 0) {
                        printf(".");fflush(stdout);
                    }
                    
                    fwrite(multi_bs[i].bs.bs_buf,1, multi_bs[i].bs.bs_len, record_file[i]);
                    fflush(record_file[i]);
                }
            }
        }
    }

    fclose(record_file[0]);
    fclose(record_file[1]);
    free(bitstream_data);
    free(yuv_data);
    return 0;
}

int main(void)
{
    int ret = 0, ch;
    int key;
    DECLARE_ATTR(cap_attr, gm_cap_attr_t);
    DECLARE_ATTR(h264e_attr, gm_h264e_attr_t);
    DECLARE_ATTR(raw_attr, gm_raw_attr_t);

    printf("Note, this sample need to update gmlib.cfg to allocate more memory pool.\n");
    printf("If not allocated, system may be crash to stop, please check %s inside comment.\n", __FILE__);
    
    gm_init(); //gmlib initial(GM库初始化)
    gm_get_sysinfo(&gm_system);

    groupfd = gm_new_groupfd(); // create new record group fd (获取groupfd)

    capture_object = gm_new_obj(GM_CAP_OBJECT); // new capture object(获取捕捉对象)
    encode_object = gm_new_obj(GM_ENCODER_OBJECT);  // // create encoder object (获取编码对象)

    capture_raw_object = gm_new_obj(GM_CAP_OBJECT); // new capture object(获取捕捉对象)
    encode_raw_object = gm_new_obj(GM_ENCODER_OBJECT);  // // create encoder object (获取编码对象)

    ch = 0; ///< Use capture channel 0

/* build encode H264 */
    cap_attr.cap_vch = ch;
    //GM8210 capture path 0(liveview), 1(CVBS), 2(can scaling), 3(can scaling)
    //GM8139/GM8287 capture path 0(liveview), 1(CVBS), 2(can scaling), 3(can't scaling down)
    cap_attr.path = 3;
    cap_attr.enable_mv_data = 0;
    gm_set_attr(capture_object, &cap_attr); // set capture attribute (设置捕捉属性)

    h264e_attr.dim.width = gm_system.cap[ch].dim.width;
    h264e_attr.dim.height = gm_system.cap[ch].dim.height;

    h264e_attr.frame_info.framerate = gm_system.cap[ch].framerate;
    h264e_attr.ratectl.mode = GM_CBR;
    h264e_attr.ratectl.gop = 60;
    h264e_attr.ratectl.bitrate = 2048;  // 2Mbps
    h264e_attr.b_frame_num = 0;  // B-frames per GOP (H.264 high profile)
    h264e_attr.enable_mv_data = 0;  // disable H.264 motion data output
    gm_set_attr(encode_object, &h264e_attr);

    // bind channel recording (绑定捕捉对象到编码对象)
    bindfd = gm_bind(groupfd, capture_object, encode_object);

/* build encode raw */
    cap_attr.cap_vch = ch;
    //GM8210 capture path 0(liveview), 1(CVBS), 2(can scaling), 3(can scaling)
    //GM8139/GM8287 capture path 0(liveview), 1(CVBS), 2(can scaling), 3(can't scaling down)
    cap_attr.path = 1;
    cap_attr.enable_mv_data = 0;
    gm_set_attr(capture_raw_object, &cap_attr); // set capture attribute (设置捕捉属性)

    raw_attr.dim.width = YUV422_WIDTH;
    raw_attr.dim.height = YUV422_HEIGHT;
    raw_attr.frame_info.framerate = 30;
    gm_set_attr(encode_raw_object, &raw_attr);

    // bind channel recording (绑定捕捉对象到编码对象)
    bindfd_raw = gm_bind(groupfd, capture_raw_object, encode_raw_object);

    if (gm_apply(groupfd) < 0) { // active setting (使生效)
        perror("Error! gm_apply fail, AP procedure something wrong!");
        exit(0);
    }

    ret = pthread_create(&enc_thread_id, NULL, encode_thread, 0);
    if (ret < 0) {
        perror("create encode thread failed");
        return -1;
    }

    printf("Enter q to quit\n");
    while (1) {
        key = getchar();
        if(key == 'q') {
            enc_exit = 1;
            break;
        }
    }

    pthread_join(enc_thread_id, NULL);
    gm_unbind(bindfd);
    gm_unbind(bindfd_raw);
    gm_apply(groupfd);
    gm_delete_obj(encode_object);
    gm_delete_obj(encode_raw_object);
    gm_delete_obj(capture_object);
    gm_delete_obj(capture_raw_object);
    gm_delete_groupfd(groupfd);
    gm_release();
    return ret;
}
