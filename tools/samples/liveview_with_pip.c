/**
 * @file liveview_with_pip.c
 *
 * Support platform: GM8210, GM8287
 * This sample demos liveview PIP (piture in pipcture)
 * Copyright (C) 2014 GM Corp. (http://www.grain-media.com)
 *
 * $Revision: 1.3 $
 * $Date: 2014/05/05 08:09:44 $
 *
 */
/**
 * @example liveview_with_pip.c
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include "gmlib.h"

gm_system_t gm_system;
void *groupfd;
void *bindfd[4];
void *capture_object[4];
void *win_object[3];

#define CAP_FULL_FRAME          1
#define CAP_CROP                2
void setup_cap_obj(int index, int cap_ch, int cap_mode)
{
    DECLARE_ATTR(cap_attr, gm_cap_attr_t);
    DECLARE_ATTR(crop_attr, gm_crop_attr_t);

    cap_attr.cap_vch = cap_ch;
    cap_attr.path = 0;
    cap_attr.enable_mv_data = 0;
    gm_set_attr(capture_object[index], &cap_attr);
    if (cap_mode == CAP_FULL_FRAME) {
        crop_attr.enabled = 0;
        gm_set_attr(capture_object[index], &crop_attr);
    } else if (cap_mode == CAP_CROP) {
        crop_attr.enabled = 1;
        crop_attr.src_crop_rect.x = (index % 2) * gm_system.cap[0].dim.width / 2;
        crop_attr.src_crop_rect.y = (index / 2) * gm_system.cap[0].dim.height / 2;
        crop_attr.src_crop_rect.width = gm_system.cap[0].dim.width / 2;
        crop_attr.src_crop_rect.height = gm_system.cap[0].dim.height / 2;
        gm_set_attr(capture_object[index], &crop_attr);
    }
}

#define WIN_BACKGROUND_FULLSCEEN    1
#define WIN_4DIV                    2
#define WIN_PIP_LAYER               3
void setup_win_obj(int index, int win_mode)
{
    DECLARE_ATTR(win_attr, gm_win_attr_t);
    win_attr.lcd_vch = GM_LCD0;
    if (win_mode == WIN_PIP_LAYER) {
        win_attr.layer = WIN_LAYER2;
        win_attr.rect.x = gm_system.lcd[0].dim.width / 3;
        win_attr.rect.y = gm_system.lcd[0].dim.height / 3;
        win_attr.rect.width = gm_system.lcd[0].dim.width / 3;
        win_attr.rect.height = gm_system.lcd[0].dim.height / 3;
    } else if (win_mode == WIN_4DIV) { 
        win_attr.layer = WIN_LAYER1;
        win_attr.rect.x = (index % 2) * gm_system.lcd[0].dim.width / 2;
        win_attr.rect.y = (index / 2) * gm_system.lcd[0].dim.height / 2;
        win_attr.rect.width = gm_system.lcd[0].dim.width / 2;
        win_attr.rect.height = gm_system.lcd[0].dim.height / 2;
    } else if (win_mode == WIN_BACKGROUND_FULLSCEEN) {
        win_attr.layer = WIN_LAYER1;
        win_attr.rect.x = 0;
        win_attr.rect.y = 0;
        win_attr.rect.width = gm_system.lcd[0].dim.width;
        win_attr.rect.height = gm_system.lcd[0].dim.height;
    }
    win_attr.visible = 1;
    gm_set_attr(win_object[index], &win_attr);
}

int main(void)
{
    int i, count = 0, key;

    gm_init();
    gm_get_sysinfo(&gm_system);
    groupfd = gm_new_groupfd();

    for (i = 0; i < 4; i++) {
        capture_object[i] = gm_new_obj(GM_CAP_OBJECT);
        win_object[i] = gm_new_obj(GM_WIN_OBJECT);
    }

    printf("Enter '1' to show pip  : ch0 fullframe on the background            \n");
    printf("                       : ch0 crop region at center on the top layer \n");
    printf("Enter '2' to show pap  : ch0 fullframe at upper left                \n");
    printf("                       : ch1 fullframe at upper right               \n");
    printf("                       : ch2 crop region at bottom left             \n");
    printf("                       : ch0 crop region at bottom right            \n");
    printf("Enter 'q' exit\n");

    while (1) {
        key = getchar();
        for (i = 0; i < count; i++)
            gm_unbind(bindfd[i]);
        if (count)
            gm_apply(groupfd);

        if (key == 'q')
            break;
        /*  The restriction of software pip/pap liveview:
         *     The pip channel must be one of running channels.
         *     Only one capture channel(and path) can be duplicate up to 4 times.
         *     The quality of the duplicated channel depends on the largest one 
         *     of their windows sizes.
         *     Recommend to re-bind fd when you are using pip/pap feature
         *     because runtime updating capture 'vch' is not supported.
         */
        if (key == '1') {
            setup_cap_obj(0, 0, CAP_FULL_FRAME);
            setup_cap_obj(1, 0, CAP_CROP);
            setup_win_obj(0, WIN_BACKGROUND_FULLSCEEN);
            setup_win_obj(1, WIN_PIP_LAYER);
            count = 2;
        } else if (key == '2') {
            setup_cap_obj(0, 0, CAP_FULL_FRAME);
            setup_cap_obj(1, 1, CAP_FULL_FRAME);
            setup_cap_obj(2, 2, CAP_CROP);
            setup_cap_obj(3, 0, CAP_CROP);
            setup_win_obj(0, WIN_4DIV);
            setup_win_obj(1, WIN_4DIV);
            setup_win_obj(2, WIN_4DIV);
            setup_win_obj(3, WIN_4DIV);
            count = 4;
        }
        for (i = 0; i < count; i ++)
            bindfd[i] = gm_bind(groupfd, capture_object[i], win_object[i]); 
        gm_apply(groupfd);
    }

    for (i = 0; i < 4; i++)
        gm_delete_obj(capture_object[i]);
    for (i = 0; i < 4; i++)
        gm_delete_obj(win_object[i]);
    gm_delete_groupfd(groupfd);
    gm_release();

    return 0;
}
