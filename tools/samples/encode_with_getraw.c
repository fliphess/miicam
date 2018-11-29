/**
 * @file encode_with_getraw.c
 *
 * Support platform: GM8210, GM8287, GM8139
 * This sample code is to get raw YUV422 data from encoder capture
 * Copyright (C) 2013 GM Corp. (http://www.grain-media.com)
 *
 * $Revision: 1.13 $
 * $Date: 2014/10/16 03:31:16 $
 *
 */
/**
 * @example encode_with_getraw.c
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
#define MAX_BITSTREAM_NUM   1

#define YUV422_WIDTH    100
#define YUV422_HEIGHT   100
#define YUV422_LEN      (YUV422_WIDTH * YUV422_HEIGHT * 2)

gm_system_t gm_system;
void *groupfd;   // return of gm_new_groupfd()
void *bindfd;    // return of gm_bind()
void *capture_object;
void *encode_object;
pthread_t enc_thread_id;
int enc_exit = 0;   // Notify program exit

static void *encode_thread(void *arg)
{
    int i, ret;
    int ch = (int)arg;
    char *bitstream_data;
    gm_pollfd_t poll_fds[MAX_BITSTREAM_NUM];
    gm_enc_multi_bitstream_t multi_bs[MAX_BITSTREAM_NUM];

    // open file for write(打开录相文件)
    bitstream_data = (char *)malloc(BITSTREAM_LEN);
    memset(bitstream_data, 0, BITSTREAM_LEN);
    memset(poll_fds, 0, sizeof(poll_fds));

    poll_fds[ch].bindfd = bindfd;
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
                }
            }
        }
    }

    free(bitstream_data);
    return 0;
}

void sample_get_rawdata(void *bindfd, gm_rawdata_mode_t rawdata_mode)
{
    static int filecount = 0;
    char    *yuv_buf = 0;
    FILE    *yuv_fd = NULL;
    char    filename[40];
    region_rawdata_t region_rawdata;
    int ret = 0;
    yuv_buf = malloc(YUV422_LEN);
    if (!yuv_buf) {
        printf("Error to allocate size %d\n", YUV422_LEN);
        return;
    }

    /*GM_PARTIAL_FROM_CAP_OBJ: get partial region based on capture output resolution (h264e_attr.dim) */
    /*GM_WHOLE_FROM_ENC_OBJ: get whole region based on encode input resolution (h264e_attr.dim) */
    region_rawdata.region.x = 100; //Don't care this element in GM_WHOLE_FROM_ENC_OBJ mode
    region_rawdata.region.y = 100; //Don't care this element in GM_WHOLE_FROM_ENC_OBJ mode
    region_rawdata.region.width = YUV422_WIDTH;
    region_rawdata.region.height = YUV422_HEIGHT;
    region_rawdata.yuv_buf = yuv_buf;
    region_rawdata.yuv_buf_len = YUV422_LEN;
    region_rawdata.rawdata_mode = rawdata_mode;
    ret = gm_get_rawdata(bindfd, &region_rawdata, 500);
    if (!ret) { // Timeout value 500ms
        printf("Error to get raw data!\n");
        goto get_exit;
    }
    if (ret == -1) { // yuv dim is over spec
        printf("Error to get raw dim!\n");
        goto get_exit;
    }
    
    if (rawdata_mode == GM_PARTIAL_FROM_CAP_OBJ) { 
        sprintf(filename, "yuv422_%dx%d_%d_partial.yuv", YUV422_WIDTH, YUV422_HEIGHT, filecount++);
    } else if (rawdata_mode == GM_WHOLE_FROM_ENC_OBJ) {
        sprintf(filename, "yuv422_%dx%d_%d_whole.yuv", YUV422_WIDTH, YUV422_HEIGHT, filecount++);
    } else {
        printf("Error, this mode(%d) is not supported.", rawdata_mode);
    }

    yuv_fd = fopen(filename, "wb");
    if (yuv_fd == NULL) {
        printf("Fail to open file %s\n", filename);
        exit(1);
    }
    printf("Write %s done!\n", filename);
    fwrite(yuv_buf, 1, YUV422_LEN, yuv_fd);
    fclose(yuv_fd);

get_exit:
    free(yuv_buf);
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

    capture_object = gm_new_obj(GM_CAP_OBJECT); // new capture object(获取捕捉对象)
    encode_object = gm_new_obj(GM_ENCODER_OBJECT);  // // create encoder object (获取编码对象)

    ch = 0; // use capture virtual channel 0
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
    if (gm_apply(groupfd) < 0) {// active setting (使生效)
        perror("Error! gm_apply fail, AP procedure something wrong!");
        exit(0);
    }

    ret = pthread_create(&enc_thread_id, NULL, encode_thread, (void *)ch);
    if (ret < 0) {
        perror("create encode thread failed");
        return -1;
    }

    printf("Start Encoding....\n");
    printf("Please select mode for getting raw data:\n"
           "  'c': get partial raw data from capture object\n"
           "  'e': get whole raw data from encode object\n"
           "  'q': quit\n");
    while (1) {
        key = getchar();
        if(key == 'q') {
            enc_exit = 1;
            break;
        } else if (key == 'c' || key == 'C') {
            sample_get_rawdata(bindfd, GM_PARTIAL_FROM_CAP_OBJ);
        } else if (key == 'e' || key == 'E') {
            sample_get_rawdata(bindfd, GM_WHOLE_FROM_ENC_OBJ);
        }
    }

    pthread_join(enc_thread_id, NULL);
    gm_unbind(bindfd);
    gm_apply(groupfd);
    gm_delete_obj(encode_object);
    gm_delete_obj(capture_object);
    gm_delete_groupfd(groupfd);
    gm_release();
    return ret;
}
