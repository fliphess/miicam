/**
 * @file encode_and_liveview_with_notification.c
 *
 * Support platform: GM8210, GM8287
 * This source code shows how's AP behavior to handle video signal change/lost
 * Copyright (C) 2014 GM Corp. (http://www.grain-media.com)
 *
 * $Revision: 1.1 $
 * $Date: 2014/03/13 03:25:26 $
 *
 */
/**
 * @example encode_and_liveview_with_notification.c
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
void *groupfd_rec, *groupfd_lv;
void *bindfd_rec, *bindfd_lv;
void *capture_object_rec, *capture_object_lv;
void *encode_object;
void *win_object;
pthread_t enc_thread_id;
int enc_exit = 0;
int ch = 0;

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

    sprintf(filename, "ch%d_notifcation.264", ch);
    record_file = fopen(filename, "wb");
    if (record_file == NULL) {
        printf("open file error(%s)! \n", filename);
        exit(1);
    }

    memset(poll_fds, 0, sizeof(poll_fds));

    poll_fds[ch].bindfd = bindfd_rec;
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
            multi_bs[i].bs.bs_buf = bitstream_data;
            multi_bs[i].bs.bs_buf_len = BITSTREAM_LEN;
            multi_bs[i].bs.mv_buf = 0;
            multi_bs[i].bs.mv_buf_len = 0;
        }
        
        if ((ret = gm_recv_multi_bitstreams(multi_bs, MAX_BITSTREAM_NUM)) < 0) {
            printf("Error return value %d\n", ret);
        } else {
            for (i = 0; i < MAX_BITSTREAM_NUM; i++) {
                if ((multi_bs[i].retval < 0) && multi_bs[i].bindfd) {
                    printf("CH%d Error to receive bitstream. ret=%d\n", i, multi_bs[i].retval);
                } else if (multi_bs[i].retval == GM_SUCCESS) {
                    printf("<CH%d, bs_len=%d> flag(%x)\n", i, multi_bs[i].bs.bs_len,
                        multi_bs[i].bs.newbs_flag);
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

void my_notify_handler(gm_obj_type_t obj_type, int vch, gm_notify_t notify)
{
    DECLARE_ATTR(h264e_attr, gm_h264e_attr_t);
    if (notify == GM_NOTIFY_SIGNAL_LOSS)
        printf("Ch(%d) Signal Loss!\n", vch);
    else if (notify == GM_NOTIFY_SIGNAL_PRESENT)
        printf("Ch(%d) Signal Present!\n", vch); 
    else if (notify == GM_NOTIFY_HW_CONFIG_CHANGE) {
        printf("Ch(%d) H/W change, restart to encode %dx%d!\n", vch, gm_system.cap[ch].dim.width,
                gm_system.cap[ch].dim.height);

//Whenever getting GM_NOTIFY_HW_CONFIG_CHANGE, ap should retrieve new system info again, and ...
//   for liveview, capture will calibrating itself internally, and ap needn't do anything.
//   for record, ap should reassign new dimension(for all path) and apply it.
        gm_get_sysinfo(&gm_system);
        h264e_attr.dim.width = gm_system.cap[ch].dim.width;
        h264e_attr.dim.height = gm_system.cap[ch].dim.height;
        h264e_attr.frame_info.framerate = gm_system.cap[ch].framerate;
        h264e_attr.ratectl.mode = GM_CBR;
        h264e_attr.ratectl.gop = 60;
        h264e_attr.ratectl.bitrate = 2000;
        h264e_attr.b_frame_num = 0;
        h264e_attr.enable_mv_data = 0;
        gm_set_attr(encode_object, &h264e_attr);

        gm_apply(groupfd_rec);
    }
}

int main(void)
{
    int ret = 0;
    int key;
    DECLARE_ATTR(cap_attr, gm_cap_attr_t);
    DECLARE_ATTR(cap_attr_lv, gm_cap_attr_t);
    DECLARE_ATTR(h264e_attr, gm_h264e_attr_t);
    DECLARE_ATTR(win_attr, gm_win_attr_t);

    gm_init();
    gm_get_sysinfo(&gm_system);

    gm_register_notify_handler(GM_NOTIFY_SIGNAL_LOSS, my_notify_handler);
    gm_register_notify_handler(GM_NOTIFY_SIGNAL_PRESENT, my_notify_handler);
    gm_register_notify_handler(GM_NOTIFY_HW_CONFIG_CHANGE, my_notify_handler);

    groupfd_rec = gm_new_groupfd();
    groupfd_lv = gm_new_groupfd();
    capture_object_rec = gm_new_obj(GM_CAP_OBJECT);
    capture_object_lv = gm_new_obj(GM_CAP_OBJECT);
    encode_object = gm_new_obj(GM_ENCODER_OBJECT);
    win_object = gm_new_obj(GM_WIN_OBJECT); 

    cap_attr.cap_vch = ch;
    cap_attr.path = 3;
    cap_attr.enable_mv_data = 0;
    gm_set_attr(capture_object_rec, &cap_attr);

    h264e_attr.dim.width = gm_system.cap[ch].dim.width;
    h264e_attr.dim.height = gm_system.cap[ch].dim.height;
    h264e_attr.frame_info.framerate = gm_system.cap[ch].framerate;
    h264e_attr.ratectl.mode = GM_CBR;
    h264e_attr.ratectl.gop = 60;
    h264e_attr.ratectl.bitrate = 2048;
    h264e_attr.b_frame_num = 0;
    h264e_attr.enable_mv_data = 0;
    gm_set_attr(encode_object, &h264e_attr);

    cap_attr_lv.cap_vch = ch;
    cap_attr_lv.path = 0;
    cap_attr_lv.enable_mv_data = 0;
    gm_set_attr(capture_object_lv, &cap_attr_lv);

    win_attr.lcd_vch = GM_LCD0;
    win_attr.rect.x = 0;
    win_attr.rect.y = 0;
    win_attr.rect.width = gm_system.lcd[0].dim.width / 2;
    win_attr.rect.height = gm_system.lcd[0].dim.height / 2;
    win_attr.visible = 1;
    gm_set_attr(win_object, &win_attr);

    bindfd_rec = gm_bind(groupfd_rec, capture_object_rec, encode_object);
    gm_apply(groupfd_rec);

    bindfd_lv = gm_bind(groupfd_lv, capture_object_lv, win_object); 
    gm_apply(groupfd_lv);

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
    gm_unbind(bindfd_rec);
    gm_unbind(bindfd_lv);
    gm_apply(groupfd_rec);
    gm_apply(groupfd_lv);
    gm_delete_obj(capture_object_rec);
    gm_delete_obj(capture_object_lv);
    gm_delete_obj(encode_object);
    gm_delete_obj(win_object);
    gm_delete_groupfd(groupfd_rec);
    gm_delete_groupfd(groupfd_lv);
    gm_release();
    return ret;
}
