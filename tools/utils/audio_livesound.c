/**
 * @file audio_playback.c
 *
 * Support platform: GM8210, GM8287, GM8139
 * This sample demos audio livesound
 * Copyright (C) 2013 GM Corp. (http://www.grain-media.com)
 *
 * $Revision: 1.8 $
 * $Date: 2014/06/26 02:55:48 $
 *
 */
/**
 * @example audio_livesound.c
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

gm_system_t gm_system;
void *groupfd;
void *bindfd;
void *audio_grab_object;
void *audio_render_object;

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
    int key, in_ch, out_ch;
    DECLARE_ATTR(audio_grab_attr, gm_audio_grab_attr_t);
    DECLARE_ATTR(audio_render_attr, gm_audio_render_attr_t);

    if (argc < 3) {
        printf("Usage:\n  #./audio_livesound [input vch] [output vch]\n\n");
        show_message();
        exit(0);
    }

    in_ch = atoi(argv[1]);
    out_ch = atoi(argv[2]);
    printf("Audio livesound from input vch %d to output vch %d\n", in_ch, out_ch);

    /**
     * This sample demonstrates audio livesound output to one device.
     * If you implement dual audio output, create the second bind with different vch and apply it.
     * All of the binds should be allocated within the same group.
     * ex: bindfd0 = grab_obj0(vch=0) -> render_obj0(vch=0)  @ group0
     *     bindfd1 = grab_obj1(vch=0) -> render_obj1(vch=1)  @ group0
     *     apply(group0);
     */

    gm_init();
    gm_get_sysinfo(&gm_system);
    groupfd = gm_new_groupfd();

    audio_grab_object = gm_new_obj(GM_AUDIO_GRAB_OBJECT);
    audio_render_object = gm_new_obj(GM_AUDIO_RENDER_OBJECT);

    audio_grab_attr.vch = in_ch;
    gm_set_attr(audio_grab_object, &audio_grab_attr);

    audio_render_attr.vch = out_ch;
    gm_set_attr(audio_render_object, &audio_render_attr);

    bindfd = gm_bind(groupfd, audio_grab_object, audio_render_object);
    if (gm_apply(groupfd) < 0) {
        perror("Error! gm_apply fail, AP procedure something wrong!");
        exit(0);
    }

    printf("Enter q to quit\n");
    while (1) {
        printf("Enter q to exit\n");
        key = getchar();
        if (key == 'q') {
            break;
        }
    }

    gm_unbind(bindfd);
    gm_apply(groupfd);
    gm_delete_obj(audio_grab_object);
    gm_delete_obj(audio_render_object);
    gm_delete_groupfd(groupfd);
    gm_release();
    return 0;
}

