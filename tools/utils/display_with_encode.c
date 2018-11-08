/**
 * @file display_with_encode.c
 * 
 * Support platform: GM8210, GM8287
 * This source code is playback and liveview show together and encode it.
 * Copyright (C) 2014 GM Corp. (http://www.grain-media.com)
 *
 * $Revision: 1.3 $
 * $Date: 2014/11/07 02:54:22 $
 *
 */
/**
 * @example display_with_encode.c
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

gm_system_t gm_system;
void *groupfd;
void *bindfd[5];
void *file_object[2];
void *capture_object[2];
void *win_object[4];
void *encode_object;
int pb_exit = 0;
int enc_exit = 0;

unsigned int time_to_ms(struct timeval *a)
{
    return (a->tv_sec * 1000) + (a->tv_usec / 1000);
}


char *pattern_name[] = {"C30", "C30"};
int pattern_frame_rate[] = {30, 30};
static void *playback_thread(void *arg)
{
    int ret = 0, length = 0, ch = 0;
    char filename[40];
    FILE *bs_fd[2], *len_fd[2];
    char *data[2];
    unsigned int frame_period[2];
    unsigned int channel_time[2];
    unsigned int current_ms;
    gm_dec_multi_bitstream_t multi_bs[2];
    struct timeval time_start;
    unsigned int diff, sleep_ms = 0;

    for (ch = 0; ch < 2; ch++) {
        sprintf(filename, "%s.264", pattern_name[ch]);
        if ((bs_fd[ch] = fopen(filename, "rb")) == NULL) {
            printf("[ERROR] Open %s failed!!\n", filename);
            exit(1);
        }
        sprintf(filename, "%s.len", pattern_name[ch]);
        if ((len_fd[ch] = fopen(filename, "rb")) == NULL) {
            printf("[ERROR] Open %s failed!!\n", filename);
            exit(1);
        }
        data[ch] = (char *)malloc(BITSTREAM_LEN);
        if (!data[ch]) {
            printf("Error allocation\n");
            exit(1);
        }
        frame_period[ch] = 1000 / pattern_frame_rate[ch];
        printf("Playback CH%d pattern %s.264 (frame time %dms) starting..\n", ch, pattern_name[ch],
            frame_period[ch]);
    }


    gettimeofday(&time_start, NULL);
    current_ms = time_to_ms(&time_start);
    for (ch = 0; ch < 2; ch++)
        channel_time[ch] = current_ms + frame_period[ch];
    
    while (1) {
        if (pb_exit == 1)
            break;

        gettimeofday(&time_start, NULL);
        current_ms = time_to_ms(&time_start);
        
        memset(multi_bs, 0, sizeof(multi_bs));
        sleep_ms = 0xFFFFFFFF;
        for (ch = 0; ch < 2; ch++) {
            if (channel_time[ch] > current_ms) {
                diff = channel_time[ch] - current_ms;
                if ((sleep_ms) && (diff < sleep_ms))
                    sleep_ms = diff;
                continue;
            }

            sleep_ms = 0;
            if (fscanf(len_fd[ch], "%d\n", &length) == EOF) {
                fseek(bs_fd[ch], 0, SEEK_SET);
                fseek(len_fd[ch], 0, SEEK_SET);
                fscanf(len_fd[ch], "%d\n", &length);
            }
            if (length == 0)
                continue;
            fread(data[ch], 1, length, bs_fd[ch]);
            multi_bs[ch].bindfd = bindfd[ch];
            multi_bs[ch].bs_buf = data[ch];
            multi_bs[ch].bs_buf_len = length;
            
            channel_time[ch] += frame_period[ch];
        }

        if (sleep_ms)
            usleep(sleep_ms * 1000);
        else {
            if ((ret = gm_send_multi_bitstreams(multi_bs, 2, 500)) < 0) {
                usleep(100000);
                continue;
            }
        }
    }
    
    for (ch = 0; ch < 2; ch++) {
        fclose(bs_fd[ch]);
        fclose(len_fd[ch]);
        free(data[ch]);
    }
    return 0;
}

#define MAX_BITSTREAM_NUM   1
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

    sprintf(filename, "ch%d_display.264", ch);
    record_file = fopen(filename, "wb");
    if (record_file == NULL) {
        printf("open file error(%s)! \n", filename);
        exit(1);
    }

    memset(poll_fds, 0, sizeof(poll_fds));

    poll_fds[ch].bindfd = bindfd[4];
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
    int i, key, lcd_vch_main;
    pthread_t playback_thread_id;
    pthread_t enc_thread_id;
    DECLARE_ATTR(win_attr, gm_win_attr_t);
    DECLARE_ATTR(file_attr, gm_file_attr_t);
    DECLARE_ATTR(cap_attr, gm_cap_attr_t);
    DECLARE_ATTR(h264e_attr, gm_h264e_attr_t);

    lcd_vch_main = 0;

    gm_init();
    gm_get_sysinfo(&gm_system);
    groupfd = gm_new_groupfd();

    //Fill file info for playback
    for (i = 0; i < 2; i++) {
        file_object[i] = gm_new_obj(GM_FILE_OBJECT);
        file_attr.vch = i;
        file_attr.max_width = 720;
        file_attr.max_height = 576;
        gm_set_attr(file_object[i], &file_attr);
    }

    //Fill cap info for liveview
    for (i = 0; i < 2; i++) {
        capture_object[i] = gm_new_obj(GM_CAP_OBJECT);
        cap_attr.cap_vch = i;
        cap_attr.path = 2;
        cap_attr.enable_mv_data = 0;
        gm_set_attr(capture_object[i], &cap_attr);
    }

    //Fill win info for display(composed by liveview and playback)
    for (i = 0; i < 4; i++) {
        win_object[i] = gm_new_obj(GM_WIN_OBJECT);
        win_attr.lcd_vch = GM_LCD0;
        win_attr.rect.x = (i % 2) * (gm_system.lcd[lcd_vch_main].dim.width / 2);
        win_attr.rect.y = (i / 2) * (gm_system.lcd[lcd_vch_main].dim.height / 2);
        win_attr.rect.width = (gm_system.lcd[lcd_vch_main].dim.width / 2);
        win_attr.rect.height = (gm_system.lcd[lcd_vch_main].dim.height / 2);
        win_attr.visible = 1;
        win_attr.win_to_enc = FROM_MAIN_DISPLAY;
        gm_set_attr(win_object[i], &win_attr);
    }

    //Fill enc info for display
    encode_object = gm_new_obj(GM_ENCODER_OBJECT);
    h264e_attr.ratectl.mode = GM_CBR;
    h264e_attr.ratectl.gop = 30;
    h264e_attr.dim.width = 720;
    h264e_attr.dim.height = 480;
    h264e_attr.ratectl.bitrate = 1024;
    h264e_attr.frame_info.framerate = 30;
    h264e_attr.b_frame_num = 0;
    h264e_attr.enable_mv_data = 0;
    gm_set_attr(encode_object, &h264e_attr);

    bindfd[0] = gm_bind(groupfd, file_object[0], win_object[0]); 
    bindfd[1] = gm_bind(groupfd, file_object[1], win_object[1]); 
    bindfd[2] = gm_bind(groupfd, capture_object[0], win_object[2]);
    bindfd[3] = gm_bind(groupfd, capture_object[1], win_object[3]);
    bindfd[4] = gm_bind(groupfd, win_object[0], encode_object);
    if (gm_apply(groupfd) < 0) {
        perror("Error! gm_apply fail, AP procedure something wrong!");
        exit(0);
    }

    if (pthread_create(&playback_thread_id, NULL, playback_thread, (void *)i) < 0) {
        perror("create thread \"sample_get_bitstream_thread\" failed!");
        return -1;
    }

    if (pthread_create(&enc_thread_id, NULL, encode_thread, (void *)0) < 0) {
        perror("create encode thread failed");
        return -1;
    }

    while (1) {
        printf("Enter q to exit\n");
        key = getchar();
        if (key == 'q') {
            pb_exit = 1;
            enc_exit = 1;
            break;
        }
    }

    pthread_join(playback_thread_id, NULL);
    pthread_join(enc_thread_id, NULL);
    for (i = 0; i < 5; i++)
        gm_unbind(bindfd[i]);
    gm_apply(groupfd);
    for (i = 0; i < 2; i++)
        gm_delete_obj(capture_object[i]);
    for (i = 0; i < 2; i++)
        gm_delete_obj(file_object[i]);
    for (i = 0; i < 4; i++)
        gm_delete_obj(win_object[i]);
    gm_delete_obj(encode_object);
    gm_delete_groupfd(groupfd);
    gm_release();

    return 0;
}
