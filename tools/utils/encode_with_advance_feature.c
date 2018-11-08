/**
 * @file encode_with_advance_feature.c
 *
 * Support platform: GM8210, GM8287, GM8139
 * This source code is to encode bitstream with advance feature, such as field coding, roi QP
 * Copyright (C) 2013 GM Corp. (http://www.grain-media.com)
 *
 * $Revision: 1.16 $
 * $Date: 2014/06/04 10:05:06 $
 *
 */
/**
 * @example encode_with_advance_feature.c
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

gm_system_t gm_system;
void *groupfd;   // return of gm_new_groupfd()
void *bindfd;    // return of gm_bind()
void *capture_object;
void *encode_object;
pthread_t enc_thread_id;
int enc_exit = 0;   // Notify program exit
unsigned int dest_width = 0, dest_height = 0;
char func_name[30];
int item;

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

    sprintf(filename, "ch%d_%dx%d_%s.264", ch, dest_width, dest_height, func_name);
    record_file = fopen(filename, "wb");
    if (record_file == NULL) {
        printf("open file error(%s)! \n", filename);
        exit(1);
    }

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
                    printf("<CH%d, mv_len=%d bs_len=%d, keyframe=%d, newbsflag=0x%x",
                            i, multi_bs[i].bs.mv_len, multi_bs[i].bs.bs_len,
                            multi_bs[i].bs.keyframe, multi_bs[i].bs.newbs_flag);
                    if (item == 1)
                        printf(", offset %u %u %u>\n", multi_bs[i].bs.slice_offset[0], 
                               multi_bs[i].bs.slice_offset[1], multi_bs[i].bs.slice_offset[2]);
                    else
                        printf(">\n");
                    fwrite(multi_bs[i].bs.bs_buf,1, multi_bs[i].bs.bs_len, record_file);
                    fflush(record_file);
                }
            }
        }
    }

    fclose(record_file);
    free(bitstream_data);
    printf("Write %s Done!\n", filename);
    return 0;
}

int main(void)
{
    int     ch, ret = 0;
    int     key;
    DECLARE_ATTR(cap_attr, gm_cap_attr_t);
    DECLARE_ATTR(h264e_attr, gm_h264e_attr_t);
    DECLARE_ATTR(h264e_advance_attr, gm_h264_advanced_attr_t);
    DECLARE_ATTR(h264e_roiqp_attr, gm_h264_roiqp_attr_t);

    printf("Enter advanced encoding item:\n     0:field coding  1:multislice  "
           "2:gray scale encode  3:ROI QP\n");
    scanf("%d",&item);
    if (item > 3) {
        printf("Invalid encoding item %d\n", item);
        exit(1);
    }
           
    gm_init(); //gmlib initial(GM库初始化)
    gm_get_sysinfo(&gm_system);

    groupfd = gm_new_groupfd(); // create new record group fd (获取groupfd)

    capture_object = gm_new_obj(GM_CAP_OBJECT); // new capture object(获取捕捉对象)
    encode_object = gm_new_obj(GM_ENCODER_OBJECT);  // // create encoder object (获取编码对象)

    ch = 0; // use capture virtual channel 0
    cap_attr.cap_vch = ch;

    //GM8210 capture path 0(liveview), 1(CVBS), 2(can scaling), 3(can scaling)
    //GM8139/GM8287 capture path 0(liveview), 1(CVBS), 2(can scaling), 3(can't scaling down)
    cap_attr.path = 2;
    cap_attr.enable_mv_data = 0;
    gm_set_attr(capture_object, &cap_attr); // set capture attribute (设置捕捉属性)

    dest_width = ((gm_system.cap[ch].dim.width >> 4) << 4);   //align 16
    dest_height = ((gm_system.cap[ch].dim.height >> 4) << 4);   //align 16

    h264e_attr.dim.width = dest_width;
    h264e_attr.dim.height = dest_height;
    h264e_attr.frame_info.framerate = gm_system.cap[ch].framerate;
    h264e_attr.ratectl.mode = GM_CBR;
    h264e_attr.ratectl.gop = 60;
    h264e_attr.ratectl.bitrate = 2048;  // 2Mbps
    h264e_attr.b_frame_num = 0;  // B-frames per GOP (H.264 high profile)
    h264e_attr.enable_mv_data = 0;  // disable H.264 motion data output
    gm_set_attr(encode_object, &h264e_attr);

    if (item == 0)
        strcpy(func_name, "field_coding");
    else if (item == 1)
        strcpy(func_name, "multi_slice");
    else if (item == 2)
        strcpy(func_name, "gray_scale");
    else if (item == 3)
        strcpy(func_name, "roi_qp");

    h264e_advance_attr.field_coding = (item == 0) ? 1 : 0;
    h264e_advance_attr.multi_slice = (item == 1) ? 4 : 0;    ///< Divide 4 regions of multislice
    h264e_advance_attr.gray_scale = (item == 2) ? 1: 0;
    gm_set_attr(encode_object, &h264e_advance_attr);

    /* give better encode quality for region (0, 0)->(width/2, height/2) */
    h264e_roiqp_attr.enabled = (item == 3) ? 1 : 0;
    h264e_roiqp_attr.rect[0].x = 0;
    h264e_roiqp_attr.rect[0].y = 0;
    h264e_roiqp_attr.rect[0].width = dest_width / 2;
    h264e_roiqp_attr.rect[0].height = dest_height / 2;
    gm_set_attr(encode_object, &h264e_roiqp_attr);

    // bind channel recording (绑定捕捉对象到编码对象)
    bindfd = gm_bind(groupfd, capture_object, encode_object);
    if (gm_apply(groupfd) < 0) { // active setting (使生效)
        perror("Error! gm_apply fail, AP procedure something wrong!");
        exit(0);
    }

    ret = pthread_create(&enc_thread_id, NULL, encode_thread, (void *)ch);
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
    gm_apply(groupfd);
    gm_delete_obj(encode_object);
    gm_delete_obj(capture_object);
    gm_delete_groupfd(groupfd);
    gm_release();
    return ret;
}
