/**
 * @file audio_record.c
 *
 * Support platform: GM8210, GM8287, GM8139
 * This sample demos audio record
 * Copyright (C) 2013 GM Corp. (http://www.grain-media.com)
 *
 * $Revision: 1.14 $
 * $Date: 2014/07/15 07:09:26 $
 *
 */
/**
 * @example audio_record.c
 */
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <signal.h>
#include <pthread.h>
#include <sys/time.h>
#include "gmlib.h"

#define BITSTREAM_LEN       12800
#define MAX_BITSTREAM_NUM   1

gm_system_t gm_system;
void *groupfd;
void *bindfd[MAX_BITSTREAM_NUM];
void *audio_grab_object[MAX_BITSTREAM_NUM];
void *audio_encode_object[MAX_BITSTREAM_NUM];
pthread_t thread_id;
int enc_exit = 0;

static void *encode_thread(void *arg)
{
    int i, ret;
    char filename[50];
    char *bitstream_data[MAX_BITSTREAM_NUM];
    FILE *bs_fd[MAX_BITSTREAM_NUM], *len_fd[MAX_BITSTREAM_NUM];
    gm_pollfd_t poll_fds[MAX_BITSTREAM_NUM];
    gm_enc_multi_bitstream_t multi_bs[MAX_BITSTREAM_NUM];

    for (i = 0; i < MAX_BITSTREAM_NUM; i++) {
        sprintf(filename, "CH%d_audio.aac", i);
        bs_fd[i] = fopen(filename, "wb");
        if (bs_fd[i] == NULL) {
            printf("open file error(%s)! \n", filename);
            exit(1);
        }
        printf("Record file: [%s]\n", filename);
        sprintf(filename, "CH%d_audio.len", i);
        len_fd[i] = fopen(filename, "wb");
        if (len_fd[i] == NULL) {
            printf("open file error(%s)! \n", filename);
            exit(1);
        }

        bitstream_data[i] = (char *)malloc(BITSTREAM_LEN);
        if (bitstream_data[i] == 0)
            return 0;
        memset(bitstream_data[i], 0, BITSTREAM_LEN);
    }

    memset(poll_fds, 0, sizeof(poll_fds));
    for (i = 0; i < MAX_BITSTREAM_NUM; i++) {
        poll_fds[i].bindfd = bindfd[i];
        poll_fds[i].event = GM_POLL_READ;
    }

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
            if (poll_fds[i].revent.bs_len > BITSTREAM_LEN) {
                printf("buffer length is not enough! %d, %d\n",
                        poll_fds[i].revent.bs_len, BITSTREAM_LEN);
                continue;
            }
            multi_bs[i].bindfd = bindfd[i];
            multi_bs[i].bs.bs_buf = bitstream_data[i];
            multi_bs[i].bs.bs_buf_len = BITSTREAM_LEN;
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
                    fprintf(len_fd[i], "%d\n", multi_bs[i].bs.bs_len);
                    fflush(bs_fd[i]);
                    fflush(len_fd[i]);
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
}

int main(int argc, char *argv[])
{
    int key, ch, i, ret = 0;
    DECLARE_ATTR(audio_grab_attr, gm_audio_grab_attr_t);
    DECLARE_ATTR(audio_encode_attr, gm_audio_enc_attr_t);

    if (argc < 2) {
        printf("Usage:\n  #./audio_record [input vch]\n\n");
        show_message();
        exit(0);
    }

    ch = atoi(argv[1]);
    printf("Audio record from input vch %d\n", ch);

    gm_init();
    gm_get_sysinfo(&gm_system);
    groupfd = gm_new_groupfd();

    for (i = 0; i < MAX_BITSTREAM_NUM; i++) {
        audio_grab_object[i] = gm_new_obj(GM_AUDIO_GRAB_OBJECT);
        audio_encode_object[i] = gm_new_obj(GM_AUDIO_ENCODER_OBJECT);
        
        audio_grab_attr.vch = ch;

        /* The supported sample_rate/sample_size/channel_type can be lookup 
           from cat /proc/videograph/vpd/au_grab
         */
        audio_grab_attr.sample_rate = 8000;
        audio_grab_attr.sample_size = 16;
        audio_grab_attr.channel_type = GM_MONO;
        gm_set_attr(audio_grab_object[i], &audio_grab_attr);

        /* bitrate: PCM(don't care), ADPCM(16000/32000) others(14500~192000)
           frame_samples: The count of samples per audio frame, suggestion:
             PCM:250~2048
             AAC: 1024*n(mono) 2048*n(stereo)
             ADPCM: 505*n(mono) 498*n(stereo)
             G711: 320*n(mono) 640*n(stereo)
              (n: 1,2,...)
        */
        audio_encode_attr.encode_type = GM_AAC;
        audio_encode_attr.bitrate = 32000; 
        audio_encode_attr.frame_samples = 1024; 
        gm_set_attr(audio_encode_object[i], &audio_encode_attr);

        bindfd[i] = gm_bind(groupfd, audio_grab_object[i], audio_encode_object[i]);
    }
    if (gm_apply(groupfd) < 0) {
        perror("Error! gm_apply fail, AP procedure something wrong!");
        exit(0);
    }

    ret = pthread_create(&thread_id, NULL, encode_thread, (void *)ch);
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

    pthread_join(thread_id, NULL);
    for (ch = 0; ch < MAX_BITSTREAM_NUM; ch++)
        gm_unbind(bindfd[ch]);
    gm_apply(groupfd);    
    for (ch = 0; ch < MAX_BITSTREAM_NUM; ch++) {
        gm_delete_obj(audio_grab_object[ch]);
        gm_delete_obj(audio_encode_object[ch]);
    }
    gm_delete_groupfd(groupfd);
    gm_release();
    return 0;
}

