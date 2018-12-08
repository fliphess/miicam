/**
 * @file encode_with_eptz.c
 *
 * Support platform: GM8210, GM8287, GM8139
 * This source code is to encode a crop crea from capture source, such as ePTZ
 * Copyright (C) 2014 GM Corp. (http://www.grain-media.com)
 *
 * $Revision: 1.10 $
 * $Date: 2014/05/14 09:56:17 $
 *
 */
/**
 * @example encode_with_eptz.c
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <sys/time.h>
#include "gmlib.h"

#define BITSTREAM_LEN   (720 * 576 * 3 / 2)
#define ENC_MAIN_W  (640)
#define ENC_MAIN_H  (320)
#define ENC_SUB_W  (320)
#define ENC_SUB_H  (168)
#define ZPTZ_TIME   (2)
gm_system_t gm_system;
#define MAX_BITSTREAM_NUM   2   // Main stream + Sub stream
void *groupfd;  // return of gm_new_groupfd() for main & sub stream
void *main_bindfd, *sub_bindfd;    // return of gm_bind()
void *main_capture_object;
void *main_enc_object, *sub_enc_object;

pthread_t enc_thread_id;
int enc_exit = 0;   // Notify program exit
int eptz_x; 
int eptz_y;
static int x_offset = 0, y_offset = 0;
static int zoom_step = 0;
static int move_right = 0, move_left = 0, move_up = 0, move_down = 0, zoom_in = 0, zoom_out = 0;

void do_move_right(int ch)
{
    DECLARE_ATTR(crop_attr, gm_crop_attr_t);
    if ((gm_system.cap[ch].dim.width - x_offset) <= eptz_x) {
        x_offset = 0;
        printf("stop move right\n");
        move_right = 1;
        return;
    }
    x_offset +=20;
    printf("move right offset(%d)\n", x_offset);
    crop_attr.enabled = 1;     
    crop_attr.src_crop_rect.x = gm_system.cap[ch].dim.width / 4 + x_offset; 
    crop_attr.src_crop_rect.y = 0;
    crop_attr.src_crop_rect.width = (gm_system.cap[ch].dim.width) / 2;
    crop_attr.src_crop_rect.height = (gm_system.cap[ch].dim.height) / 2;
    gm_set_attr(main_capture_object, &crop_attr);
    if (gm_apply(groupfd) < 0) { // active setting (使生效)
        perror("Error! gm_apply fail, AP procedure something wrong!");
        exit(0);
    }
}

void do_move_left(int ch)
{
    DECLARE_ATTR(crop_attr, gm_crop_attr_t);

    if ((gm_system.cap[ch].dim.width + x_offset) <= eptz_x) {
        x_offset = 0;
        printf("stop move left\n");
        move_left = 1;
        return;
    }
    x_offset -=20;
    printf("move left offset(%d)\n", x_offset);
    crop_attr.enabled = 1;     
    crop_attr.src_crop_rect.x = gm_system.cap[ch].dim.width / 4 + x_offset;  
    crop_attr.src_crop_rect.y = 0;
    crop_attr.src_crop_rect.width = (gm_system.cap[ch].dim.width) / 2;
    crop_attr.src_crop_rect.height = (gm_system.cap[ch].dim.height) / 2;
    gm_set_attr(main_capture_object, &crop_attr);

    if (gm_apply(groupfd) < 0) { // active setting (使生效)
        perror("Error! gm_apply fail, AP procedure something wrong!");
        exit(0);
    }
}

void do_move_up(int ch)
{
    DECLARE_ATTR(crop_attr, gm_crop_attr_t);

    if ((gm_system.cap[ch].dim.height - y_offset) <= eptz_y) {
        y_offset = 0;
        printf("stop move up\n");
        move_up = 1;
        return;
    }
    y_offset += 15;
    printf("move up offset(%d)\n", y_offset);
    crop_attr.enabled = 1;     
    crop_attr.src_crop_rect.x = 0; 
    crop_attr.src_crop_rect.y = gm_system.cap[ch].dim.height / 4 + y_offset;
    crop_attr.src_crop_rect.width = (gm_system.cap[ch].dim.width) / 2;
    crop_attr.src_crop_rect.height = (gm_system.cap[ch].dim.height) /2;
    gm_set_attr(main_capture_object, &crop_attr);

    if (gm_apply(groupfd) < 0) { // active setting (使生效)
        perror("Error! gm_apply fail, AP procedure something wrong!");
        exit(0);
    }
}

void do_move_down(int ch)
{
    DECLARE_ATTR(crop_attr, gm_crop_attr_t);

    if ((gm_system.cap[ch].dim.height + y_offset) <= eptz_y) {
        y_offset = 0;
        printf("stop move down\n");
        move_down = 1;
        return;
    }
    y_offset -= 15;
    printf("move down offset(%d)\n", y_offset);
    crop_attr.enabled = 1;     
    crop_attr.src_crop_rect.x = 0; 
    crop_attr.src_crop_rect.y = gm_system.cap[ch].dim.height / 4 + y_offset;
    crop_attr.src_crop_rect.width = (gm_system.cap[ch].dim.width) / 2;
    crop_attr.src_crop_rect.height = (gm_system.cap[ch].dim.height) /2;
    gm_set_attr(main_capture_object, &crop_attr);

    if (gm_apply(groupfd) < 0) { // active setting (使生效)
        perror("Error! gm_apply fail, AP procedure something wrong!");
        exit(0);
    }
}

void do_zoom_in(int ch)
{
    DECLARE_ATTR(crop_attr, gm_crop_attr_t);
    
    if (((gm_system.cap[ch].dim.height / 4) - zoom_step) <= 0) {
        zoom_step = 0;
        printf("stop zoom in\n");
        zoom_in = 1;
        return;
    }
    zoom_step += 30;
    printf("zoom in offset(%d)\n", zoom_step);
    crop_attr.enabled = 1;     
    crop_attr.src_crop_rect.x = zoom_step;
    crop_attr.src_crop_rect.y = zoom_step;
    crop_attr.src_crop_rect.width = gm_system.cap[ch].dim.width - zoom_step;
    crop_attr.src_crop_rect.height = gm_system.cap[ch].dim.height - zoom_step;
    gm_set_attr(main_capture_object, &crop_attr);

    if (gm_apply(groupfd) < 0) { // active setting (使生效)
        perror("Error! gm_apply fail, AP procedure something wrong!");
        exit(0);
    }
}

void do_zoom_out(int ch)
{
    DECLARE_ATTR(crop_attr, gm_crop_attr_t);

    if (((gm_system.cap[ch].dim.height / 4) + zoom_step) <= 0) {
        zoom_step = 0;
        printf("stop zoom out\n");
        zoom_out = 1;
        return;
    }
    zoom_step -= 30;
    printf("zoom out offset(%d)\n", zoom_step);
    crop_attr.enabled = 1;     
    crop_attr.src_crop_rect.x = ((gm_system.cap[ch].dim.width / 4) + zoom_step); 
    crop_attr.src_crop_rect.y = ((gm_system.cap[ch].dim.height / 4) + zoom_step);
    crop_attr.src_crop_rect.width = (gm_system.cap[ch].dim.width / 2) - zoom_step;
    crop_attr.src_crop_rect.height = (gm_system.cap[ch].dim.height / 2) - zoom_step;
    gm_set_attr(main_capture_object, &crop_attr);

    if (gm_apply(groupfd) < 0) { // active setting (使生效)
        perror("Error! gm_apply fail, AP procedure something wrong!");
        exit(0);
    }
}

void do_normal_record(int ch)
{
    DECLARE_ATTR(crop_attr, gm_crop_attr_t);

    printf("normal rec\n");
    crop_attr.enabled = 1;     
    crop_attr.src_crop_rect.x = (gm_system.cap[ch].dim.width / 2); 
    crop_attr.src_crop_rect.y = (gm_system.cap[ch].dim.height  / 2);
    crop_attr.src_crop_rect.width = (gm_system.cap[ch].dim.width) / 2;
    crop_attr.src_crop_rect.height = (gm_system.cap[ch].dim.height) /2;
    gm_set_attr(main_capture_object, &crop_attr);

    if (gm_apply(groupfd) < 0) { // active setting (使生效)
        perror("Error! gm_apply fail, AP procedure something wrong!");
        exit(0);
    }
}


static void *encode_thread(void *arg)
{
    int i, ret;
    int ch = (int)arg;
    FILE *record_file[2];
    char filename[50];
    char *bitstream_data[MAX_BITSTREAM_NUM];
    gm_pollfd_t poll_fds[MAX_BITSTREAM_NUM];
    gm_enc_multi_bitstream_t multi_bs[MAX_BITSTREAM_NUM];
    struct timeval time_start;
    struct timeval cur_start;

    // open file for write(打开录相文件)
    for (i = 0; i < MAX_BITSTREAM_NUM; i++) {
        bitstream_data[i] = (char *)malloc(BITSTREAM_LEN);
        if (bitstream_data[i] == 0)
            return 0;
        memset(bitstream_data[i], 0, BITSTREAM_LEN);
    }
    
    sprintf(filename, "ch%d_%dx%d_mainstream_eptz.264", ch, ENC_MAIN_W,
        ENC_MAIN_H);
    record_file[0] = fopen(filename, "wb");
    if (record_file[0] == NULL) {
        printf("open file error(%s)! \n", filename);
        exit(1);
    }

    sprintf(filename, "ch%d_%dx%d_substream_eptz.264", ch, ENC_SUB_W, 
        ENC_SUB_H);

    record_file[1] = fopen(filename, "wb");
    if (record_file[1] == NULL) {
        printf("open file error(%s)! \n", filename);
        exit(1);
    }

    memset(poll_fds, 0, sizeof(poll_fds));

    /*** Capture mainstream stream */
    poll_fds[0].bindfd = main_bindfd;
    poll_fds[0].event = GM_POLL_READ;
    /*** Capture another path substream */
    poll_fds[1].bindfd = sub_bindfd;
    poll_fds[1].event = GM_POLL_READ;

    gettimeofday(&time_start, NULL);     
    while (enc_exit == 0) {
        /** poll bitstream until 500ms timeout */
        ret = gm_poll(poll_fds, MAX_BITSTREAM_NUM, 500);
        gettimeofday(&cur_start, NULL);
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
            multi_bs[i].bs.bs_buf = bitstream_data[i];  // set buffer point(指定输出缓冲指针位置)
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
                    fwrite(multi_bs[i].bs.bs_buf, 1, multi_bs[i].bs.bs_len, record_file[i]);
                    fflush(record_file[i]);

                    if ((cur_start.tv_sec - time_start.tv_sec) == ZPTZ_TIME) {
                        gettimeofday(&time_start, NULL);                        
                        if (!zoom_in)
                            do_zoom_in(ch);  
                        else if (!zoom_out)
                            do_zoom_out(ch);  
                        else if (!move_right)
                            do_move_right(ch); 
                        else if (!move_left)
                            do_move_left(ch);  
                        else if (!move_up)
                            do_move_up(ch);  
                        else if (!move_down)
                            do_move_down(ch);  
                        else
                            do_normal_record(ch);
                    }
                }
            }
        }    
    }

    for (i = 0; i < MAX_BITSTREAM_NUM; i++) {
        fclose(record_file[i]);
        free(bitstream_data[i]);
    }
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
    groupfd = gm_new_groupfd(); // create main stream group fd (获取groupfd)
    eptz_x = gm_system.cap[0].dim.width * 3 / 4; 
    eptz_y = gm_system.cap[0].dim.height * 3 / 4;

    main_capture_object = gm_new_obj(GM_CAP_OBJECT); // new capture object(获取捕捉对象)
    main_enc_object = gm_new_obj(GM_ENCODER_OBJECT);  // // create encoder object (获取编码对象)
    sub_enc_object = gm_new_obj(GM_ENCODER_OBJECT);  // // create encoder object (获取编码对象)
    
    ch = 0; // use capture virtual channel 0

    /*** Init mainstream */    
    cap_attr.cap_vch = ch;

    //GM8210 capture path 0(liveview), 1(CVBS), 2(can scaling), 3(can scaling)
    //GM8139/GM8287 capture path 0(liveview), 1(CVBS), 2(can scaling), 3(can't scaling down)
    cap_attr.path = 2;
    cap_attr.enable_mv_data = 0;
    gm_set_attr(main_capture_object, &cap_attr); // set capture attribute (设置捕捉属性)

    h264e_attr.dim.width = ENC_MAIN_W;
    h264e_attr.dim.height = ENC_MAIN_H;
    h264e_attr.frame_info.framerate = gm_system.cap[ch].framerate;
    h264e_attr.ratectl.mode = GM_CBR;
    h264e_attr.ratectl.gop = 60;
    h264e_attr.ratectl.bitrate = 2048;  // 2Mbps
    h264e_attr.b_frame_num = 0; // B-frames per GOP (H.264 high profile)
    h264e_attr.enable_mv_data = 0;  // disable H.264 motion data output
    gm_set_attr(main_enc_object, &h264e_attr);
    /*** bind main stream recording (绑定捕捉对象到编码对象) */
    main_bindfd = gm_bind(groupfd, main_capture_object, main_enc_object);
    if (gm_apply(groupfd) < 0) {  // active setting (使生效)
        perror("Error! gm_apply fail, AP procedure something wrong!");
        exit(0);
    }

    /*** Init substream */

    h264e_attr.ratectl.bitrate = 512;  // 512Kbps
    h264e_attr.dim.width = ENC_SUB_W;
    h264e_attr.dim.height = ENC_SUB_H;
    gm_set_attr(sub_enc_object, &h264e_attr);

    /*** bind sub stream recording (绑定捕捉对象到编码对象) */
    sub_bindfd = gm_bind(groupfd, main_capture_object, sub_enc_object);
    if (gm_apply(groupfd) < 0) {  // active setting (使生效)
        perror("Error! gm_apply fail, AP procedure something wrong!");
        exit(0);
    }

    ret = pthread_create(&enc_thread_id, NULL, encode_thread, (void *)ch);
    if (ret < 0) {
        perror("create encode thead failed");
        return -1;
    }

    printf("Enter q to quit\n");
    while (1) {
        key = getchar();
        if (key == 'q') {
            enc_exit = 1;
            break;
        }
    }

    pthread_join(enc_thread_id, NULL);
    gm_unbind(main_bindfd);
    gm_unbind(sub_bindfd);
    gm_apply(groupfd);
    gm_delete_obj(main_enc_object);
    gm_delete_obj(sub_enc_object);
    gm_delete_obj(main_capture_object);
    gm_delete_groupfd(groupfd);
    gm_release();
    return ret;
}
