/**
 * @file encode_with_av_sync.c
 *
 * Support platform: GM8210, GM8287, GM8139
 * This source code is to encode audio and video with timestamp
 * Copyright (C) 2013 GM Corp. (http://www.grain-media.com)
 *
 * $Revision: 1.4 $
 * $Date: 2014/03/13 05:56:25 $
 *
 */
/**
 * @example encode_with_av_sync.c
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <sys/time.h>
#include "gmlib.h"
#include <sys/time.h>

#define BITSTREAM_LEN       (720 * 576 * 3 / 2)
#define AUDIO_BITSTREAM_LEN 12800
#define AU_BITSTREAM_LEN    12800
#define MAX_BITSTREAM_NUM   1

gm_system_t gm_system;
void *groupfd; 
void *audio_groupfd; 
void *bindfd;  
void *audio_bindfd;  
void *capture_object;
void *encode_object;
void *audio_grab_object;
void *audio_encode_object;
pthread_t enc_thread_id;
pthread_t audio_enc_thread_id;
int enc_exit = 0;
char *audio_bs_data;

static void *video_encode_thread(void *arg)
{
    int i, ret;
    int ch = (int)arg;
    FILE *record_file, *length_file;
    char filename[50];
    char *bitstream_data;
    gm_pollfd_t poll_fds[MAX_BITSTREAM_NUM];
    gm_enc_multi_bitstream_t multi_bs[MAX_BITSTREAM_NUM];

    bitstream_data = (char *)malloc(BITSTREAM_LEN);
    memset(bitstream_data, 0, BITSTREAM_LEN);

    sprintf(filename, "av_sync_video_ch%d.264", ch);
    record_file = fopen(filename, "wb");
    if (record_file == NULL) {
        printf("open file error(%s)! \n", filename);
        exit(1);
    }

    sprintf(filename, "av_sync_video_ch%d.len", ch);
    length_file = fopen(filename, "wb");
    if (length_file == NULL) {
        printf("open file error(%s)! \n", filename);
        exit(1);
    }

    memset(poll_fds, 0, sizeof(poll_fds));

    poll_fds[ch].bindfd = bindfd;
    poll_fds[ch].event = GM_POLL_READ;
    while (enc_exit == 0) {
        ret = gm_poll(poll_fds, MAX_BITSTREAM_NUM, 500);
        if (ret == GM_TIMEOUT) {
            printf("Poll timeout!!");
            continue;
        }

        memset(multi_bs, 0, sizeof(multi_bs));
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
                    fwrite(multi_bs[i].bs.bs_buf,1, multi_bs[i].bs.bs_len, record_file);
                    fprintf(length_file,"%u %u\n", multi_bs[i].bs.bs_len, multi_bs[i].bs.timestamp);
                }
            }
        }
    }
    fclose(record_file);
    fclose(length_file);
    free(bitstream_data);
    return 0;
}

static void *audio_encode_thread(void *arg)
{
    int i, ret;
    int ch = (int)arg;
    char filename[50];
    char *bitstream_data[MAX_BITSTREAM_NUM];
    FILE *bs_fd[MAX_BITSTREAM_NUM], *len_fd[MAX_BITSTREAM_NUM];
    gm_pollfd_t poll_fds[MAX_BITSTREAM_NUM];
    gm_enc_multi_bitstream_t multi_bs[MAX_BITSTREAM_NUM];

    for (i = 0; i < MAX_BITSTREAM_NUM; i++) {
        sprintf(filename, "av_sync_audio_ch%d.aac", i);
        bs_fd[i] = fopen(filename, "wb");
        if (bs_fd[i] == NULL) {
            printf("open file error(%s)! \n", filename);
            exit(1);
        }
        printf("Record file: [%s]\n", filename);
        sprintf(filename, "av_sync_audio_ch%d.len", i);
        len_fd[i] = fopen(filename, "wb");
        if (len_fd[i] == NULL) {
            printf("open file error(%s)! \n", filename);
            exit(1);
        }

        bitstream_data[i] = (char *)malloc(AUDIO_BITSTREAM_LEN);
        if (bitstream_data[i] == 0)
            return 0;
        memset(bitstream_data[i], 0, AUDIO_BITSTREAM_LEN);
    }

    memset(poll_fds, 0, sizeof(poll_fds));
    poll_fds[ch].bindfd = audio_bindfd;
    poll_fds[ch].event = GM_POLL_READ;

    while (enc_exit == 0) {
        ret = gm_poll(poll_fds, MAX_BITSTREAM_NUM, 1000);
        if (ret == GM_TIMEOUT) {
            printf("Poll timeout!!\n");
            continue;
        }

        memset(multi_bs, 0, sizeof(multi_bs));
        for (i = 0; i < MAX_BITSTREAM_NUM; i++) {
            if (poll_fds[i].revent.event != GM_POLL_READ) {
                continue;
            }
            if (poll_fds[i].revent.bs_len > AUDIO_BITSTREAM_LEN) {
                printf("buffer length is not enough! %d, %d\n",
                        poll_fds[i].revent.bs_len, AUDIO_BITSTREAM_LEN);
                continue;
            }
            multi_bs[i].bindfd = audio_bindfd;
            multi_bs[i].bs.bs_buf = bitstream_data[i];
            multi_bs[i].bs.bs_buf_len = AUDIO_BITSTREAM_LEN;
            multi_bs[i].bs.mv_buf = NULL;
            multi_bs[i].bs.mv_buf_len = 0;
        }

        if ((ret = gm_recv_multi_bitstreams(multi_bs, MAX_BITSTREAM_NUM)) < 0)
            printf("Error return value %d\n", ret);
        else {
            for (i = 0; i < MAX_BITSTREAM_NUM; i++) { 
                if (!multi_bs[i].bindfd)
                    continue;
                if (multi_bs[i].retval < 0) {  
                    printf("get bitstreame error! ret = %d\n", ret);
                } else if (multi_bs[i].retval == GM_SUCCESS) {
                    fwrite(multi_bs[i].bs.bs_buf, 1, multi_bs[i].bs.bs_len, bs_fd[i]);
                    fprintf(len_fd[i], "%u %u\n", multi_bs[i].bs.bs_len, multi_bs[i].bs.timestamp);
                }
            }
        }
    }

    for (i = 0; i < MAX_BITSTREAM_NUM; i++) {
        fclose(bs_fd[i]);
        fclose(len_fd[i]);
    }
    return 0;
}

void show_message(void)
{
    printf("You may use commands to know GM standard EVB audio setting:\n");
    printf("  #cat /proc/videograph/vpd/au_grab\n");
    printf("  #cat /proc/videograph/vpd/au_render\n");
    printf("  GM8210_32D1_1080P:\n");
    printf("    input vch: 0\n");
    printf("    output vch: 0(nvp1918)   2(HDMI)\n");
    printf("  GM8210_8HD_1080P:\n");
    printf("    input vch: 8\n");
    printf("    output vch: 3(HDMI)\n");
    printf("  GM8287_16D1_1080P:\n");
    printf("    input vch: 0\n");
    printf("    output vch: 0(nvp1918)   1(HDMI)\n");
    printf("  GM8287_4HD_1080P:\n");
    printf("    input vch: 4\n");
    printf("    output vch: 1(cx20811)   2(HDMI)\n");
    printf("  GM8283_8CH_960H_1080P:\n");
    printf("    input vch: 0\n");
    printf("    output vch: 0(nvp1918)   1(HDMI)\n");
    printf("  GM8282_16CH_960H_1080P:\n");
    printf("    input vch: 0\n");
    printf("    output vch: 0(nvp1918)\n");
    printf("  GM8138/GM8139:\n");
    printf("    input vch: 0\n");
    printf("    output vch: 0(adda302)\n");
    printf(" ps. This sample sets video input vch=0 for all EVB.\n");
}

int main(int argc, char *argv[])
{
    int ch, ret = 0;
    int key;
    DECLARE_ATTR(cap_attr, gm_cap_attr_t);
    DECLARE_ATTR(h264e_attr, gm_h264e_attr_t);
    DECLARE_ATTR(audio_grab_attr, gm_audio_grab_attr_t);
    DECLARE_ATTR(audio_encode_attr, gm_audio_enc_attr_t);

    if (argc < 2) {
        printf("Usage:\n  #./encode_with_av_sync [input vch]\n\n");
        show_message();
        exit(0);
    }

    ch = atoi(argv[1]);
    printf("Audio record from input vch %d\n", ch);


    gm_init();
    gm_get_sysinfo(&gm_system);

    groupfd = gm_new_groupfd();
    audio_groupfd = gm_new_groupfd();

    capture_object = gm_new_obj(GM_CAP_OBJECT);
    encode_object = gm_new_obj(GM_ENCODER_OBJECT);
    audio_grab_object = gm_new_obj(GM_AUDIO_GRAB_OBJECT);
    audio_encode_object = gm_new_obj(GM_AUDIO_ENCODER_OBJECT);

    ch = 0;
    cap_attr.cap_vch = ch;
    cap_attr.path = 3;
    cap_attr.enable_mv_data = 0;
    gm_set_attr(capture_object, &cap_attr);

    h264e_attr.dim.width = gm_system.cap[ch].dim.width;
    h264e_attr.dim.height = gm_system.cap[ch].dim.height;
    h264e_attr.frame_info.framerate = gm_system.cap[ch].framerate;
    h264e_attr.ratectl.mode = GM_CBR;
    h264e_attr.ratectl.gop = 60;
    h264e_attr.ratectl.bitrate = 2048;
    h264e_attr.b_frame_num = 0;
    h264e_attr.enable_mv_data = 0;
    gm_set_attr(encode_object, &h264e_attr);

    bindfd = gm_bind(groupfd, capture_object, encode_object);

    audio_grab_attr.vch = ch;
    gm_set_attr(audio_grab_object, &audio_grab_attr);

    audio_encode_attr.encode_type = GM_AAC;
    audio_encode_attr.bitrate = 32000;
    gm_set_attr(audio_encode_object, &audio_encode_attr);

    audio_bindfd = gm_bind(audio_groupfd, audio_grab_object, audio_encode_object);
 
    if (gm_apply(groupfd) < 0) {
        perror("Error! gm_apply fail, AP procedure something wrong!");
        exit(0);
    }
    if (gm_apply(audio_groupfd) < 0) {
        perror("Error! gm_apply fail, AP procedure something wrong!");
        exit(0);
    }

    ret = pthread_create(&enc_thread_id, NULL, video_encode_thread, (void *)ch);
    if (ret < 0) {
        perror("create encode thread failed");
        return -1;
    }

    ret = pthread_create(&audio_enc_thread_id, NULL, audio_encode_thread, (void *)ch);
    if (ret < 0) {
        perror("create audio encode thread failed");
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
    pthread_join(audio_enc_thread_id, NULL);
    gm_unbind(bindfd);
    gm_unbind(audio_bindfd);
    gm_apply(groupfd);
    gm_apply(audio_groupfd);
    gm_delete_obj(encode_object);
    gm_delete_obj(capture_object);
    gm_delete_obj(audio_grab_object);
    gm_delete_obj(audio_encode_object);
    gm_delete_groupfd(groupfd);
    gm_delete_groupfd(audio_groupfd);
    gm_release();
    return ret;
}
