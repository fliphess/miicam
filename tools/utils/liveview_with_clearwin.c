/**
 * @file liveview_with_clearwin.c
 *
 * Support platform: GM8210, GM8287, GM8139
 * This source code is liveview with clearwin sample; 
 * (此示例为预览与清屏程序)
 * Copyright (C) 2012 GM Corp. (http://www.grain-media.com)
 *
 * $Revision: 1.19 $
 * $Date: 2014/04/03 06:45:55 $
 *
 */
/**
 * @example liveview_with_clearwin.c
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include "gmlib.h"

#define LV_MAX_CHANNEL      1

gm_system_t gm_system;
void *groupfd;   // return of gm_new_groupfd()
void *bindfd;    // return of gm_bind()
void *capture_object;
void *win_object;

static int do_clear_win(int mode, int x, int y, int w, int h, unsigned int pattern)
{
    int i, ret = 0;
    gm_clear_window_t cw_str;
    
    cw_str.in_w = 32;
    cw_str.in_h= 32;
    cw_str.in_fmt= GM_FMT_YUV422;
    cw_str.buf = (unsigned char *)malloc(cw_str.in_w * cw_str.in_h * 2 * sizeof(char));
    if (!cw_str.buf) {
        printf("Error buffer allocation\n");
        exit(1);
    }
    for (i = 0; i < (cw_str.in_w * cw_str.in_h / 2); i++)
        *(unsigned int *)(cw_str.buf + 4 * i) = pattern;
    cw_str.out_x = x;
    cw_str.out_y = y;
    cw_str.out_w = w;
    cw_str.out_h = h;
    cw_str.mode = (gm_clear_window_mode_t)mode;

    if (gm_clear_window(GM_LCD0, &cw_str) != 0){
        printf("error in gm_clear_window\n");
        ret = -1;
    }
        
    free(cw_str.buf);
    return ret;
}

int main(void)
{
    int ch, key, position = 0;
    DECLARE_ATTR(cap_attr, gm_cap_attr_t);
    DECLARE_ATTR(win_attr, gm_win_attr_t);

    gm_init(); // GM library initial (GM库初始化)
    gm_get_sysinfo(&gm_system);
    groupfd = gm_new_groupfd(); // new display group fd (获取一个groupfd)

    capture_object = gm_new_obj(GM_CAP_OBJECT); // new capture object (获取一个捕捉对象)
    win_object = gm_new_obj(GM_WIN_OBJECT); // new display object (获取一个显示对象)

    ch = 0;
    cap_attr.cap_vch = ch;

    //GM8210 capture path 0(liveview), 1(CVBS), 2(can scaling), 3(can scaling)
    //GM8139/GM8287 capture path 0(liveview), 1(CVBS), 2(can scaling), 3(can't scaling down)
    cap_attr.path = 0;
    cap_attr.enable_mv_data = 0;
    gm_set_attr(capture_object, &cap_attr); // set capture attribute (设置捕捉属性)

    win_attr.lcd_vch = GM_LCD0;
    win_attr.rect.x = (position % 2) * (gm_system.lcd[0].dim.width / 2);
    win_attr.rect.y = (position / 2) * (gm_system.lcd[0].dim.height / 2);
    win_attr.rect.width = gm_system.lcd[0].dim.width / 2;
    win_attr.rect.height = gm_system.lcd[0].dim.height / 2;
    win_attr.visible = 1;  // 1:visable(显示) 0:invisable(不显示)
    gm_set_attr(win_object, &win_attr); // set display attribute (设置显示属性)

    /** bind channel for liveview (绑定捕捉对象输出到显示对象) */
    bindfd = gm_bind(groupfd, capture_object, win_object); 
    if (gm_apply(groupfd) < 0) { // active setting (使生效)
        perror("Error! gm_apply fail, AP procedure something wrong!");
        exit(0);
    }

    while (1) {
        printf("Enter q to exit, a to switch position, c to fill color\n");
        key = getchar();
        if (key == 'q')
            break;
        else if (key == 'a') {
            int x, y, width, height;
            x = (position % 2) * (gm_system.lcd[0].dim.width / 2);
            y = (position / 2) * (gm_system.lcd[0].dim.height / 2);
            width = gm_system.lcd[0].dim.width / 2;
            height = gm_system.lcd[0].dim.height / 2;

            do_clear_win(GM_ACTIVE_BY_APPLY, x, y, width, height, 0x10801080); // fill black

            position++;
            if (position == 4)
                position = 0;
            win_attr.rect.x = (position % 2) * (gm_system.lcd[0].dim.width / 2);
            win_attr.rect.y = (position / 2) * (gm_system.lcd[0].dim.height / 2);
            gm_set_attr(win_object, &win_attr); // set display attribute (设置显示属性)
            if (gm_apply(groupfd) < 0) {  // active setting (使生效)
                perror("Error! gm_apply fail, AP procedure something wrong!");
                exit(0);
            }
        } else if (key == 'c') {
            int x, y, width, height;
            x = gm_system.lcd[0].dim.width / 3;
            y = gm_system.lcd[0].dim.height / 3;
            width = gm_system.lcd[0].dim.width / 2;
            height = gm_system.lcd[0].dim.height / 2;

            /** fill YUV422 color pattern */
            do_clear_win(GM_ACTIVE_IMMEDIATELY, x, y, width, height, 0x80208020);
        }
    }

    gm_unbind(bindfd);
    gm_apply(groupfd);
    gm_delete_obj(capture_object);
    gm_delete_obj(win_object);
    gm_delete_groupfd(groupfd);
    gm_release();

    return 0;
}
